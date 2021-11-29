#include <iostream>

int tree[500000][3];
bool is_correct = true;

void recursive_traversal(int current_vertex, int min_diff, int max_diff) {
    if (tree[current_vertex][0] <= min_diff || tree[current_vertex][0] >= max_diff) {
        is_correct = false;
    }

    if (tree[current_vertex][1] != 0) {
        recursive_traversal(tree[current_vertex][1], min_diff, tree[current_vertex][0]);
    }

    if (tree[current_vertex][2] != 0) {
        recursive_traversal(tree[current_vertex][2], tree[current_vertex][0], max_diff);
    }
}

int main() {
    freopen("check.in", "r", stdin);
    freopen("check.out", "w", stdout);

    int N; std::cin >> N;
    for (int i = 0; i < N; i++) {
        std::cin >> tree[i + 1][0];
        std::cin >> tree[i + 1][1];
        std::cin >> tree[i + 1][2];
    }

    recursive_traversal(1, -1000000000, 1000000000);

    if (is_correct) std::cout << "YES" << std::endl;
    else std::cout << "NO" << std::endl;
}