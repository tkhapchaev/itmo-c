#include <stdio.h>
#include <locale.h>
#include <malloc.h>
#include <string.h>

typedef struct ID3v2_header {
    char id[3];
    char version[2];
    char flags;
    unsigned char size[4];
}
ID3v2_header;

typedef struct ID3v2_frame {
    char id[4];
    unsigned char size[4];
    char flags[2];
}
ID3v2_frame;

unsigned int decode_size(unsigned char * array_size) {
    unsigned int size_part_1 = array_size[0];
    unsigned int size_part_2 = array_size[1];
    unsigned int size_part_3 = array_size[2];
    unsigned int size_part_4 = array_size[3];
    size_part_3 <<= 7;
    size_part_2 <<= 14;
    size_part_1 <<= 21;
    return size_part_1 | size_part_2 | size_part_3 | size_part_4;
}

void encode_size(unsigned int size, unsigned char * array_size) {
    array_size[3] = size & 0x7f;
    array_size[2] = (size >> 7) & 0x7f;
    array_size[1] = (size >> 14) & 0x7f;
    array_size[9] = (size >> 21) & 0x7f;
}

char to_CP1251(wchar_t byte_order_mark, wchar_t byte) {
    if (byte_order_mark == 0xfffe) {
        byte = ((byte & 0xff) << 8) | ((byte & 0xff00) >> 8);
    }

    if (byte < 128) return byte;
    if (byte >= 0x410 && byte <= 0x44f) {
        byte -= 0x410;
        byte += 0xc0;
        return byte;
    }

    if (byte == 0x401) return 'Ё';
    if (byte == 0x451) return 'Е';
    return '?';
}

void show_frame(ID3v2_frame * frame, char * frame_data) {
    unsigned int frame_size = decode_size(frame -> size);
    for (int i = 0; i < sizeof(frame -> id); i++) {
        printf("%c", frame -> id[i]);
    }

    printf(": ");
    int txt_size;
    if (frame_data[0] == 1) {
        txt_size = (frame_size - 3) / 2;
        wchar_t * txt = (wchar_t * )(frame_data + 1);

        if (frame -> id[0] == 'C' && frame -> id[1] == 'O' && frame -> id[2] == 'M' && frame -> id[3] == 'M') {
            txt_size -= 2;
            txt = (wchar_t * )(frame_data + 5);
        }

        wchar_t bom = * txt++;
        for (int i = 0; i < txt_size; i++) {
            printf("%c", to_CP1251(bom, txt[i]));
        }
    } else {
        txt_size = frame_size - 1;
        char * txt = frame_data + 1;

        if (frame -> id[0] == 'C' && frame -> id[1] == 'O' && frame -> id[2] == 'M' && frame -> id[3] == 'M') {
            txt_size -= 4;
            txt = frame_data + 5;
        }

        for (int i = 0; i < txt_size; i++) {
            printf("%c", txt[i]);
        }
    }

    printf("\n");
}

void get_property(FILE * file_in, char * property_name) {
    ID3v2_header header;
    ID3v2_frame frame;
    fread( & header, sizeof(ID3v2_header), 1, file_in);
    int header_size = decode_size(header.size);
    unsigned int frame_size;
    unsigned char * frame_data;
    int frame_end = 0;
    while (ftell(file_in) < header_size) {
        fread( & frame, sizeof(ID3v2_frame), 1, file_in);
        frame_size = decode_size(frame.size);
        if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0) break;
        frame_data = malloc(frame_size);
        fread(frame_data, 1, frame_size, file_in);

        if (memcmp(property_name, frame.id, 4) == 0) {
            show_frame( & frame, frame_data);
            frame_end = 1;
        }

        free(frame_data);
    }

    if (!frame_end) {
        printf("Error :: property not found.");
    }
}

wchar_t CP1251_to_wchar(unsigned char byte) {
    if (byte < 128) return byte;
    if (byte == 'Е') return 0x451;
    if (byte == 'Ё') return 0x401;
    if (byte >= 0xc0 && byte <= 0xff) return byte - 0xc0 + 0x410;
    return '?';
}

