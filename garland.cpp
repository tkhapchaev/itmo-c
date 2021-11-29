#include <iostream>
using namespace std;

double garlandBinarySearch(double arr[], int N) {
    double left = 0, right = arr[0];
    int flag = 0, i;

    while (right - left > 0.0000000001) {
        arr[1] = (left + right) / 2;
        flag = 0;

        for (i = 2; i < N; i++) {
            arr[i] = 2 * arr[i - 1] - arr[i - 2] + 2;

            if (arr[i] < 0) {
                flag = 1;
                break;
            }
        }

        if (flag == 0) {
            right = arr[1];
        }

        else left = arr[1];
    }

    return arr[N - 1];
}

int main() {
    freopen("garland.in", "r", stdin);
    freopen("garland.out", "w", stdout);

    int Number; cin >> Number;
    double *arr = new double[Number]; cin >> arr[0];

    double result = garlandBinarySearch(arr, Number);

    cout.precision(2);
    cout << fixed << result;
}
