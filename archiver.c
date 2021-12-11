#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define _NUMBER_OF_ARCHIVED_FILES 12
#define _MAX_ARCHIVE_SIZE 0xFF
#define _START_TAGGING 23
#define _SPLITTER_SIZE 6

uint64_t POINTER = 0;
uint64_t to_count_sizes[_MAX_ARCHIVE_SIZE];
uint64_t end_tagging = 0;

uint64_t get_file_size(unsigned char file_name[]) {
    FILE* file = fopen(file_name, "rb");

    if (file == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);

    fclose(file);
    return size;
}

void add_file_to_archive(unsigned char archive_name[], unsigned char file_name[]) {
    FILE* archive = fopen(archive_name, "ab");
    FILE* file = fopen(file_name, "rb");

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return ;
    }

    if (file == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return ;
    }

    if (POINTER == 0) fseek(archive, 0, 0);
    else fseek(archive, to_count_sizes[POINTER - 1], 0);

    uint64_t size = get_file_size(file_name);
    unsigned char* buffer = (unsigned char* )calloc(size, sizeof(unsigned char));

    if (buffer == NULL) {
        printf("%s", "FATAL :: THE FILE IS TOO LARGE ");
        return ;
    }

    for (uint64_t i = 0; i < size; i++) {
        fread(&buffer[i], sizeof(unsigned char), 1, file);
    }

    for (uint64_t j = 0; j < size; j++) {
        fwrite(&buffer[j], sizeof(unsigned char), 1, archive);
    }

    unsigned char to_split[_SPLITTER_SIZE] = " $eof ";
    fwrite(to_split, sizeof(unsigned char), _SPLITTER_SIZE, archive);
    to_count_sizes[POINTER] = size;
    POINTER++;
    free(buffer);
    fclose(file);
    fclose(archive);
    remove(file_name);
}

void init(unsigned char archive_name[], unsigned char value) {
    FILE* archive = fopen(archive_name, "wb");

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return ;
    }

    fwrite("ARCHIVED :: ", sizeof(unsigned char), 12, archive);
    fwrite(&value, sizeof(unsigned char), 1, archive);
    fwrite(" FILES :: ", sizeof(unsigned char), 10, archive);

    fseek(archive, 0, SEEK_END);
    fclose(archive);
}

void write_file_tag(unsigned char archive_name[], unsigned char file_name[]) {
    FILE* archive = fopen(archive_name, "ab");

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return ;
    }

    fwrite(file_name, sizeof(unsigned char), strlen(file_name), archive);
    fwrite(" :: ", sizeof(unsigned char), 4, archive);
    fclose(archive);
}

void show_archived_files_list(unsigned char archive_name[], int call_mode) {
    FILE* archive = fopen(archive_name, "rb");
    unsigned char archive_size;
    unsigned char NAME_BUFFER[4096];
    unsigned char bytes[4];

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE");
        return ;
    }

    uint32_t TAG_END = 0, i = 0;
    fseek(archive, _NUMBER_OF_ARCHIVED_FILES, 0);
    fread(&archive_size, sizeof(unsigned char), 1, archive);
    fseek(archive, _START_TAGGING, 0);

    while (TAG_END != archive_size) {
        fseek(archive, _START_TAGGING + i, 0);
        bytes[0] = fgetc(archive);
        bytes[1] = fgetc(archive);
        bytes[2] = fgetc(archive);
        bytes[3] = fgetc(archive);

        if (bytes[0] == ' ' && bytes[1] == ':' && bytes[2] == ':' && bytes[3] == ' ') {
            TAG_END++;
        }

        NAME_BUFFER[i] = bytes[0]; i++;
        end_tagging = ftell(archive);
    }

    if (call_mode == 1) printf("ARCHIVED [%d] FILES :: %s ", archive_size, NAME_BUFFER);
    fclose(archive);
}

void prepare_sizes_list(unsigned char archive_name[]) {
    FILE* archive = fopen(archive_name, "ab");

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return ;
    }

    show_archived_files_list(archive_name, 0);
    fseek(archive, end_tagging + 1, 0);
    fwrite("SIZE OF :: ", sizeof(unsigned char), 11, archive);
    fclose(archive);
}

void write_file_size(unsigned char archive_name[], unsigned char file_name[]) {
    FILE* archive = fopen(archive_name, "ab");

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE ");
        return ;
    }

    unsigned char file_size[0x14] = {0};
    itoa(get_file_size(file_name), file_size, 10);
    fwrite(file_size, 20, 1, archive);
    fwrite(" ", sizeof(unsigned char), 1, archive);
    fclose(archive);
}

void extract_files(unsigned char archive_name[]) {
    FILE* archive = fopen(archive_name, "rb");
    unsigned char file_size[20] = {0};
    unsigned char archive_size = 0;

    if (archive == NULL) {
        printf("%s", "FATAL :: UNABLE TO OPEN THE FILE");
        return ;
    }

    fseek(archive, _NUMBER_OF_ARCHIVED_FILES, 0);
    fread(&archive_size, sizeof(unsigned char), 1, archive);
    show_archived_files_list(archive_name, 0);
    fseek(archive, end_tagging + 0xB, 0);

    fread(&file_size, 20, 1, archive);
    fclose(archive);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("%s", "FATAL :: TOO FEW ARGUMENTS ");
        return 0;
    }

    if (strcmp(argv[1], "--file") == 0) {
        if (strcmp(argv[3], "--create") == 0) {
            if (argc < 5) {
                printf("%s", "FATAL :: TOO FEW ARGUMENTS ");
                return 0;
            }

            init(argv[2], argc - 4);
            for (int i = 0; i < argc - 4; i++) {
                write_file_tag(argv[2], argv[i + 4]);
            }

            prepare_sizes_list(argv[2]);
            for (int j = 0; j < argc - 4; j++) {
                write_file_size(argv[2], argv[j + 4]);
            }

            for (int k = 0; k < argc - 4; k++) {
                add_file_to_archive(argv[2], argv[k + 4]);
            }

            return 0;
        }

        if (strcmp(argv[3], "--extract") == 0) {
            extract_files(argv[2]);
            return 0;
        }

        if (strcmp(argv[3], "--list") == 0) {
            show_archived_files_list(argv[2], 1);
            return 0;
        }

        return 0;
    }

    else printf("FATAL :: UNKNOWN COMMAND ");
    return 0;
}