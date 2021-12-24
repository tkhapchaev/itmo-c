#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define _NUMBER_OF_ARCHIVED_FILES 0xC
#define _MAX_ARCHIVE_SIZE 0xFF
#define _START_TAGGING 0x17
#define _SPLITTER_SIZE 0x6
#define _MAX_INT uint64_t
#define BYTE unsigned char
_MAX_INT pointer = 0;
_MAX_INT end_tagging = 0;
_MAX_INT to_count_sizes[_MAX_ARCHIVE_SIZE];

_MAX_INT get_file_size(BYTE file_name[]) {
  FILE * file = fopen(file_name, "rb");
  if (file == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  fseek(file, 0, SEEK_END);
  _MAX_INT size = ftell(file);
  fclose(file);
  return size;
}

void add_file_to_archive(BYTE archive_name[], BYTE file_name[]) {
  FILE * archive = fopen(archive_name, "ab");
  FILE * file = fopen(file_name, "rb");
  _MAX_INT size = get_file_size(file_name);
  BYTE * buffer = (BYTE * ) calloc(size, sizeof(BYTE));
  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  if (file == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  if (pointer == 0) fseek(archive, 0, 0);
  else fseek(archive, to_count_sizes[pointer - 1], 0);
  if (buffer == NULL) {
    printf("%s", "Error :: the file is too large.\n");
    exit(0);
  }

  for (_MAX_INT i = 0; i < size; i++) {
    fread( & buffer[i], sizeof(BYTE), 1, file);
  }

  for (_MAX_INT j = 0; j < size; j++) {
    fwrite( & buffer[j], sizeof(BYTE), 1, archive);
  }

  BYTE to_split[_SPLITTER_SIZE] = " $eof ";
  fwrite(to_split, sizeof(BYTE), _SPLITTER_SIZE, archive);
  to_count_sizes[pointer] = size;
  pointer++;
  free(buffer);
  fclose(file);
  fclose(archive);
  remove(file_name);
}

void init(BYTE archive_name[], BYTE value) {
  FILE * archive = fopen(archive_name, "wb");
  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  fwrite("ARCHIVED :: ", sizeof(BYTE), 12, archive);
  fwrite( & value, sizeof(BYTE), 1, archive);
  fwrite(" FILES :: ", sizeof(BYTE), 10, archive);
  fseek(archive, 0, SEEK_END);
  fclose(archive);
}

void write_file_tag(BYTE archive_name[], BYTE file_name[]) {
  FILE * archive = fopen(archive_name, "ab");
  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  fwrite(file_name, sizeof(BYTE), strlen(file_name), archive);
  fwrite(" :: ", sizeof(BYTE), 4, archive);
  fclose(archive);
}

void show_archived_files_list(BYTE archive_name[], int call_mode) {
  FILE * archive = fopen(archive_name, "rb");
  BYTE archive_size;
  BYTE name_buffer[0x200];
  BYTE chars[0x4];
  _MAX_INT tag_end = 0, i = 0;
  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  fseek(archive, _NUMBER_OF_ARCHIVED_FILES, 0);
  fread( & archive_size, sizeof(BYTE), 1, archive);
  fseek(archive, _START_TAGGING, 0);
  while (tag_end != archive_size) {
    fseek(archive, _START_TAGGING + i, 0);
    chars[0] = fgetc(archive);
    chars[1] = fgetc(archive);
    chars[2] = fgetc(archive);
    chars[3] = fgetc(archive);
    if (chars[0] == ' ' && chars[1] == ':' && chars[2] == ':' && chars[3] == ' ') {
      tag_end++;
    }

    name_buffer[i] = chars[0];
    end_tagging = ftell(archive);
    i++;
  }

  if (call_mode == 1) printf("ARCHIVED %d FILES :: %s", archive_size, name_buffer);
  fclose(archive);
}

void prepare_sizes_list(BYTE archive_name[]) {
  FILE * archive = fopen(archive_name, "ab");
  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  show_archived_files_list(archive_name, 0);
  fseek(archive, end_tagging + 1, 0);
  fwrite("SIZE OF :: ", sizeof(BYTE), 11, archive);
  fclose(archive);
}

void write_file_size(BYTE archive_name[], BYTE file_name[]) {
  FILE * archive = fopen(archive_name, "ab");
  BYTE file_size[0x14] = {
    0
  };

  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  ulltoa(get_file_size(file_name), file_size, 0xA);
  fwrite(file_size, 0x14, 1, archive);
  fwrite(" ", sizeof(BYTE), 1, archive);
  fclose(archive);
}

void extract_files(BYTE archive_name[]) {
  FILE * archive = fopen(archive_name, "rb");
  _MAX_INT tag_end = 0, seek = _START_TAGGING;
  _MAX_INT tag_number = 0;
  _MAX_INT char_in_tag = 0;
  BYTE archive_size = 0;
  BYTE names[0x100][0x400];
  BYTE buffer[0x1];
  BYTE chars[0x4];
  BYTE file_size[0x14] = {
    0
  };
  _MAX_INT sizes[0x100] = {
    0
  };
  for (int i = 0; i < 0x100; i++) {
    for (int j = 0; j < 0x400; j++) {
      names[i][j] = 0;
    }
  }

  if (archive == NULL) {
    printf("%s", "Error :: unable to open the file.\n");
    exit(0);
  }

  fseek(archive, _NUMBER_OF_ARCHIVED_FILES, 0);
  fread( & archive_size, sizeof(BYTE), 1, archive);
  show_archived_files_list(archive_name, 0);
  fseek(archive, end_tagging + 0xB, 0);
  for (int size_counter = 0; size_counter < archive_size; size_counter++) {
    fread( & file_size, 20, 1, archive);
    sizes[size_counter] = strtoull(file_size, NULL, 10);
    _MAX_INT current_seek_position = ftell(archive);
    fseek(archive, current_seek_position + 1, 0);
  }

  fseek(archive, _START_TAGGING, 0);
  for (int i = 0; i < archive_size; i++) {
    tag_end = 0;
    char_in_tag = 0;
    while (tag_end == 0) {
      fseek(archive, seek, 0);
      chars[0] = fgetc(archive);
      chars[1] = fgetc(archive);
      chars[2] = fgetc(archive);
      chars[3] = fgetc(archive);
      if (chars[0] == ' ' && chars[1] == ':' && chars[2] == ':' && chars[3] == ' ') {
        seek = seek + 4;
        tag_number++;
        tag_end = 1;
        break;
      }

      names[tag_number][char_in_tag] = chars[0];
      char_in_tag++;
      seek++;
    }
  }

  fseek(archive, end_tagging + 0xB + (0x15 * archive_size), 0);
  for (int size_counter = 0; size_counter < archive_size; size_counter++) {
    _MAX_INT current_seek_position = 0;
    FILE * file = fopen(names[size_counter], "wb");
    if (file == NULL) {
      printf("%s", "Error :: unable to open the file.\n");
      exit(0);
    }

    for (seek = 0; seek < sizes[size_counter]; seek++) {
      fread( & buffer[0], sizeof(BYTE), 1, archive);
      fwrite( & buffer[0], sizeof(BYTE), 1, file);
      current_seek_position = ftell(archive);
    }

    fseek(archive, current_seek_position + 6, 0);
    fclose(file);
  }

  fclose(archive);
  remove(archive_name);
}

int main(int argc, char * argv[]) {
  if (argc < 4) {
    printf("%s", "Error :: too few arguments.\n");
    return 0;
  }

  if (strcmp(argv[1], "--file") == 0) {
    if (strcmp(argv[3], "--create") == 0) {
      if (argc < 5) {
        printf("%s", "Error :: too few arguments.\n");
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
  } else printf("Error :: unknown command.\n");
  return 0;
}