void set_new_property_value(FILE * file_in, char * property_name, unsigned char * property_value) {
    ID3v2_header header;
    ID3v2_frame frame;
    fread( & header, sizeof(ID3v2_header), 1, file_in);
    int header_size = decode_size(header.size);
    unsigned char * headerdata = malloc(header_size + sizeof(ID3v2_header));
    memset(headerdata, 0, header_size + sizeof(ID3v2_header));
    unsigned char * pointer = headerdata + sizeof(ID3v2_header);
    unsigned int frame_size;
    unsigned char * framedata;
    while (ftell(file_in) < header_size) {
        fread( & frame, sizeof(ID3v2_frame), 1, file_in);
        frame_size = decode_size(frame.size);
        if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0) break;
        framedata = malloc(frame_size);
        fread(framedata, 1, frame_size, file_in);

        if (memcmp(property_name, frame.id, 4) != 0) {
            memcpy(pointer, & frame, sizeof(ID3v2_frame));
            pointer += sizeof(ID3v2_frame);
            memcpy(pointer, framedata, frame_size);
            pointer += frame_size;
        }

        free(framedata);
    }

    int ascii_only = 1;
    for (int i = 0; property_value[i]; i++) {
        if (property_value[i] >= 0x80) {
            ascii_only = 0;
            break;
        }
    }

    int framesize;
    if (ascii_only) {
        framesize = strlen(property_value) + 1;
    } else {
        framesize = strlen(property_value) * 2 + 3;
    }

    if (strcmp(property_name, "COMM") == 0) {
        framesize += 4;
    }

    if ((pointer - headerdata) + framesize + sizeof(ID3v2_frame) > header_size + sizeof(ID3v2_header)) {
        printf("Error :: not enough space to save new property value.");
        return;
    }

    for (int i = 0; i < 4; i++) {
        * pointer++ = property_name[i];
    }

    encode_size(framesize, pointer);
    pointer += 4;
    * pointer++ = 0;
    * pointer++ = 0;
    if (ascii_only) {
        * pointer++ = 0;
        if (strcmp(property_name, "COMM") == 0) {
            * pointer++ = 'e';
            * pointer++ = 'n';
            * pointer++ = 'g';
            * pointer++ = 0;
        }

        memcpy(pointer, property_value, strlen(property_value));
        pointer += strlen(property_value);
    } else {
        * pointer++ = 1;
        if (strcmp(property_name, "COMM") == 0) {
            * pointer++ = 'e';
            * pointer++ = 'n';
            * pointer++ = 'g';
            * pointer++ = 0;
        }

        * pointer++ = 0xff;
        * pointer++ = 0xfe;
        for (int i = 0; property_value[i]; i++) {
            wchar_t * wptr = (wchar_t * ) pointer;
            * wptr = CP1251_to_wchar(property_value[i]);
            pointer += 2;
        }

    }

    fseek(file_in, 0, SEEK_SET);
    memcpy(headerdata, & header, sizeof(ID3v2_header));
    fwrite(headerdata, 1, header_size + sizeof(ID3v2_header), file_in);
}

void show(FILE * file_in) {
    ID3v2_header header;
    ID3v2_frame frame;
    fread( & header, sizeof(ID3v2_header), 1, file_in);
    int header_size = decode_size(header.size);
    unsigned int frame_size;
    unsigned char * frame_data;
    while (ftell(file_in) < header_size) {
        fread( & frame, sizeof(ID3v2_frame), 1, file_in);
        frame_size = decode_size(frame.size);
        if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0) break;
        frame_data = malloc(frame_size);
        fread(frame_data, 1, frame_size, file_in);
        show_frame( & frame, frame_data);
        free(frame_data);
    }
}

int main(int argc, char * argv[]) {
    setlocale(LC_ALL, "Russian");
    int cmd = -1;
    char filepath[128] = {
        0
    };
    char prop_name[32] = {
        0
    };
    char prop_value[32] = {
        0
    };

    if (argc < 3) {
        printf("Error :: wrong parameters.");
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
        printf("Error :: can't open input file.");
        return 0;
    }

    FILE * file_in = fopen(filepath, "r+b");
    if (file_in == NULL) {
        printf("Error :: can't open file %s.", filepath);
        return 0;
    }

    switch (cmd) {
    case 0:
        show(file_in);

        break;
    case 1:
        if (prop_name[0] == 0) {
            printf("Error :: no property.");
        } else {
            get_property(file_in, prop_name);
        }

        break;
    case 2:
        if (prop_name[0] == 0) {
            printf("Error :: no property.");
            break;
        }

        if (prop_value[0] == 0) {
            printf("Error :: no value.");
            break;
        }

        set_new_property_value(file_in, prop_name, prop_value);
        break;
    }

    fclose(file_in);
    return 0;
}
