#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <direct.h>
#define BYTE unsigned char
#define WORD uint16_t
#define DWORD uint32_t

#pragma pack(push, 1)
typedef struct bitmap_header {
  WORD type;
  DWORD size;
  WORD reserved_1;
  WORD reserved_2;
  DWORD offsetBits;
}
bitmap_file_header_structure;

#pragma pack(pop)
typedef struct bitmap_info {
  DWORD size;
  DWORD width;
  DWORD height;
  WORD planes;
  WORD bitCount;
  DWORD compression;
  DWORD imageSize;
  DWORD XPixelsPerMeter;
  DWORD YPixelsPerMeter;
  DWORD colorsUsed;
  DWORD colorsImportant;
}
bitmap_info_header_structure;

int count_neighbours(char ** data, int current_position_x, int current_position_y, int max_height, int max_width) {
  int neighbours = 0;
  int ri = current_position_x + 1;
  int rd = current_position_x - 1;
  int ci = current_position_y + 1;
  int cd = current_position_y - 1;
  if (ri == max_height)
    ri = 0;
  if (rd == -1)
    rd = max_height - 1;
  if (ci == max_width)
    ci = 0;
  if (cd == -1)
    cd = max_width - 1;
  neighbours += data[rd][cd] == 1;
  neighbours += data[rd][current_position_y] == 1;
  neighbours += data[rd][ci] == 1;
  neighbours += data[current_position_x][cd] == 1;
  neighbours += data[current_position_x][ci] == 1;
  neighbours += data[ri][cd] == 1;
  neighbours += data[ri][current_position_y] == 1;
  neighbours += data[ri][ci] == 1;
  return neighbours;
}

int main(int argc, char ** argv) {
  BYTE out_filename[128];
  int next_dump = 0;
  char palette[8];
  int palette_size;
  bitmap_file_header_structure bmp_header;
  bitmap_info_header_structure bmp_info;
  char input[128] = {
    0
  };
  char output[128] = {
    0
  };
  int max_iterations = -1;
  int dump_frequency = 1;
  char ** data, ** previous, ** temporary;
  char * current_row;
  FILE * file_in;
  int cmd = 1;
  while (cmd < argc) {
    if (strcmp(argv[cmd], "--input") == 0) {
      cmd++;
      strcpy(input, argv[cmd]);
    }

    if (strcmp(argv[cmd], "--output") == 0) {
      cmd++;
      strcpy(output, argv[cmd]);
    }
    if (strcmp(argv[cmd], "--max_iter") == 0) {
      cmd++;
      sscanf(argv[cmd], "%d", & max_iterations);
    }

    if (strcmp(argv[cmd], "--dump_freq") == 0) {
      cmd++;
      sscanf(argv[cmd], "%d", & dump_frequency);
    }

    cmd++;
  }

  if (input[0] == 0 || output[0] == 0) return 0;
  _mkdir(output);
  file_in = fopen(input, "rb");
  if (file_in == NULL) return NULL;
  if (fread( & bmp_header, 1, sizeof(bmp_header), file_in) != sizeof(bmp_header)) {
    fclose(file_in);
    return NULL;
  }

  if (bmp_header.type != 0x4d42) {
    printf("Error :: %s is not a .bmp file.\n", input);
    fclose(file_in);
    return NULL;
  }

  fread( & bmp_info, 1, sizeof(bitmap_info_header_structure), file_in) != sizeof(bitmap_info_header_structure);
  palette_size = bmp_header.offsetBits - ftell(file_in);
  fread(palette, 1, palette_size, file_in);
  if (bmp_info.compression != 0) {
    printf("Error :: compression is not supported.\n");
    fclose(file_in);
    return NULL;
  }

  int current_string_length = (bmp_header.size - bmp_header.offsetBits) / bmp_info.height;
  if (bmp_info.bitCount != 1) {
    printf("Error :: not a monochrome image.\n");
    fclose(file_in);
    return NULL;

  }

  current_row = malloc(current_string_length);
  data = malloc(bmp_info.height * sizeof(int * ));
  previous = malloc(bmp_info.height * sizeof(int * ));
  temporary = malloc(bmp_info.height * sizeof(int * ));
  for (int i = 0; i < bmp_info.height; i++) {
    data[i] = malloc(bmp_info.width);
    previous[i] = calloc(bmp_info.width, 1);
    temporary[i] = malloc(bmp_info.width);
    fseek(file_in, bmp_header.offsetBits + (bmp_info.height - i - 1) * current_string_length, SEEK_SET);
    fread(current_row, 1, current_string_length, file_in);
    for (int j = 0; j < bmp_info.width; j++) {
      data[i][j] = (current_row[j / 8] >> (7 - j % 8)) & 1;
    }
  }

  fclose(file_in);
  int current_iteration = 0;
  while (max_iterations == -1 || current_iteration < max_iterations) {
    unsigned int counter;
    unsigned int j;
    unsigned int k;
    for (j = 0; j < bmp_info.height && memcmp(data[j], previous[j], bmp_info.width) == 0; j++);
    if (j == bmp_info.height) {
      break;
    }

    for (j = 0, k = bmp_info.width; j < bmp_info.height && k == bmp_info.width; j++)
      for (k = 0; k < bmp_info.width && data[j][k] == ' '; k++);

    if (j == bmp_info.height) {
      break;
    }

    if (current_iteration == next_dump) {
      sprintf(out_filename, "%s\\%06d.bmp", output, current_iteration);
      file_in = fopen(out_filename, "wb");
      fwrite( & bmp_header, 1, sizeof(bmp_header), file_in);
      fwrite( & bmp_info, 1, sizeof(bmp_info), file_in);
      fwrite(palette, 1, palette_size, file_in);
      for (int i = 0; i < bmp_info.height; i++) {
        memset(current_row, 0, current_string_length);
        for (int j = 0; j < bmp_info.width; j++) {
          current_row[j / 8] |= data[bmp_info.height - 1 - i][j] << (7 - j % 8);
        }

        fwrite(current_row, 1, current_string_length, file_in);
      }

      fclose(file_in);
      next_dump += dump_frequency;
    }

    for (j = 0; j < bmp_info.height; j++) memcpy(previous[j], data[j], bmp_info.width);
    for (j = 0; j < bmp_info.height; j++) {
      for (k = 0; k < bmp_info.width; k++) {
        counter = count_neighbours(data, j, k, bmp_info.height, bmp_info.width);
        if (data[j][k] == 0) {
          if (counter == 3)
            temporary[j][k] = 1;
          else
            temporary[j][k] = 0;
        }

        if (data[j][k] == 1) {
          if (counter < 2 || counter > 3)
            temporary[j][k] = 0;
          else
            temporary[j][k] = 1;
        }
      }
    }

    for (j = 0; j < bmp_info.height; j++) memcpy(data[j], temporary[j], bmp_info.width);
    current_iteration++;
  }

  for (int i = 0; i < bmp_info.height; i++) {
    free(data[i]);
    free(previous[i]);
    free(temporary[i]);
  }

  free(data);
  free(previous);
  free(temporary);
  free(current_row);
  return 0;
}
