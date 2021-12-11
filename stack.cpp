#include <iostream>
using namespace std;

typedef struct myStack {
  int number;
  myStack * prev;
}
myStack;

myStack * Stack;

void addToStack(int value) {
  myStack * tmp;
  tmp = new myStack();
  tmp -> number = value;
  tmp -> prev = Stack;
  Stack = tmp;
}

void deleteFromStack() {
  myStack * tmp;
  tmp = Stack;
  cout << tmp -> number << endl;
  Stack = Stack -> prev;
}

int main() {
  freopen("stack.in", "r", stdin);
  freopen("stack.out", "w", stdout);

  cin.tie(0);
  ios_base::sync_with_stdio(0);

  int N;
  cin >> N;
  char sign;
  int A;

  for (int i = 0; i < N; i++) {
    cin >> sign;

    if (sign == '-') {
      deleteFromStack();
    }

    if (sign == '+') {
      cin >> A;
      addToStack(A);
    }
  }

  fclose(stdin);
  fclose(stdout);
  return 0;
}
