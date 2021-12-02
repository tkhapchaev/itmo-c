#include <stdio.h>
#include <locale.h>
#include <malloc.h>
#include <string.h>

struct id3header {
    char identifier[3];
    char version[2];
    char flags;
    unsigned char size[4];
};

struct id3frame {
    char id[4];
    unsigned char size[4];
    char flags[2];
};

unsigned int decode_size(unsigned char * ph) {
    unsigned int ha = ph[0];
    unsigned int hb = ph[1];
    unsigned int hc = ph[2];
    unsigned int hd = ph[3];
    hc <<= 7;
    hb <<= 14;
    ha <<= 21;
    return ha | hb | hc | hd;
}

void encode_size(unsigned int sz, unsigned char * ph) {
    ph[3] = sz & 0x7f;
    ph[2] = (sz >> 7) & 0x7f;
    ph[1] = (sz >> 14) & 0x7f;
    ph[9] = (sz >> 21) & 0x7f;
}

char tocp1251(wchar_t bom, wchar_t c) {
    if (bom == 0xfffe) {
        c = ((c & 0xff) << 8) | ((c & 0xff00) >> 8);
    }
    if (c < 128) {
        return c;
    }
    if (c >= 0x410 && c <= 0x44f) {
        c -= 0x410;
        c += 0xc0;
        return c;
    }
    if (c == 0x401)
        return 'Ё';
    if (c == 0x451)
        return 'ё';
    return '?';
}

void show_frame(struct id3frame * frame, char * framedata) {
    unsigned int fsize = decode_size(frame -> size);
    for (int i = 0; i < sizeof(frame -> id); i++) {
        printf("%c", frame -> id[i]);
    }
    printf(": ");
    int ts;
    if (framedata[0] == 1) {
        ts = (fsize - 3) / 2;
        wchar_t * txt = (wchar_t * )(framedata + 1);
        if (frame -> id[0] == 'C' && frame -> id[1] == 'O' && frame -> id[2] == 'M' && frame -> id[3] == 'M') {
            ts -= 2;
            txt = (wchar_t * )(framedata + 5);
        }
        wchar_t bom = * txt++;

        for (int i = 0; i < ts; i++) {
            printf("%c", tocp1251(bom, txt[i]));
        }
    } else {
        ts = fsize - 1;
        char * txt = framedata + 1;
        if (frame -> id[0] == 'C' && frame -> id[1] == 'O' && frame -> id[2] == 'M' && frame -> id[3] == 'M') {
            ts -= 4;
            txt = framedata + 5;
        }

        for (int i = 0; i < ts; i++) {
            printf("%c", txt[i]);
        }
    }
    printf("\n");
}

void get(FILE * f, char * prop_name) {
    struct id3header header;
    struct id3frame frame;
    fread( & header, sizeof(struct id3header), 1, f);
    int hsize = decode_size(header.size);
    unsigned int fsize;
    unsigned char * framedata;
    int fnd = 0;
    while (ftell(f) < hsize) {
        fread( & frame, sizeof(struct id3frame), 1, f);
        fsize = decode_size(frame.size);
        if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0)
            break;
        framedata = malloc(fsize);
        fread(framedata, 1, fsize, f);
        if (memcmp(prop_name, frame.id, 4) == 0) {
            show_frame( & frame, framedata);
            fnd = 1;
        }
        free(framedata);
    }
    if (!fnd) {
        printf("Property not found.\n");
    }
}

wchar_t cp1251towchar(unsigned char c) {
    if (c < 128)
        return c;
    if (c == 'ё')
        return 0x451;
    if (c == 'Ё')
        return 0x401;
    if (c >= 0xc0 && c <= 0xff)
        return c - 0xc0 + 0x410;
    return '?';
}

