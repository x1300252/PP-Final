#include <bits/stdc++.h>
using namespace std;

vector<vector<vector<pair<int, int>>>> combinations;

void turn_into_collide_form(vector<vector<pair<int, int>>> *all_results,
                            vector<int> *result) {  
  vector<int> unique_result = *result;
  vector<pair<int, int>> final_result;
  int cnt;
  vector<int>::iterator it = unique(unique_result.begin(), unique_result.end());
  unique_result.resize(distance(unique_result.begin(), it));
  for (size_t i = 0; i < unique_result.size(); i++) {
    cnt = count(result->begin(), result->end(), unique_result[i]);
    final_result.push_back(make_pair(unique_result[i], cnt));
  }

  all_results->push_back(final_result);

  return;
}

void get_combinations(int target, int start, int max_size,
                      vector<vector<pair<int, int>>> *all_results,
                      vector<int> *result) {
  // if (result->size() == (max_size - 1) && target == start) {
  //   result->push_back(target);

  //   turn_into_collide_form(all_results, result);
  //   result->pop_back();
  //   return;
  // } else if (result->size() >= max_size) {
  //   return;
  // }

  // for (int i = start; i <= target;) {
  //   result->push_back(i);
  //   if (i == target) {
  //     turn_into_collide_form(all_results, result);
  //   } else {
  //     get_combinations(target - i, i, max_size, all_results, result);
  //   }
  //   i = result->back() + 1;
  //   result->pop_back();
  // }

  if (target < 0)
    return;

  if (result->size() == max_size && target)
    return;
  
  if (target == 0) {
    turn_into_collide_form(all_results, result);
    return;
  }

  int i = start;
  while (target - i >= 0) {
    result->push_back(i);
    get_combinations(target - i, i, max_size, all_results, result);
    i++;
    result->pop_back();
  }  

  return;
}

void compute_combinations() {
  for (int i = 1; i < 51; i++) {
    vector<vector<pair<int, int>>> all_results;
    vector<int> result;
    get_combinations(i, 1, 7, &all_results, &result);

    // cout << i << ": ";
    // for (size_t j = 0; j < all_results.size(); j++) {
    // 	cout << "( ";
    // 	for (size_t k = 0; k < all_results[j].size(); k++) {
    // 		cout << all_results[j][k].first << ":" << all_results[j][k].second << " ";
    // 	}
    // 	cout << ") ";
    // }
    // cout << endl;
  }

  for (int i = 51; i < 301; i++) {
    vector<vector<pair<int, int>>> all_results;
    vector<int> result;
    get_combinations(i, 1, 5, &all_results, &result);

    // cout << i << ": ";
    // for (size_t j = 0; j < all_results.size(); j++) {
    // 	cout << "( ";
    // 	for (size_t k = 0; k < all_results[j].size(); k++) {
    // 		cout << all_results[j][k].first << ":" << all_results[j][k].second << " ";
    // 	}
    // 	cout << ") ";
    // }
    // cout << endl;
  }

  // for (int i = 301; i < 1001; i++) {
  //   vector<vector<pair<int, int>>> all_results;
  //   vector<int> result;
  //   get_combinations(i, 1, 4, &all_results, &result);

  //   // cout << i << ": ";
  //   // for (size_t j = 0; j < all_results.size(); j++) {
  //   // 	cout << "( ";
  //   // 	for (size_t k = 0; k < all_results[j].size(); k++) {
  //   // 		cout << all_results[j][k].first << ":" << all_results[j][k].second << " ";
  //   // 	}
  //   // 	cout << ") ";
  //   // }
  //   // cout << endl;
  // }

  return;
}

int main() {
  compute_combinations();

  return 0;
}
