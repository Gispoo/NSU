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
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        cout << A[i] << " ";
    }
    cout << "\n\n";
}

vector<double> init_A(int N) {
    vector<double> A(N * N, 0);

    #pragma omp parallel for
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
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        res[i] = sin(2 * 3.14 * i / N);
    }
    return res;
}

vector<double> mul_vector_scalar(vector<double> A, double k) {
    int N = A.size();
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        A[i] *= k;
    }

    return A;
}

vector<double> mul_matrix(vector<double> A, vector<double> B) {
    int N = B.size();

    vector<double> res(N, 0);
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            res[i] += A[N * i + k] * B[k];
        }
    }

    return res;
}

vector<double> sub_vector(vector<double> A, vector<double> B) {
    int N = A.size();

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        A[i] = A[i] - B[i];
    }

    return A;
}

double len_vec(vector<double> A) {
    int N = A.size();

    double sum = 0;
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        sum += A[i] * A[i];
    }

    return sqrt(sum);
}

double dot_product(const vector<double>& a, const vector<double>& b) {
    double result = 0.0;
    #pragma omp parallel for
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
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
    #pragma omp parallel for
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

    do {
        c = sub_vector(mul_matrix(A, x), b);
        double t = calculateTAY(A, N);
        x = sub_vector(x, mul_vector_scalar(c, t));
    } while(!is_solve(c, b));

    return x;
}

int main() {
    
    int N;
    cout << "Enter the size of the matrix N: ";
    cin >> N;
    
    // ofstream out("out.txt");
    int num_threads = 1;
    while(num_threads <= 16) {
        omp_set_num_threads(num_threads);
        int max_threads = omp_get_max_threads();
        cout << "Maximum number of threads: " << max_threads << "\n";
        
        int min = 999999999;
        for (int i = 0; i < 3; ++i) {
            auto start = high_resolution_clock::now();

            vector<double> A = init_A(N);
            // vector<double> b(N, (N + 1) / 1.0);
            vector<double> u = init_random(N);
            vector<double> b = mul_matrix(A, u);
            vector<double> x = solve_system(A, b);

            // if (i == 0) {
            //     print_vector(u);
            //     print_vector(x);
            // }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            if (duration.count() < min) {
                min = duration.count();
            }
        }
        cout << "Number threads: " << num_threads << "; Lead time: " << min << " milliseconds\n";

        num_threads *= 2;
    }

    return 0;
}