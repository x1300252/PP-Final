#include <bits/stdc++.h>

std::vector<std::vector<std::vector<int> > > combinations;

void get_combinations(int target, int start, int max_size,
                      std::vector<std::vector<int> > *all_results,
                      std::vector<int> *result) {
  if (result->size() == (max_size - 1) && target == start) {
    result->push_back(target);
    all_results->push_back(*result);
    result->pop_back();
    return;
  } else if (result->size() >= max_size) {
		return;
	}

  for (int i = start; i <= target;) {
    result->push_back(i);
    if (i == target) {
      all_results->push_back(*result);
    } else {
      get_combinations(target - i, i, max_size, all_results, result);
    }
    i = result->back() + 1;
    result->pop_back();
  }

  return;
}

void compute_combinations() {
  for (int i = 1; i < 10; i++) {
    std::vector<std::vector<int> > all_results;
    std::vector<int> result;
    get_combinations(i, 1, 7, &all_results, &result);

		// std::cout << i << ": ";
		// for (size_t j = 0; j < all_results.size(); j++) {
		// 	std::cout << "( ";
		// 	for (size_t k = 0; k < all_results[j].size(); k++) {
		// 		std::cout << all_results[j][k] << " ";
		// 	}
		// 	std::cout << ") ";
		// }
		// std::cout << std::endl;
  }
	return;
}

int main() {
	compute_combinations();

	return 0;
}
