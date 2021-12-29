#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct TNode {
  int value;
  struct requests_node * next;
}
TNode;

typedef struct TQueue {
  TNode * head;
  TNode * tail;
}
TQueue;

TQueue * push(TQueue * queue, int key) {
  TNode * newRequest = malloc(sizeof(TNode));
  newRequest -> value = key;
  newRequest -> next = NULL;
  if ((queue -> head != NULL) && (queue -> tail != NULL)) {
    queue -> tail -> next = newRequest;
    queue -> tail = newRequest;
  } else {
    queue -> head = queue -> tail = newRequest;
  }

  return queue;
}

int show_head(TQueue * queue) {
  int key = 0;
  if ((queue -> head != NULL) && (queue -> tail != NULL)) {
    key = queue -> head -> value;
  }

  return key;
}

int pop(TQueue * queue) {
  int key = 0;
  TNode * oldRequest;
  if (queue -> head != NULL) {
    oldRequest = queue -> head;
    key = oldRequest -> value;
    queue -> head = queue -> head -> next;

    free(oldRequest);
  }

  return key;
}

int main() {
  clock_t start, stop;
  start = clock();
  FILE * file_in = fopen("access_log_Jul95", "r");
  TQueue requests;
  requests.head = NULL;
  requests.tail = NULL;
  int max = 0;
  int line = 0;
  int seconds = 0;
  int listPlace = 1;
  int toCountLines = 1;
  int toCountErrors = 0;
  int toCountRequests = 0;
  int continue_flag = 1;
  int maxDateTo[4] = {
    0
  };
  int maxDateFrom[4] = {
    0
  };
  char symbol;
  char string[550];
  char * toFind;
  char * bracketPosition;
  char * stringPointer;
  char error_0[10] = "\" 500 ";
  char error_1[10] = "\" 501 ";
  char error_2[10] = "\" 502 ";
  char error_3[10] = "\" 503 ";
  char error_4[10] = "\" 504 ";
  char error_5[10] = "\" 505 ";
  char error_6[10] = "\" 506 ";
  char error_7[10] = "\" 507 ";
  char error_8[10] = "\" 508 ";
  char error_10[10] = "\" 510 ";
  char error_11[10] = "\" 511 ";
  while ((symbol = fgetc(file_in)) != EOF) {
    if (symbol == '\n') toCountLines++;
  }

  fseek(file_in, 0, 0);
  scanf("%d", & seconds);
  if (seconds <= 0) {
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("Error :: the number is invalid.\n");
    continue_flag = 0;
  }

  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  printf("5XX -- Server errors -- list:\n");
  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  for (int i = 0; i < toCountLines; i++) {
    int found = 0;
    stringPointer = fgets(string, 550, file_in);
    if ((toFind = strstr(stringPointer, error_0)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_1)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_2)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_3)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_4)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_5)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_6)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_7)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_8)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_10)) != NULL) found++;
    if ((toFind = strstr(stringPointer, error_11)) != NULL) found++;
    if (found > 0) {
      if (i != toCountLines - 1) {
        toCountErrors++;
        printf("%d. %s", listPlace, string);
        listPlace++;
      } else if (i == toCountLines - 1) {
        toCountErrors++;
        printf("%d. %s\n", listPlace, string);
        listPlace++;
      }
    }
  }

  fseek(file_in, 0, 0);
  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  printf("5XX -- Server errors -- counted: %d.\n", toCountErrors);
  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  if (continue_flag != 0) {
    for (int i = 0; i < toCountLines; i++) {
      stringPointer = fgets(string, 550, file_in);
      bracketPosition = strchr(stringPointer, '[');
      if (bracketPosition != NULL) {
        line++;
        int bracketPositionInt = bracketPosition - stringPointer;
        int dayTens = string[bracketPositionInt + 1] - '0';
        int dayUnits = string[bracketPositionInt + 2] - '0';
        int daySummary = 10 * dayTens + dayUnits;
        int hoursTens = string[bracketPositionInt + 13] - '0';
        int hoursUnits = string[bracketPositionInt + 14] - '0';
        int hoursSummary = 10 * hoursTens + hoursUnits;
        int minutesTens = string[bracketPositionInt + 16] - '0';
        int minutesUnits = string[bracketPositionInt + 17] - '0';
        int minutesSummary = 10 * minutesTens + minutesUnits;
        int secondsTens = string[bracketPositionInt + 19] - '0';
        int secondsUnits = string[bracketPositionInt + 20] - '0';
        int secondsSummary = 10 * secondsTens + secondsUnits;
        int result = (daySummary * 86400 + hoursSummary * 3600 + minutesSummary * 60 + secondsSummary) - 86400;
        if (i == 0) push( & requests, result);
        else if (i != 0) {
          push( & requests, result);
          toCountRequests++;
          if (toCountRequests > max) {
            max = toCountRequests;
            maxDateFrom[0] = ((show_head( & requests) / 86400) + 1);
            maxDateFrom[1] = ((show_head( & requests) % 86400) / 3600);
            maxDateFrom[2] = (((show_head( & requests) % 86400) % 3600) / 60);
            maxDateFrom[3] = ((((show_head( & requests) % 86400) % 3600) % 60));
            maxDateTo[0] = daySummary;
            maxDateTo[1] = hoursSummary;
            maxDateTo[2] = minutesSummary;
            maxDateTo[3] = secondsSummary;
          }

          while (abs(show_head( & requests) - result) >= seconds) {
            pop( & requests);
            toCountRequests--;
          }
        }
      }
    }

    printf("Max number of requests in %d seconds -- %d.\n", seconds, max);
    printf("%s%.2i%s%.2i%s%.2i%s%.2i%s", "From: [", maxDateFrom[0], "/Jul/1995:", maxDateFrom[1], ":", maxDateFrom[2], ":", maxDateFrom[3], " -0400]\n");
    printf("%s%.2i%s%.2i%s%.2i%s%.2i%s", "To: [", maxDateTo[0], "/Jul/1995:", maxDateTo[1], ":", maxDateTo[2], ":", maxDateTo[3], " -0400]\n");
  }

  stop = clock();
  if (continue_flag > 0) printf("-----------------------------------------------------------------------------------------------------------------------\n");
  printf("Process finished in %d seconds.\n", (int)(stop - start) / CLK_TCK);
  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  fclose(file_in);
  return 0;
}
