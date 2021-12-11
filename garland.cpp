#include <iostream>
using namespace std;

double garlandBinarySearch(double array[], int N) {
  double left = 0, right = array[0];
  int flag = 0, i;

  while (right - left > 0.0000000001) {
    array[1] = (left + right) / 2;
    flag = 0;

    for (i = 2; i < N; i++) {
      array[i] = 2 * array[i - 1] - array[i - 2] + 2;

      if (array[i] < 0) {
        flag = 1;
        break;
      }
    }

    if (flag == 0) {
      right = array[1];
    } else left = array[1];
  }

  return array[N - 1];
}

int main() {
  freopen("garland.in", "r", stdin);
  freopen("garland.out", "w", stdout);

  int Number;
  cin >> Number;
  double * array = new double[Number];
  cin >> array[0];
  double result = garlandBinarySearch(array, Number);

  cout.precision(2);
  cout << fixed << result;

  fclose(stdin);
  fclose(stdout);
  return 0;
}
