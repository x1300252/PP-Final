#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <algorithm> // std::unique, std::count
#include <vector> // std::vecotr
#include <cmath> // std::pow
#include <sys/time.h>
#include "tbb/concurrent_vector.h"
using namespace std;
using namespace tbb;

concurrent_vector<concurrent_vector<vector<pair<int, int>>>> combinations;
vector<vector<double>> probabilities;

void turn_into_collide_form(int idx,
                            vector<int> *result) {
  vector<int> unique_result(*result);
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

void compute_combinations(int max_idx) {
    struct timeval diff, start, end;
    gettimeofday(&start, NULL);

    // max_idx++;

    // concurrent_vector<vector<pair<int, int>>> tmp;
    // for (int i=0; i<max_idx+1; i++) {
    //     combinations.push_back(tmp);
    // }

    combinations.grow_to_at_least(max_idx + 1);

    #pragma omp parallel for
    for (int i = 1; i < max_idx+1; i++) {
        vector<int> result(1, i);
        // result.push_back(i);
        get_combinations(max_idx - i, i, max_idx, &result);
        // result.pop_back();
    }

    gettimeofday(&end, NULL);
    timersub(&end, &start, &diff);
    cout << "Compute combinational sums: " << diff.tv_sec << "." << diff.tv_usec << "s" << endl;
}

void get_probability(double *hash_tables, int max_idx, int hash_table_size) {
    int fractorials[8] = {1, 1, 2, 6, 24, 120 , 720, 5040};

    double *factors = (double *)calloc(max_idx + 1, sizeof(double));
    for (size_t i = 0; i < max_idx + 1; i++) {
        if (hash_tables[i] != 0) {
            factors[i] = hash_tables[i] / hash_table_size;
        }
    }

    // double temp, sum_of_probabilities;
    vector<double> probabilities_of_i;
    probabilities[0] = probabilities_of_i;
    #pragma omp parallel for
    for (size_t i = 1; i < combinations.size(); i++) {
        vector<double> probabilities_of_i;
        double sum_of_probabilities = 0;
        // if (factors[i] != 0) {
            for (size_t j = 0; j < combinations[i].size(); j++) {
                double temp = 1;
                for (size_t k = 0; k < combinations[i][j].size(); k++) {
                    // if (factors[combinations[i][j][k].first] == 0) {
                    //     temp = 0;
                    //     break;
                    // }
                    temp *= pow(factors[combinations[i][j][k].first], combinations[i][j][k].second) / fractorials[combinations[i][j][k].second];
                }
                probabilities_of_i.push_back(temp);
                sum_of_probabilities += temp;
            }

            for (size_t a = 0; a < probabilities_of_i.size(); a++) {
                probabilities_of_i[a] /= sum_of_probabilities;
            }
        // }
        // else { 
            if (probabilities_of_i.size() == 0)
                probabilities_of_i.push_back(sum_of_probabilities);
        // }
        probabilities[i] = probabilities_of_i;
    }
}

double WMRD(double *current, double *next, int max_idx) {
    double sum_of_abs = 0;
    double sum_of_avg = 0;
    for (int i = 0; i < max_idx + 1; i++) {
        sum_of_abs += abs(next[i] - current[i]);
        sum_of_avg += (next[i] + current[i]) / 2;
    }

    return sum_of_abs / sum_of_avg;
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
    infile.close();

    double *current = (double *)realloc(hash_table, sizeof(double) * (max_idx + 1));

    compute_combinations(max_idx);
    probabilities.reserve(combinations.size());

    struct timeval diff, start, end;
    double *next = (double *)calloc(max_idx + 1, sizeof(double));
    double wmrd;
    int iteration_cnt = 0;
    while (1) {
        iteration_cnt++;
        gettimeofday(&start, NULL);
        get_probability(current, max_idx, hash_table_size); 
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);
        cout << "Compute probabilities: " << diff.tv_sec << "." << diff.tv_usec << "s" << endl;

        gettimeofday(&start, NULL);

        #pragma omp parallel for reduction(+: next[:max_idx + 1]) 
        for (size_t i = 1; i < combinations.size(); i++) {
            // if (current[i] != 0) {
                for (size_t j = 0; j < combinations[i].size(); j++) {
                    // if (probabilities[i][j] != 0) {
                        for (size_t k = 0; k < combinations[i][j].size(); k++) {
                            next[combinations[i][j][k].first] += current[i] * combinations[i][j][k].second * probabilities[i][j];
                        }
                    // }
                }
            // }
        }
        wmrd = WMRD(current, next, max_idx);
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);
        cout << "Iteration " << iteration_cnt << ": " << wmrd << "\t" << diff.tv_sec << "." << diff.tv_usec << "s" << endl;

        if (iteration_cnt == 2)
            break;

        free(current);
        current = next;
        next = (double *)calloc(max_idx + 1, sizeof(double));
        probabilities.clear(); 
    }

    string out_fname("../recover_result/");
    out_fname += argv[1];
    out_fname += "_1.csv";
    ofstream outfile(out_fname.c_str());
    outfile << "Flow.size,recover_result" << endl;
    for (int i = 1; i < max_idx + 1; i++) {
        outfile << i << "," << current[i] << endl;
    }
    outfile.close();

    free(current);
    return 0;
}
