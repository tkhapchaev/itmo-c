#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int leftChild(int i) {
    return 2 * i + 1;
}

int rightChild(int i) {
    return 2 * i + 2;
}

void heapify(int arr[], int index, int arr_size) {
    int left = leftChild(index);
    int right = rightChild(index);
    int largest = index;

    if ((left < arr_size) && (arr[left] > arr[largest])) largest = left;
    if ((right < arr_size) && (arr[right] > arr[largest])) largest = right;

    if (largest != index) {
        swap(arr[index], arr[largest]);
        heapify(arr, largest, arr_size);
    }
}

void buildHeap(int arr[], int arr_size) {
    for (int i = (arr_size / 2 - 1); i >= 0; i--) {
        heapify(arr, i, arr_size);
    }
}

void heapSort(int arr[], int arr_size) {
    buildHeap(arr, arr_size);
    for (int i = arr_size - 1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, 0, i);
    }
}

int main() {
    freopen("sort.in", "r", stdin);
    freopen("sort.out", "w", stdout);

    int N; cin >> N;
    int toSort[N];

    for (int i = 0; i < N; i ++) {
        cin >> toSort[i];
    }

    heapSort(toSort, N);
    for (int i = 0; i < N; i ++) {
        cout << toSort[i] << " ";
    }
}
