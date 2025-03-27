#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

// ssh -i lin_priv -p 2222 opp145@84.237.52.21
// Ov2P7z-qP
// g++ -O2 -o 2 -fopenmp main2.cpp
// scp -i lin_priv -P 2222 ./lab2/parallel/main2.cpp opp145@84.237.52.21:~/lab2/parallel

using namespace std;
using namespace std::chrono;

int main() {
    int N;
    cout << "Enter the size of the matrix N: ";
    cin >> N;

    int thread_counts[] = {1, 2, 4, 8, 16};

    for (int i = 0; i < 5; i++) {
        int num_threads = thread_counts[i];
        omp_set_num_threads(num_threads);
        cout << "Number threads: " << num_threads << "\n\n";

        int min_time = 999999999;

        for (int j = 0; j < 3; j++) {
            auto start = high_resolution_clock::now();

            vector<double> A(N * N, 0.0); 
            vector<double> u(N, 0.0);
            vector<double> b(N, 0.0);
            vector<double> x(N, 0.0);
            vector<double> c(N, 0.0);
            
            double len_b = 0.0;
            double t_param = 0.0;
            bool solved = false;
            double tol = 1e-9;
            double local_sum = 0.0;
            double local_max = 0.0;
            double norm_sum = 0.0;

            #pragma omp parallel shared(A, u, b, x, c, len_b, t_param, solved)
            {
                #pragma omp for
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        A[i * N + j] = (i == j) ? 2.0 : 1.0;
                    }
                }

                #pragma omp for
                for (int i = 0; i < N; i++) {
                    u[i] = sin(2.0 * 3.14 * i / N);
                }

                #pragma omp for
                for (int i = 0; i < N; i++) {
                    double temp_sum = 0.0;
                    for (int j = 0; j < N; j++) {
                        temp_sum += A[i * N + j] * u[j];
                    }
                    b[i] = temp_sum;
                }

                #pragma omp for reduction(+:local_sum)
                for (int i = 0; i < N; i++) {
                    local_sum += b[i] * b[i];
                }
                #pragma omp single
                {
                    len_b = sqrt(local_sum);
                }

                #pragma omp for reduction(max:local_max)
                for (int i = 0; i < N; i++) {
                    double row_sum = 0.0;
                    for (int j = 0; j < N; j++) {
                        row_sum += fabs(A[i * N + j]);
                    }
                    if (row_sum > local_max) {
                        local_max = row_sum;
                    }
                }
                #pragma omp single
                {
                    t_param = 1.0 / local_max;
                }


                while (true) {               
                    norm_sum = 0;
                    #pragma omp for reduction(+:norm_sum)
                    for (int i = 0; i < N; i++) {
                        double temp_sum = 0.0;
                        for (int j = 0; j < N; j++) {
                            temp_sum += A[i * N + j] * x[j];
                        }
                        c[i] = temp_sum - b[i];
                        norm_sum += c[i] * c[i];
                    }
                    
                    #pragma omp single
                    {
                        double norm_c = sqrt(norm_sum);
                        if (norm_c / len_b < tol) {
                            solved = true;
                        }
                    }

                    #pragma omp barrier

                    if (solved) {
                        break;
                    }

                    #pragma omp for
                    for (int i = 0; i < N; i++) {
                        x[i] = x[i] - t_param * c[i];
                    }

                    #pragma omp barrier
                } // конец while
            } // конец параллельной облости

            if (j == 0) {
                bool correct = true;
                for (int i = 0; i < N; i++) {
                    if (fabs(u[i] - x[i]) > 1e-6) {
                        correct = false;
                        break;
                    }
                }
                if (correct) {
                    cout << "The decision is correct\n";
                } else {
                    cout << "The decision is wrong\n";
                }
            }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start).count();
            if (duration < min_time) {
                min_time = duration;
            }
        } // конец цикла из 3 запусков

        cout << "Lead time: " << min_time << " milliseconds\n-----------------\n";
    } // конец прохода по числу потоков

    return 0;
}
