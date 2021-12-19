#include <stdio.h>
#define TYPELEN 128

typedef struct {
  unsigned char num[TYPELEN];
}
uint1024_t;

uint1024_t from_uint(unsigned int x) {
  uint1024_t result = {
    0
  };
  unsigned int * pointer = (unsigned int) & result;
  * pointer = x;

  return result;
}

uint1024_t add_op(uint1024_t x, uint1024_t y) {
  uint1024_t result = {
    0
  };
  int carry = 0;
  for (int i = 0; i < TYPELEN; i++) {
    unsigned int numberOne = x.num[i];
    unsigned int numberTwo = y.num[i];
    unsigned int summary = numberOne + numberTwo + carry;
    result.num[i] = summary;
    carry = summary >> 8;
  }

  return result;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y) {
  uint1024_t result = {
    0
  };
  int carry = 0;
  unsigned int numberOne;
  unsigned int numberTwo;
  unsigned int difference;
  for (int i = 0; i < TYPELEN; i++) {
    numberOne = x.num[i];
    numberTwo = y.num[i];
    difference = numberOne - numberTwo - carry;
    result.num[i] = difference;
    carry = (difference >> 8) & 1;
  }

  return result;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y) {
  uint1024_t result = {
    0
  };
  unsigned int checkingBit;
  for (int i = 0; i < TYPELEN * 8; i++) {
    int byteIndex = i >> 3;
    checkingBit = 1 << (i & 7);
    if ((x.num[byteIndex] & checkingBit) != 0) {
      result = add_op(result, y);
    }

    y = add_op(y, y);
  }

  return result;
}

void printf_value(uint1024_t x) {
  char valueOut[310];
  unsigned char highPart;
  unsigned int lessPart;
  int isZero;
  int j = 0;
  while (1) {
    highPart = 0;
    isZero = 1;
    for (int i = 0; i < TYPELEN; i++) {
      lessPart = (highPart << 8) + x.num[TYPELEN - i - 1];
      highPart = lessPart % 10;
      x.num[TYPELEN - i - 1] = lessPart / 10;

      if (x.num[TYPELEN - i - 1] != 0) {
        isZero = 0;
      }
    }

    valueOut[j++] = highPart + '0';

    if (isZero) {
      break;
    }
  }

  for (int i = 0; i < j / 2; i++) {
    char temporary = valueOut[i];
    valueOut[i] = valueOut[j - i - 1];
    valueOut[j - i - 1] = temporary;
  }

  valueOut[j] = 0;
  printf(valueOut);
}

void scanf_value(uint1024_t * x) {
  uint1024_t ten = from_uint(10);
  uint1024_t digit;
  int symbol;
  for (int i = 0; i < TYPELEN; i++)
    x -> num[i] = 0;

  while (1) {
    symbol = getchar();
    if (symbol == '\n' || symbol == EOF) {
      break;
    }

    if (symbol >= '0' && symbol <= '9') {
      digit = from_uint(symbol - '0');
      * x = mult_op( * x, ten);
      * x = add_op( * x, digit);
    }
  }
}

int main() {
  uint1024_t a = from_uint(1200000010);
  uint1024_t b = add_op(a, a);
  uint1024_t c = mult_op(a, a);
  uint1024_t d = subtr_op(b, a);
  uint1024_t e;
  uint1024_t f;

  printf_value(a);
  printf("\n");
  printf_value(b);
  printf("\n");
  printf_value(c);
  printf("\n");
  printf_value(d);
  printf("\n");
  printf("\n");
  scanf_value( & e);
  scanf_value( & f);
  printf("\n");
  printf_value(add_op(e, f));
  printf("\n");
  printf_value(mult_op(e, f));
  printf("\n");
  printf_value(subtr_op(e, f));
  return 0;
}
