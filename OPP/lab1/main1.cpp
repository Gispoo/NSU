#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <mpi.h>
#include <cstdlib>

// ssh -i lin_priv -p 2222 opp145@84.237.52.21
// ssh -i new_key -p 22 hpcuser145@84.237.117.10 -X
// Ov2P7z-qP
// mpicxx -O2 -o 1 main1.cpp
// mpecxx -mpilog -O2 -o 1 main1.cpp
// ~/sw/bin/mpecc -mpilog -O2 -o 2 main2.cpp компиляция
// mpirun -np 2 ./1 10 1
// scp -i lin_priv -P 2222 /home/gispo/OPP/lab1/parallel/main1.cpp opp145@84.237.52.21:/home/opp145/lab1/parallel
// scp -i new_key ./lab1/main1.cpp hpcuser145@84.237.117.10:~/lab1
// ~/sw/bin/jumpshot

using namespace std;
using namespace std::chrono;

void init_A(vector<double> &A, int cols, int rows, int start_row) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i + start_row == j) {
                A[cols * i + j] = 2.0;
            } else {
                A[cols * i + j] = 1.0;
            }
        }
    }
}

void init_random(vector<double> &res, int N) {
    for (int i = 0; i < N; ++i) {
        res[i] = sin(2 * 3.14 * i / N);
    }
}

void mul_vector_scalar(vector<double> &res, double k) {
    int N = res.size();
    for (int i = 0; i < N; ++i) {
        res[i] *= k;
    }
}

void mul_matrix(vector<double> &A, vector<double> &B, vector<double> &res, int rows) {
    int N = B.size();
    
    for (int i = 0; i < rows; ++i) {
        res[i] = 0.0;
        for (int k = 0; k < N; ++k) {
            res[i] += A[N * i + k] * B[k];
        }
    }
}

void sub_vector(vector<double> &res, vector<double> &B) {
    int N = res.size();

    for (int i = 0; i < N; ++i) {
        res[i] = res[i] - B[i];
    }
}

double len_vec(vector<double> &A) {
    int N = A.size();

    double sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += A[i] * A[i];
    }

    return sqrt(sum);
}

bool is_solve(vector<double> &c, double len_b) {
    double e = 0.000000001;
    if (len_vec(c) / len_b < e) {
        return true;
    } else {
        return false;
    }
}

double calculateTAY(vector<double> &A, int cols, int rows) {
    double maxSum = 0;
    
    for (int i = 0; i < rows; i++) {
        double rowSum = 0;
        for (int j = 0; j < cols; j++) {
            rowSum += fabs(A[cols * i + j]);
        }
        if (rowSum > maxSum) {
            maxSum = rowSum;
        }
    }
    double absolute_maxSum;
    MPI_Allreduce(&maxSum, &absolute_maxSum, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    return 1.0 / absolute_maxSum; 
}

void solve_system(vector<double> &A, vector<double> &x, vector<double> &b, int N, int rows) {
    vector<double> c(rows, 0.0);
    vector<double> whole_c(N, 0.0);

    bool solved = false;
    double t = calculateTAY(A, N, rows);
    double len_b = len_vec(b);
    while(!solved) {
        mul_matrix(A, x, c, rows); // c = Ax
        MPI_Allgather(c.data(), rows, MPI_DOUBLE, whole_c.data(), rows, MPI_DOUBLE, MPI_COMM_WORLD);

        sub_vector(whole_c, b); // c = Ax - b
        
        solved = is_solve(whole_c, len_b);

        mul_vector_scalar(whole_c, t); // c = t(Ax - b)
        sub_vector(x, whole_c); // x = x - t(Ax - b)
    }
}

bool is_correct(vector<double> &u, vector<double> &x, int N) {
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

int mpi_init(int N, int *rank, int *size, int *start_row, int k) {
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);

    // if (*rank == 0) {
    //     cout << "Enter the size of the matrix N: ";
    //     cin >> *N;
    //     cout << "number of launches: ";
    //     cin >> *k;
    //     cout << "\nNumber of processes: " << *size << "\n";
    // }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int rows_per_process = N / *size;
    int remainder = N % *size;

    *start_row = *rank * rows_per_process;
    int end_row = (*rank + 1) * rows_per_process;

    if (*rank < remainder) {
        *start_row += *rank;
        end_row += *rank + 1;
    } else {
        *start_row += remainder;
        end_row += remainder;
    }

    return end_row - *start_row;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int N = atoi(argv[1]), rank, size, start_row, k = atoi(argv[2]);
    int rows = mpi_init(N, &rank, &size, &start_row, k);
    if (rank == 0) cout << "N: " << N << ", launches: " << k << "\n";
    cout << "rank: " << rank << "; start row: " << start_row << "; rows: " << rows << "\n";

    int min = 999999999;
    for (int i = 0; i < k; ++i) {
        auto start = high_resolution_clock::now();

        vector<double> A(N * rows);
        init_A(A, N, rows, start_row);
        
        vector<double> u(N);
        init_random(u, N);
        
        vector<double> b(rows);
        mul_matrix(A, u, b, rows);
        vector<double> whole_B(N);
        MPI_Allgather(b.data(), rows, MPI_DOUBLE, whole_B.data(), rows, MPI_DOUBLE, MPI_COMM_WORLD);

        vector<double> x(N, 0.0);
        solve_system(A, x, whole_B, N, rows);
        
        if (i == 0 && rank == 0) is_correct(u, x, N);
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        if (duration.count() < min) min = duration.count();
    }
    cout << "Lead time: " << min << " milliseconds\n-----------------\n";
    
    MPI_Finalize();
    return 0;
}