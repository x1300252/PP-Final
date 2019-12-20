#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <algorithm> // std::unique, std::count
#include <vector> // std::vecotr
#include <cmath> // std::pow
#include <sys/time.h>
#include <pthread.h>

using namespace std;

vector<vector<vector<pair<int, int>>>> combinations;
vector<vector<double>> probabilities;

#define max_threads_num 32
pthread_t threads[max_threads_num];
int threads_num;
pthread_mutex_t *lock, next_lock;
double *current, *next_temp;

struct for_args {
  int start;
  int end;
  int max_idx;
  double *factors;
};

for_args threads_for_args[max_threads_num];

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

  pthread_mutex_lock(&lock[idx]);
  combinations[idx].push_back(final_result);
  pthread_mutex_unlock(&lock[idx]);

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

void* pthread_omp_parallel_for_get_combinations(void* args) {
  int start = ((for_args*)args) -> start;
  int max_idx = ((for_args*)args) -> max_idx;

  for (int i = start; i <= max_idx; i += threads_num) {
    vector<int> result(1, i);
    get_combinations(max_idx - i, i, max_idx, &result);
  }

  return NULL;
}

void compute_combinations(int max_idx) {
    struct timeval diff, start, end;
    gettimeofday(&start, NULL);

    combinations.resize(max_idx+1);

    int threads_size = (max_idx + 1) / threads_num;

    for (int i = 1; i <= threads_num; i++) {
      threads_for_args[i].start = i;
      threads_for_args[i].max_idx = max_idx;

      pthread_create(threads + i - 1, NULL, pthread_omp_parallel_for_get_combinations, threads_for_args + i);
    }
    
    for (int i = 0; i < threads_num; i++) {
      pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);
    timersub(&end, &start, &diff);
    cout << "Compute combinational sums: " << diff.tv_sec << "." << diff.tv_usec << "s" << endl;
}

void* pthread_omp_parallel_for_get_probability(void* args) {
  int start = ((for_args*)args) -> start;
  int end = ((for_args*)args) -> end;
  double *factors = ((for_args*)args) -> factors;

  int fractorials[8] = {1, 1, 2, 6, 24, 120 , 720, 5040};

  for (size_t i = start; i < end; i++) {
    vector<double> probabilities_of_i;
    double sum_of_probabilities = 0;
    for (size_t j = 0; j < combinations[i].size(); j++) {
      double temp = 1;
      for (size_t k = 0; k < combinations[i][j].size(); k++) {
        temp *= pow(factors[combinations[i][j][k].first], combinations[i][j][k].second) / fractorials[combinations[i][j][k].second];
      }
      probabilities_of_i.push_back(temp);
      sum_of_probabilities += temp;
    }

    for (size_t a = 0; a < probabilities_of_i.size(); a++) {
      probabilities_of_i[a] /= sum_of_probabilities;
    }
    
    if (probabilities_of_i.size() == 0)
      probabilities_of_i.push_back(sum_of_probabilities);

    probabilities[i] = probabilities_of_i;
  }

  return NULL;
}

void get_probability(double *hash_tables, int max_idx, int hash_table_size) {

    double *factors = (double *)calloc(max_idx + 1, sizeof(double));
    for (size_t i = 0; i < max_idx + 1; i++) {
        if (hash_tables[i] != 0) {
            factors[i] = hash_tables[i] / hash_table_size;
        }
    }

    vector<double> probabilities_of_i;
    probabilities[0] = probabilities_of_i;
    
    int threads_size = combinations.size() / threads_num;
    for (int i = 0; i < threads_num; i++) {
      threads_for_args[i].start = i == 0 ? 1 : i * threads_size;
      threads_for_args[i].end = i == threads_num -1 ? combinations.size() : (i + 1) * threads_size;
      threads_for_args[i].factors = factors;

      pthread_create(threads + i, NULL, pthread_omp_parallel_for_get_probability, threads_for_args + i);
    }
    
    for (int i = 0; i < threads_num; i++) {
      pthread_join(threads[i], NULL);
    }
}