void set(FILE * f, char * prop_name, unsigned char * prop_value) {
    struct id3header header;
    struct id3frame frame;
    fread( & header, sizeof(struct id3header), 1, f);
    int hsize = decode_size(header.size);
    unsigned char * headerdata = malloc(hsize + sizeof(struct id3header));
    memset(headerdata, 0, hsize + sizeof(struct id3header));
    unsigned char * ptr = headerdata + sizeof(struct id3header);
    unsigned int fsize;
    unsigned char * framedata;
    while (ftell(f) < hsize) {
        fread( & frame, sizeof(struct id3frame), 1, f);
        fsize = decode_size(frame.size);
        if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0)
            break;
        framedata = malloc(fsize);
        fread(framedata, 1, fsize, f);
        if (memcmp(prop_name, frame.id, 4) != 0) {
            memcpy(ptr, & frame, sizeof(struct id3frame));
            ptr += sizeof(struct id3frame);
            memcpy(ptr, framedata, fsize);
            ptr += fsize;
        }
        free(framedata);
    }

    int asciionly = 1;
    for (int i = 0; prop_value[i]; i++) {
        if (prop_value[i] >= 0x80) {
            asciionly = 0;
            break;
        }
    }
    int framesize;
    if (asciionly) {
        framesize = strlen(prop_value) + 1;
    } else {
        framesize = strlen(prop_value) * 2 + 3;
    }
    if (strcmp(prop_name, "COMM") == 0) {
        framesize += 4;
    }
    if ((ptr - headerdata) + framesize + sizeof(struct id3frame) > hsize + sizeof(struct id3header)) {
        printf("Not enough room to save property\n");
        return;
    }
    for (int i = 0; i < 4; i++) {
        * ptr++ = prop_name[i];
    }
    encode_size(framesize, ptr);
    ptr += 4;
    * ptr++ = 0;
    * ptr++ = 0;
    if (asciionly) {
        * ptr++ = 0;
        if (strcmp(prop_name, "COMM") == 0) {
            * ptr++ = 'e';
            * ptr++ = 'n';
            * ptr++ = 'g';
            * ptr++ = 0;
        }
        memcpy(ptr, prop_value, strlen(prop_value));
        ptr += strlen(prop_value);
    } else {
        * ptr++ = 1;
        if (strcmp(prop_name, "COMM") == 0) {
            * ptr++ = 'e';
            * ptr++ = 'n';
            * ptr++ = 'g';
            * ptr++ = 0;
        }

        * ptr++ = 0xff;
        * ptr++ = 0xfe;
        for (int i = 0; prop_value[i]; i++) {
            wchar_t * wptr = (wchar_t * ) ptr;
            * wptr = cp1251towchar(prop_value[i]);
            ptr += 2;
        }

    }
    fseek(f, 0, SEEK_SET);
    memcpy(headerdata, & header, sizeof(struct id3header));
    fwrite(headerdata, 1, hsize + sizeof(struct id3header), f);
}

void show(FILE * f) {
    struct id3header header;
    struct id3frame frame;
    fread( & header, sizeof(struct id3header), 1, f);
    int hsize = decode_size(header.size);
    unsigned int fsize;
    unsigned char * framedata;
    while (ftell(f) < hsize) {
        fread( & frame, sizeof(struct id3frame), 1, f);
        fsize = decode_size(frame.size);
        if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0)
            break;
        framedata = malloc(fsize);
        fread(framedata, 1, fsize, f);
        show_frame( & frame, framedata);
        free(framedata);
    }
}
int main(int argc, char * argv[]) {
    setlocale(LC_ALL, "Russian");
    char filepath[128] = {
            0
    };
    int cmd = -1;
    char prop_name[32] = {
            0
    };
    char prop_value[32] = {
            0
    };
    if (argc < 3) {
        printf("Wrong parameters.\n");
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (memcmp(argv[i], "--filepath=", 11) == 0) {
            strcpy(filepath, & argv[i][11]);
        }
        if (strcmp(argv[i], "--show") == 0) {
            cmd = 0;
        }
        if (memcmp(argv[i], "--get=", 6) == 0) {
            strcpy(prop_name, & argv[i][6]);
            cmd = 1;
        }
        if (memcmp(argv[i], "--set=", 6) == 0) {
            strcpy(prop_name, & argv[i][6]);
            cmd = 2;
        }
        if (memcmp(argv[i], "--value=", 8) == 0) {
            strcpy(prop_value, & argv[i][8]);
        }
    }
    if (filepath[0] == 0) {
        printf("No input file.\n");
        return 0;
    }
    FILE * f = fopen(filepath, "r+b");
    if (f == NULL) {
        printf("Can't open file %s\n", filepath);
        return 0;
    }

    switch (cmd) {
        case 0:
            show(f);
            break;
        case 1:
            if (prop_name[0] == 0) {
                printf("No property.\n");
            } else {
                get(f, prop_name);
            }
            break;
        case 2:
            if (prop_name[0] == 0) {
                printf("No property.\n");
                break;
            }
            if (prop_value[0] == 0) {
                printf("No value.\n");
                break;
            }
            set(f, prop_name, prop_value);
            break;
    }
    fclose(f);
    return 0;
}
