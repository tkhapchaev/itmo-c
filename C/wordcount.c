#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {
  long wordsNumber = 0, bytesNumber = 0, linesNumber = 0, symbolsNumber = 0;
  int position = 0, isEmpty = 0, isUnknown = 1;
  char symbol;
  FILE * file_in = fopen(argv[2], "r");
  while ((symbol = fgetc(file_in)) != EOF) {
    symbolsNumber = symbolsNumber + 1;
    if (symbol == '\n')
      linesNumber = linesNumber + 1;
  }

  if (symbolsNumber == 0) {
    printf("%s", "FATAL :: FILE IS EMPTY");
    linesNumber = 0;
    isEmpty = 1;
  } else
    linesNumber = linesNumber + 1;

  fseek(file_in, 0, 0);
  while ((symbol = fgetc(file_in)) != EOF) {
    if (symbol == ' ' || symbol == '\n' || symbol == '\t')
      position = 0;
    else if (position == 0) {
      position = 1;
      wordsNumber = wordsNumber + 1;
    }
  }

  if (isEmpty == 0) {
    if ((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "--lines") == 0)) {
      printf("%s %ld", "Lines:", linesNumber);
      isUnknown = 0;
    }

    if ((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--bytes") == 0)) {
      bytesNumber = symbolsNumber + linesNumber - 1;
      printf("%s %ld", "Bytes:", bytesNumber);
      isUnknown = 0;
    }

    if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--words") == 0)) {
      printf("%s %ld", "Words:", wordsNumber);
      isUnknown = 0;
    }

    if (isUnknown == 1) {
      printf("%s", "FATAL :: UNKNOWN COMMAND");
    }
  }

  fclose(file_in);
  return 0;
}
