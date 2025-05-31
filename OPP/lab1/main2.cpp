#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <mpi.h>
#include <cstdlib>

// ssh -i lin_priv -p 2222 opp145@84.237.52.21
// ssh -i new_key -p 22 hpcuser145@84.237.117.10
// Ov2P7z-qP
// mpicxx -O2 -o 2 main2.cpp
// mpecxx -mpilog -O2 -o 2 main2.cpp
// mpirun -np 2 ./2 10 1
// scp -i lin_priv -P 2222 /home/gispo/OPP/lab1/parallel/main2.cpp opp145@84.237.52.21:/home/opp145/lab1/parallel
// scp -i new_key ./lab1/main2.cpp hpcuser145@84.237.117.10:~/lab1

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

void mul_matrix(vector<double> &A, vector<double> &x, vector<double> &res, int N, int rank, int size) {
    int rows = x.size();
    MPI_Status status;
    fill(res.begin(), res.end(), 0.0);
    
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < rows; ++i) {
            for (int k = 0; k < rows; ++k) {
                res[i] += A[N * i + k + (j * rows)] * x[k];
            }
        }
        vector<double> x_temp = x;

        MPI_Sendrecv(x_temp.data(), rows, MPI_DOUBLE, (rank + 1) % size, 1,
                    x.data(), rows, MPI_DOUBLE, (rank - 1 + size) % size, 1,
                    MPI_COMM_WORLD, &status);
    }
}

void sub_vector(vector<double> &res, vector<double> &B) {
    int N = res.size();

    for (int i = 0; i < N; ++i) {
        res[i] = res[i] - B[i];
    }
    // cout << "sub vector succsec\n";
}

double len_vec(vector<double> &A) {
    int N = A.size();

    double local_sum = 0.0;
    for (int i = 0; i < N; ++i) {
        local_sum += A[i] * A[i];
    }

    double global_sum = 0.0;
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    return sqrt(global_sum);
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

    // cout << "calculateTAY succsec\n";
    return 1.0 / absolute_maxSum; 
}

void solve_system(vector<double> &A, vector<double> &x, vector<double> &b, int N, int rows, int rank, int size) {
    vector<double> c(rows, 0.0);

    bool solved = false;
    double t = calculateTAY(A, N, rows);
    double len_b = len_vec(b);
    while(!solved) {
        mul_matrix(A, x, c, N, rank, size); // c = Ax

        sub_vector(c, b); // c = Ax - b
        
        solved = is_solve(c, len_b);

        mul_vector_scalar(c, t); // c = t(Ax - b)
        sub_vector(x, c); // x = x - t(Ax - b)
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
    //     cout << "["  << *rank << "] Enter the size of the matrix N: ";
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

int main(int argc,char *argv[]) {
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
        
        vector<double> u(rows);
        init_random(u, rows);
        
        vector<double> b(rows);
        mul_matrix(A, u, b, N, rank, size);

        vector<double> x(rows, 0.0);
        solve_system(A, x, b, N, rows, rank, size);
        
        if (i == 0 && rank == 0) {
            is_correct(u, x, rows);
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        if (duration.count() < min) {
            min = duration.count();
        }
    }
    cout << "Lead time: " << min << " milliseconds\n-----------------\n";
    
    MPI_Finalize();
    return 0;
}