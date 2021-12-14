#include <iostream>
using namespace std;

int priorityQueue[100000] = {
  0
};
int box[100000] = {
  0
};

int leftChild(int index) {
  return 2 * index;
}

int rightChild(int index) {
  return 2 * index + 1;
}

int parent(int index) {
  return index / 2;
}

void minHeapify(int arr[], int index, int __size) {
  int left = leftChild(index);
  int right = rightChild(index);
  int smallest = index;

  if (left <= __size && arr[left] < arr[index]) smallest = left;
  if (right <= __size && arr[right] < arr[smallest]) smallest = right;

  if (smallest != index) {
    swap(arr[index], arr[smallest]);
    minHeapify(arr, smallest, __size);
  }
}

void decreaseKey(int arr[], int index, int key) {
  arr[index] = key;

  while (index > 1 && arr[parent(index)] > arr[index]) {
    swap(arr[index], arr[parent(index)]);
    index = parent(index);
  }
}

void push(int arr[], int key, int & __size) {
  __size += 1;
  arr[__size] = 2147483647;

  decreaseKey(arr, __size, key);
}

int extractMin(int arr[], int & __size) {
  if (__size < 1) {
    cout << '*' << endl;
    return 0;
  }

  int toExtract = arr[1];
  arr[1] = arr[__size];
  __size -= 1;

  minHeapify(arr, 1, __size);
  cout << toExtract << endl;
  return 0;
}

int findValueInQueue(int value, int & __size) {
  int index;

  for (index = 1; index <= __size; index++) {
    if (box[value] == priorityQueue[index]) break;
  }

  return index;
}

int main() {
  freopen("priorityqueue.in", "r", stdin);
  freopen("priorityqueue.out", "w", stdout);

  int numberOne = 0;
  int numberTwo = 0;
  int counter = 0, N = 0;

  string command;

  while (cin >> command) {
    if (command == "push") {
      cin >> numberOne;
      push(priorityQueue, numberOne, N);
    }

    if (command == "extract-min") {
      numberOne = -1;
      extractMin(priorityQueue, N);
    }

    if (command == "decrease-key") {
      cin >> numberOne >> numberTwo;
      int __res = findValueInQueue(numberOne, N);
      decreaseKey(priorityQueue, __res, numberTwo);
      box[numberOne] = numberTwo;
    }

    counter++;
    box[counter] = numberOne;
  }

  fclose(stdin);
  fclose(stdout);
  return 0;
}
