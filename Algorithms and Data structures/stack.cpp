#include <iostream>
using namespace std;

typedef struct TStack {
  int number;
  TStack * prev;
}
TStack;

TStack * stack;
void addToStack(int value) {
  TStack * tmp;
  tmp = new TStack();
  tmp -> number = value;
  tmp -> prev = stack;
  stack = tmp;
}

void deleteFromStack() {
  TStack * tmp;
  tmp = stack;
  cout << tmp -> number << endl;
  stack = stack -> prev;
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
