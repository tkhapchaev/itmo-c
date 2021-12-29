#include <iostream>
#define _QUEUE_SIZE 1000000
using namespace std;

typedef struct myQueue {
  int maxQueue[_QUEUE_SIZE];
  int prev = 0;
  int next = 1;
}
myQueue;

void addToQueue(myQueue * Queue, int value) {
  if (Queue -> prev < _QUEUE_SIZE - 1) {
    Queue -> prev += 1;
    Queue -> maxQueue[Queue -> prev] = value;
  }
}

int deleteFromQueue(myQueue * Queue) {
  int tmp = Queue -> maxQueue[Queue -> next];
  Queue -> next += 1;
  return tmp;
}

int main() {
  cin.tie(0);
  ios_base::sync_with_stdio(0);
  freopen("queue.in", "r", stdin);
  freopen("queue.out", "w", stdout);
  int N;
  cin >> N;
  int number = 0;
  char sign;
  myQueue * Q = new myQueue[sizeof(myQueue * )];
  for (int i = 0; i < N; i++) {
    cin >> sign;

    if (sign == '-') {
      cout << deleteFromQueue(Q) << endl;
    }

    if (sign == '+') {
      cin >> number;
      addToQueue(Q, number);
    }
  }

  fclose(stdin);
  fclose(stdout);
  return 0;
}