void* pthread_omp_parallel_for_get_next_size_distribution(void* args) {
  int start = ((for_args*)args) -> start;
  int end = ((for_args*)args) -> end;
  int max_idx = ((for_args *)args)->max_idx;

  double *local_next = (double *)calloc(max_idx + 1, sizeof(double));

  // #pragma omp parallel for reduction(+: next_temp[:max_idx + 1]) 
  for (size_t i = start; i < end; i++) {
      // if (current[i] != 0) {
          for (size_t j = 0; j < combinations[i].size(); j++) {
              // if (probabilities[i][j] != 0) {
                  for (size_t k = 0; k < combinations[i][j].size(); k++) {
                    local_next[combinations[i][j][k].first] += current[i] * combinations[i][j][k].second * probabilities[i][j];
                  }
              // }
          }
      // }
  }

  pthread_mutex_lock(&next_lock);
  for (int i = 0; i <= max_idx; i++) {
    next_temp[i] += local_next[i];
  }
  pthread_mutex_unlock(&next_lock);

  return NULL;
}

void get_next_size_distribution(int max_idx) {
  int threads_size = combinations.size() / threads_num;
  for (int i = 0; i < threads_num; i++) {
    threads_for_args[i].start = i == 0 ? 1 : i * threads_size;
    threads_for_args[i].end = i == threads_num -1 ? combinations.size() : (i + 1) * threads_size;
    threads_for_args[i].max_idx = max_idx;

    pthread_create(threads + i, NULL, pthread_omp_parallel_for_get_next_size_distribution, threads_for_args + i);
  }
  
  for (int i = 0; i < threads_num; i++) {
    pthread_join(threads[i], NULL);
  }
}


double WMRD(double *current, double *next_temp, int max_idx) {
    double sum_of_abs = 0;
    double sum_of_avg = 0;
    for (int i = 0; i < max_idx + 1; i++) {
        sum_of_abs += abs(next_temp[i] - current[i]);
        sum_of_avg += (next_temp[i] + current[i]) / 2;
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

    current = (double *)realloc(hash_table, sizeof(double) * (max_idx + 1));

    threads_num = atoi(argv[3]);

    // init mutexes
    lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * (max_idx + 1));
    for (int i = 0; i < max_idx+1; i++) {
      pthread_mutex_init(&lock[i], NULL);
    }
    

    compute_combinations(max_idx);
    probabilities.reserve(combinations.size());

 
    threads_for_args[threads_num - 1].end = max_idx + 1;

    struct timeval diff, start, end;
    next_temp = (double *)calloc(max_idx + 1, sizeof(double));
    double wmrd;
    int iteration_cnt = 0;

    for (int i = 0; i < max_idx+1; i++) {
      pthread_mutex_init(&lock[i], NULL);
    }

    pthread_mutex_init(&next_lock, NULL);

    while (1) {
        iteration_cnt++;
        gettimeofday(&start, NULL);
        get_probability(current, max_idx, hash_table_size); 
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);
        cout << "Compute probabilities: " << diff.tv_sec << "." << diff.tv_usec << "s" << endl;

        gettimeofday(&start, NULL);

        get_next_size_distribution(max_idx);

        wmrd = WMRD(current, next_temp, max_idx);
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);
        cout << "Iteration " << iteration_cnt << ": " << wmrd << "\t" << diff.tv_sec << "." << diff.tv_usec << "s" << endl;

        if (iteration_cnt == 2)
            break;

        free(current);
        current = next_temp;
        next_temp = (double *)calloc(max_idx + 1, sizeof(double));
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

    for (int i = 0; i < max_idx+1; i++) {
      pthread_mutex_destroy(&lock[i]);
    }

    free(lock);
    free(current);

    return 0;
}
