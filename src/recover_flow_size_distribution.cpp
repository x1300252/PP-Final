#include <bits/stdc++.h>
#include<sys/time.h>
using namespace std;

vector<vector<vector<pair<int, int>>>> combinations;
vector<vector<double>> probabilities;

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

    if(idx < 51 && result_size > 5)
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

void get_probability(double *hash_tables, int max_idx, int hash_table_size) {
    int fractorials[8] = {1, 1, 2, 6, 24, 120 , 720, 5040};

    double *factors = (double *)calloc(max_idx + 1, sizeof(double));
    for (size_t i = 0; i < max_idx + 1; i++) {
        if (hash_tables[i] != 0) {
            factors[i] = hash_tables[i] / hash_table_size;
        }
        // cout << i << " " << factors[i] << endl;
    }

    double temp, sum_of_probabilities;
    for (size_t i = 1; i < combinations.size(); i++) {
        vector<double> probabilities_of_i;
        sum_of_probabilities = 0;
        if (factors[i] != 0) {
            for (size_t j = 0; j < combinations[i].size(); j++) {
                temp = 1;
                for (size_t k = 0; k < combinations[i][j].size(); k++) {
                    if (factors[combinations[i][j][k].first] == 0) {
                        temp = 0;
                        break;
                    }
                    temp *= pow(factors[combinations[i][j][k].first], combinations[i][j][k].second) / fractorials[combinations[i][j][k].second];
                }
                probabilities_of_i.push_back(temp);
                sum_of_probabilities += temp;
            }
        }
        cout << i << " " << sum_of_probabilities << endl;
        probabilities_of_i.push_back(sum_of_probabilities);
        probabilities.push_back(probabilities_of_i);
    }
}

int main(int argc, char** argv) {
    // read hash table
    int hash_table_size = atoi(argv[1]);
    double *hash_table;
    hash_table = (double *)calloc(hash_table_size, sizeof(double));

    ifstream infile(argv[2]);
    string line;
    int idx;
    double val;
    int max_idx = 0;

    getline(infile, line);
    while (infile >> idx >> val) {
        if (val != 0) {
            hash_table[idx] = val;
            max_idx = idx;
        }
    }

    double *temp = (double *)realloc(hash_table, sizeof(double) * (max_idx + 1));

    struct  timeval start;
    struct  timeval end;

    gettimeofday(&start, NULL);
    vector<vector<pair<int, int>>> tmp;
    for (int i=0; i<max_idx+1; i++) {
        combinations.push_back(tmp);
    }

    vector<int> result;
    get_combinations(max_idx, 1, max_idx, &result);
    gettimeofday(&end, NULL);

    cout << "Compute combinational sums: " << end.tv_sec-start.tv_sec << "." << end.tv_usec-start.tv_usec << "s" << endl;

    gettimeofday(&start, NULL);
    get_probability(temp, max_idx, hash_table_size);
    gettimeofday(&end, NULL);
    cout << "Compute probabilities: " << end.tv_sec-start.tv_sec << "." << end.tv_usec-start.tv_usec << "s" << endl;


    return 0;
}