#include <iostream>
using namespace std;

int main() {
  freopen("isheap.in", "r", stdin);
  freopen("isheap.out", "w", stdout);

  int N;
  cin >> N;
  int arr[N];

  for (int i = 1; i <= N; i++) {
    cin >> arr[i];
  }

  bool isHeap = true;

  for (int i = 1; i < N / 2; i++) {
    if ((2 * i > N) || (arr[i] > arr[2 * i])) isHeap = false;

    if ((2 * i + 1 > N) || (arr[i] > arr[2 * i + 1])) isHeap = false;
  }

  if (isHeap) cout << "YES";
  else cout << "NO";

  fclose(stdin);
  fclose(stdout);
  return 0;
}
