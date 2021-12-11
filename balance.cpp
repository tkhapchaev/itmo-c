#include <iostream>

int tree[500000][3];
int heights[500000];
int N;

void count_heights() {
  for (int counter = N; counter > 0; counter--) {
    int left_tree = tree[counter][1];
    int right_tree = tree[counter][2];
    int greater = 0;

    if (heights[left_tree] > heights[right_tree]) greater = heights[left_tree];
    else greater = heights[right_tree];

    heights[counter] = 1 + greater;
  }
}

int main() {
  freopen("balance.in", "r", stdin);
  freopen("balance.out", "w", stdout);

  std::cin >> N;
  for (int i = 0; i < N; i++) {
    std::cin >> tree[i + 1][0];
    std::cin >> tree[i + 1][1];
    std::cin >> tree[i + 1][2];
  }

  count_heights();
  for (int i = 0; i < N; i++) {
    std::cout << heights[tree[i + 1][2]] - heights[tree[i + 1][1]] << std::endl;
  }

  fclose(stdin);
  fclose(stdout);
  return 0;
}
