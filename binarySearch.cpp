#include <iostream>
using namespace std;

int myBinarySearch(int A[], int Number, int key) {
  int left = -1;
  int right = Number;
  int middle = 0;
  while (left < right - 1) {
    middle = (left + right) / 2;
    if (A[middle] < key) {
      left = middle;
    } else {
      right = middle;
    }
  }

  if (right < Number && A[right] == key) {
    return right + 1;
  } else {
    return -1;
  }
}

int myBinarySearchLast(int A[], int Number, int key) {
  int left = -1;
  int right = Number;
  int middle = 0;
  while (left < right - 1) {
    middle = (left + right) / 2;
    if (A[middle] <= key) {
      left = middle;
    } else {
      right = middle;
    }
  }

  if (left > 0 && A[left] == key) {
    return left + 1;
  } else {
    return -1;
  }
}

int main() {
  freopen("binsearch.in", "r", stdin);
  freopen("binsearch.out", "w", stdout);

  int N = 0;
  cin >> N;

  int arr[N] = {
    0
  };
  int counter = 0;
  int funclast = 0;
  int funcfirst = 0;
  int arr_counting[N] = {
    0
  };

  for (int i = 0; i < N; i++) {
    cin >> arr[i];
  }

  int M;
  cin >> M;
  int arr_requests[M] = {
    0
  };

  for (int i = 0; i < M; i++) {
    cin >> arr_requests[i];
  }

  for (int i = 0; i < M; i++) {
    counter = arr_requests[i];
    funcfirst = myBinarySearch(arr, N, counter);
    funclast = myBinarySearchLast(arr, N, counter);

    cout << funcfirst << " " << funclast << endl;

  }
}
