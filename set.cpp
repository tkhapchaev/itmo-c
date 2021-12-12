#include <iostream>
#include <vector>
#define _HASH_TABLE_SIZE 1000000

std::vector < std::vector < int >> HASH_TABLE(_HASH_TABLE_SIZE);

int hash(int value) {
  return abs(value) % _HASH_TABLE_SIZE;
}

bool exists(int value) {
  int current_hash_value = hash(value);
  for (int counter = 0; counter < HASH_TABLE[current_hash_value].size(); counter++) {
    if (HASH_TABLE[current_hash_value][counter] == value) return true;
  }

  return false;
}

void insert(int value) {
  if (!exists(value)) {
    HASH_TABLE[hash(value)].push_back(value);
  }
}

void remove(int value) {
  if (exists(value)) {
    int current_hash_value = hash(value);
    auto current_index = HASH_TABLE[current_hash_value].begin();
    while (current_index != HASH_TABLE[current_hash_value].end()) {
      if ( * current_index == value) break;
      current_index++;
    }

    HASH_TABLE[current_hash_value].erase(current_index);
  }
}

int main() {
  freopen("set.in", "r", stdin);
  freopen("set.out", "w", stdout);

  std::cin.tie(0);
  std::ios_base::sync_with_stdio(false);

  int value;
  std::string command;
  while (std::cin >> command >> value) {
    if (command == "insert") insert(value);
    if (command == "delete") remove(value);
    if (command == "exists") {
      if (exists(value)) std::cout << "true" << std::endl;
      else std::cout << "false" << std::endl;
    }
  }

  fclose(stdin);
  fclose(stdout);
  return 0;
}
