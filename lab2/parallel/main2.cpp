#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void print_vector(vector<double> A) {
    int N = A.size();
    #pragma omp for
    for (int i = 0; i < N; ++i) {
        cout << A[i] << " ";
    }
    cout << "\n\n";
}

vector<double> init_A(int N) {
    vector<double> A(N * N, 0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                A[N * i + j] = 2.0;
            } else {
                A[N * i + j] = 1.0;
            }
        }
    }

    return A;
}

vector<double> init_random(int N) {
    vector<double> res(N);
    for (int i = 0; i < N; ++i) {
        res[i] = sin(2 * 3.14 * i / N);
    }
    return res;
}

vector<double> mul_vector_scalar(vector<double> const &A, double k, vector<double> &result) {
    int N = A.size();
    vector<double> result(N);
    for (int i = 0; i < N; ++i) {
        result[i] = A[i] * k;
    }

    return result;
}

vector<double> mul_matrix(vector<double> A, vector<double> B) {
    int N = B.size();

    vector<double> res(N, 0);
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            res[i] += A[N * i + k] * B[k];
        }
    }

    return res;
}

vector<double> sub_vector(vector<double> A, vector<double> B) {
    int N = A.size();
    vector<double> result(N);
    for (int i = 0; i < N; ++i) {
        result[i] = A[i] - B[i];
    }

    return result;
}

double len_vec(vector<double> A) {
    int N = A.size();

    double sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += A[i] * A[i];
    }

    return sqrt(sum);
}

bool is_solve(vector<double> c, vector<double> b) {
    double e = 0.000000001;
    if (len_vec(c) / len_vec(b) < e) {
        return true;
    } else {
        return false;
    }
}

double calculateTAY(vector<double> A, int N) {
    double maxSum = 0;
    for (int i = 0; i < N; i++) {
        double rowSum = 0;
        for (int j = 0; j < N; j++) {
            rowSum += fabs(A[N * i + j]);
        }
        if (rowSum > maxSum) {
            maxSum = rowSum;
        }
    }
    return 1.0 / maxSum; 
}

vector<double> solve_system(vector<double> A, vector<double> b) {
    int N = b.size();
    vector<double> x(N, 0.0);
    vector<double> c(N, 0.0);
    vector<double> x_new(N, 0.0);
    double t;

    bool solved = false;
    while (!solved) {
        c = sub_vector(mul_matrix(A, x), b);
        t = calculateTAY(A, N);

        #pragma omp critical
        {
            x_new = sub_vector(x, mul_vector_scalar(c, t));
            x = x_new;
        }
        solved = is_solve(c, b);
    }

    return x;
}

bool is_correct(vector<double> u, vector<double> x, int N) {
    double error_rate = 1e-6;
    for (int i = 0; i < N; ++i) {
        if (fabs(u[i] - x[i]) > error_rate) {
            printf("The decision is wrong\n\n");
            return false;
        }
    }
    printf("The decision is correct\n\n");
    return true;
}

int main() {
    int N;
    cout << "Enter the size of the matrix N: ";
    cin >> N;
    
    int num_threads = 1;
    while(num_threads <= 16) {
        omp_set_num_threads(num_threads);
        int max_threads = omp_get_max_threads();
        cout << "Maximum number of threads: " << max_threads << "\n";
        
        int min = 999999999;
        for (int i = 0; i < 3; ++i) {
            auto start = high_resolution_clock::now();

            #pragma omp parallel
            {
            vector<double> A = init_A(N);
            vector<double> u = init_random(N);
            vector<double> b = mul_matrix(A, u);
            vector<double> x = solve_system(A, b);
            is_correct(u, x, N);
            }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            if (duration.count() < min) {
                min = duration.count();
            }
        }
        cout << "Number threads: " << num_threads << "; Lead time: " << min << " milliseconds\n\n";

        num_threads *= 2;
    }

    return 0;
}