#include <iostream>
#include <vector>
#define _HASH_TABLE_SIZE 1000000

std::vector < std::vector < std::pair < std::string, std::string >>> HASH_TABLE(_HASH_TABLE_SIZE);

int hash(std::string key) {
  int current_hash_value = 0;
  for (int counter = 0; counter < key.length(); counter++) {
    current_hash_value = (current_hash_value * 17 + key[counter] - 'a' + _HASH_TABLE_SIZE) % _HASH_TABLE_SIZE;
  }

  return current_hash_value;
}

int main() {
  freopen("map.in", "r", stdin);
  freopen("map.out", "w", stdout);

  std::cin.tie(0);
  std::ios_base::sync_with_stdio(false);

  std::string command, key, value;
  unsigned int current_hash_value;
  unsigned int index;
  while (std::cin >> command >> key) {
    bool got_key_flag = false;
    current_hash_value = hash(key);
    for (int counter = 0; counter < HASH_TABLE[current_hash_value].size(); counter++) {
      if (HASH_TABLE[current_hash_value][counter].first == key) {
        index = counter;
        got_key_flag = true;
        break;
      }
    }

    if (command == "delete" && got_key_flag) {
      auto to_delete = HASH_TABLE[current_hash_value].begin() + index;
      HASH_TABLE[current_hash_value].erase(to_delete);
    }

    if (command == "put") {
      std::cin >> value;
      if (got_key_flag) {
        HASH_TABLE[current_hash_value][index].second = value;
      }

      if (!got_key_flag) {
        HASH_TABLE[current_hash_value].push_back(make_pair(key, value));
      }
    }

    if (command == "get") {
      if (got_key_flag) {
        std::cout << HASH_TABLE[current_hash_value][index].second << std::endl;
      }

      if (!got_key_flag) {
        std::cout << "none" << std::endl;
      }
    }
  }

  fclose(stdin);
  fclose(stdout);
  return 0;
}
