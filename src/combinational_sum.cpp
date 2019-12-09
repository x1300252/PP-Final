#include <bits/stdc++.h>
using namespace std;

vector<vector<vector<pair<int, int>>>> combinations;

void turn_into_collide_form(int idx,
                            vector<int> *result) {
  // cout << idx << endl;
  vector<int> unique_result = *result;
  vector<pair<int, int>> final_result;
  int cnt;
  vector<int>::iterator it = unique(unique_result.begin(), unique_result.end());
  unique_result.resize(distance(unique_result.begin(), it));
  for (size_t i = 0; i < unique_result.size(); i++) {
    cnt = count(result->begin(), result->end(), unique_result[i]);
    final_result.push_back(make_pair(unique_result[i], cnt));
  }

  combinations[idx].push_back(final_result);

  return;
}

void get_combinations(int target, int start, int max_target,
                      vector<int> *result) {
  int result_size, idx;
  // cout << target << " " << start << " " << max_target << " " << result->size() << endl;
  if (result->size()) {
    turn_into_collide_form(max_target-target, result);
  }

  for (int i = start; i < max_target+1; i++) {
    if (target < i) {
      break;
    }

    result_size = result->size();
    idx = max_target - target + i;

    if(idx < 51 && result_size > 6)
      break;
    else if (idx > 50 && idx < 301 && result_size > 3)
      break;
    else if (idx > 300 && idx > 300 && result_size > 2)
      break;
    result->push_back(i);
    get_combinations(target - i, i, max_target, result);
    result->pop_back();
  }

  return;
}

int main() {
  // compute_combinations();
  vector<vector<pair<int, int>>> tmp;
  for (int i=0; i<11; i++) {
    combinations.push_back(tmp);
  }

  vector<int> result;
  int max = 10;
  for (int i = 1; i < max; i++) {
    result.push_back(i);
    get_combinations(max - i, i, max, &result);
    result.pop_back();
  }

  // ofstream combinational_sum_file;
  // combinational_sum_file.open ("combinational_sum.txt");
  for (int i = 1; i < 11; i++) {
    cout << i << " " << combinations[i].size() << endl;
    for (int j = 0; j < combinations[i].size(); j++) {
      cout << "( ";
      for (int k = 0; k < combinations[i][j].size(); k++) {
        cout << combinations[i][j][k].first << ":" << combinations[i][j][k].second << " ";
      }
      cout << ") ";
    }
    cout << endl;
  }
  // combinational_sum_file.close();
  return 0;
}
