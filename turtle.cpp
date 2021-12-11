#include <iostream>
#include <vector>
using namespace std;

int main() {
  freopen("turtle.in", "r", stdin);
  freopen("turtle.out", "w", stdout);
  int h, w;
  cin >> h >> w;

  vector < vector < int >> A(h, vector < int > (w)); 
  vector < vector < int >> B(h, vector < int > (w)); 

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      cin >> A[i][j];
    }
  }

  B[h - 1][0] = A[h - 1][0];
  for (int i = h - 2; i >= 0; i--) {
    B[i][0] = B[i + 1][0] + A[i][0];
  }

  for (int j = 1; j < w; j++) {
    B[h - 1][j] = B[h - 1][j - 1] + A[h - 1][j];
  }

  for (int i = h - 2; i >= 0; i--) {
    for (int j = 1; j < w; j++) {
      B[i][j] = A[i][j] + max(B[i + 1][j], B[i][j - 1]);
    }
  }

  cout << B[0][w - 1];
}
