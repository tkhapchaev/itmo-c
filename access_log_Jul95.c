#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct requests_node {
  int value;
  struct requests_node * next;
}
NODE;

typedef struct requests_queue {
  NODE * head;
  NODE * tail;
}
QUEUE;

QUEUE * push(QUEUE * __queue, int key) {
  NODE * newRequest = malloc(sizeof(NODE));
  newRequest -> value = key;
  newRequest -> next = NULL;

  if ((__queue -> head != NULL) && (__queue -> tail != NULL)) {
    __queue -> tail -> next = newRequest;
    __queue -> tail = newRequest;
  } else {
    __queue -> head = __queue -> tail = newRequest;
  }

  return __queue;
}

int show_head(QUEUE * __queue) {
  int key = 0;

  if ((__queue -> head != NULL) && (__queue -> tail != NULL)) {
    key = __queue -> head -> value;
  }

  return key;
}

int pop(QUEUE * __queue) {
  int key = 0;
  NODE * oldRequest;

  if (__queue -> head != NULL) {
    oldRequest = __queue -> head;
    key = oldRequest -> value;
    __queue -> head = __queue -> head -> next;

    free(oldRequest);
  }

  return key;
}

int main() {
  clock_t start, stop;
  start = clock();
  FILE * file_in = fopen("access_log_Jul95", "r");
  QUEUE requests;
  requests.head = NULL;
  requests.tail = NULL;

  int max = 0;
  int line = 0;
  int seconds = 0;
  int listPlace = 1;
  int __continue = 1;
  int toCountLines = 1;
  int toCountErrors = 0;
  int toCountRequests = 0;

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
  char * __pointToString;

  char ERROR_A[10] = "\" 500 ";
  char ERROR_B[10] = "\" 501 ";
  char ERROR_C[10] = "\" 502 ";
  char ERROR_D[10] = "\" 503 ";
  char ERROR_E[10] = "\" 504 ";
  char ERROR_F[10] = "\" 505 ";
  char ERROR_G[10] = "\" 506 ";
  char ERROR_H[10] = "\" 507 ";
  char ERROR_I[10] = "\" 508 ";
  char ERROR_J[10] = "\" 510 ";
  char ERROR_K[10] = "\" 511 ";

  while ((symbol = fgetc(file_in)) != EOF) {
    if (symbol == '\n') toCountLines++;
  }
  fseek(file_in, 0, 0);

  scanf("%d", & seconds);
  if (seconds <= 0) {
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("FATAL :: THE NUMBER IS INVALID \n");
    __continue = 0;
  }

  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  printf("5XX -- Server errors -- list: \n");
  printf("-----------------------------------------------------------------------------------------------------------------------\n");

  for (int i = 0; i < toCountLines; i++) {
    int __flag = 0;
    __pointToString = fgets(string, 550, file_in);
    if ((toFind = strstr(__pointToString, ERROR_A)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_B)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_C)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_D)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_E)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_F)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_G)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_H)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_I)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_J)) != NULL) __flag++;
    if ((toFind = strstr(__pointToString, ERROR_K)) != NULL) __flag++;

    if (__flag > 0) {
      if (i != toCountLines - 1) {
        toCountErrors++;
        printf("%d. %s", listPlace, string);
        listPlace++;
      } else if (i == toCountLines - 1) {
        toCountErrors++;
        printf("%d. %s \n", listPlace, string);
        listPlace++;
      }
    }
  }
  fseek(file_in, 0, 0);

  printf("-----------------------------------------------------------------------------------------------------------------------\n");
  printf("5XX -- Server errors -- counted: %d \n", toCountErrors);
  printf("-----------------------------------------------------------------------------------------------------------------------\n");

  if (__continue != 0) {
    for (int i = 0; i < toCountLines; i++) {
      __pointToString = fgets(string, 550, file_in);
      bracketPosition = strchr(__pointToString, '[');

      if (bracketPosition != NULL) {
        line++;
        int bracketPositionInt = bracketPosition - __pointToString;

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

    printf("Max number of requests in %d seconds -- %d \n", seconds, max);
    printf("%s%.2i%s%.2i%s%.2i%s%.2i%s", "FROM: [", maxDateFrom[0], "/Jul/1995:", maxDateFrom[1], ":", maxDateFrom[2], ":", maxDateFrom[3], " -0400] \n");
    printf("%s%.2i%s%.2i%s%.2i%s%.2i%s", "TO: [", maxDateTo[0], "/Jul/1995:", maxDateTo[1], ":", maxDateTo[2], ":", maxDateTo[3], " -0400] \n");
  }

  stop = clock();
  if (__continue > 0) printf("-----------------------------------------------------------------------------------------------------------------------\n");
  printf("Process finished in %d seconds \n", (int)(stop - start) / CLK_TCK);
  printf("-----------------------------------------------------------------------------------------------------------------------\n");

  fclose(file_in);
}
