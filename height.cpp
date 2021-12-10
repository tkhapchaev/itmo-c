#include <iostream>

int height = 0;
int tree[500000][3];

void search_for_height(int current_child, int max_height) {
  if (max_height > height) height = max_height;

  if (tree[current_child][1] != 0) {
    search_for_height(tree[current_child][1], max_height + 1);
  }

  if (tree[current_child][2] != 0) {
    search_for_height(tree[current_child][2], max_height + 1);
  }
}

int main() {
  freopen("height.in", "r", stdin);
  freopen("height.out", "w", stdout);

  int N;
  std::cin >> N;
  for (int i = 0; i < N; i++) {
    std::cin >> tree[i + 1][0];
    std::cin >> tree[i + 1][1];
    std::cin >> tree[i + 1][2];
  }

  search_for_height(1, 1);
  if (N > 0) std::cout << height << std::endl;
  else std::cout << 0 << std::endl;
}
