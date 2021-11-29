#include <iostream>
using namespace std;

int main() {
    freopen("radixsort.in", "r", stdin);
    freopen("radixsort.out", "w", stdout);

    int N; cin >> N;
    int M; cin >> M;
    int Phase; cin >> Phase;
    
    int flag = 0;
    char** strings = new char* [N];

    for (int i = 0; i < N; i++) {
        strings[i] = new char [M];
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cin >> strings[i][j];
        }
    }

    for (int position = M - 1; position >= 0; position--) {
        for (int i = 0; i < N - 1; i++) {
            for (int j = 0; j < N - i - 1; j++) {
                if (strings[j][position] > strings[j + 1][position]) {
                    swap(strings[j], strings[j + 1]);
                }
            }
        }

        flag++;
        if (flag == Phase) {
            break;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cout << strings[i][j];
        }

        cout << "\n";
    }
}
