#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <mpi.h>
#include <algorithm>
#include <cstdlib>
// #include <cblas.h>

// ssh -i new_key -p 22 hpcuser145@84.237.117.10 -X
// mpicxx -O2 -o main main.cpp
// ~/sw/bin/mpecc -mpilog -O2 -o main main.cpp компиляция
// mpirun -np 2 ./main
// scp -i new_key ./lab3/main.cpp hpcuser145@84.237.117.10:~/lab3
// ~/sw/bin/jumpshot

using namespace std;

template <typename T>
void copy_matrix(T* A, T* B, int n1, int n2) {
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n2; ++j) {
            B[i * n2 + j] = A[i * n2 + j];
        }
    }
}

template <typename T>
void print_matrix(T* matrix, int rows, int cols) {
    cout << "Матрица " << rows << " * " << cols << "\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i * cols + j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

template <typename T>
void create_matrix(T* matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i * cols + j] = sin(2 * 3.14 * i / rows);
        }
    }
}

template <typename T>
void matrix_multiply(const T* A, const T* B, T* C, int n1, int n2, int n3) {
    for (int i = 0; i < n1; ++i) {
        for (int k = 0; k < n2; ++k) {
            for (int j = 0; j < n3; ++j) {
                C[i * n3 + j] += A[i * n2 + k] * B[k * n3 + j];
            }
        }
    }
}

template <typename T>
bool verify_result(const T* A, const T* B, int rows, int cols, double tolerance = 1e-5) {
    if (A == nullptr || B == nullptr) return false; // добавленная проверка
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (abs(A[i * cols + j] - B[i * cols + j]) > tolerance) {
                return false;
            }
        }
    }
    return true;
}

void distribute_matrix_by_lines(const double* matrix, double* stripe_matrix, int blocks, int len_blocks, int step, MPI_Comm *comm_row, int size) {
    MPI_Datatype column_type, final_column_type;
    MPI_Type_vector(
        blocks,                                        // Количество блоков
        len_blocks,                                  // Длина каждого блока
        step,                                        // Шаг между началом каждого блока
        MPI_DOUBLE,                                   // Тип данных
        &column_type);                                // Коммуникатор

    MPI_Type_commit(&column_type);
    
    MPI_Type_create_resized(column_type, 0, len_blocks * sizeof(double), &final_column_type);
    MPI_Type_commit(&final_column_type);
    
    int* sendcounts = new int[size];
    int* displacements = new int[size];
    for (int i = 0; i < size; ++i) {
        sendcounts[i] = 1;
        displacements[i] = i;
    }
    MPI_Scatterv(matrix, sendcounts, displacements, final_column_type,
        stripe_matrix, blocks * len_blocks, MPI_DOUBLE, 0, *comm_row);

    MPI_Type_free(&final_column_type);  
    MPI_Type_free(&column_type);
}

void init_2dgrid(int *size, int dims[], int *global_rank, MPI_Comm *comm_row, MPI_Comm *comm_col) {
    MPI_Comm_size(MPI_COMM_WORLD, size);
    MPI_Comm_rank(MPI_COMM_WORLD, global_rank);
    // определение размеров решетки: dims
    MPI_Dims_create(*size, 2, dims);
    if (*global_rank == 0) {
        printf("Процессов %d, решетка высотой %d, шириной %d\n", *size, dims[0], dims[1]);
    }
    MPI_Comm_split(MPI_COMM_WORLD, *global_rank % dims[1], *global_rank, comm_col);
    MPI_Comm_split(MPI_COMM_WORLD, *global_rank / dims[1], *global_rank, comm_row);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    // dims - размеры решетки по каждой размерности
    int dims[2] = {0, 0};
    int size, global_rank;
    MPI_Comm comm_row, comm_col;

    init_2dgrid(&size, dims, &global_rank, &comm_row, &comm_col);

    int n1 = atoi(argv[1]);
    int n2 = atoi(argv[2]);
    int n3 = atoi(argv[3]);

    double start_time = MPI_Wtime();

    double* A = nullptr;
    double* B = nullptr;

    if (global_rank == 0) {
        A = new double[n1 * n2];
        create_matrix(A, n1, n2);
        // cout << "Матрица А:\n";
        // print_matrix(A, n1, n2);

        B = new double[n2 * n3];
        create_matrix(B, n2, n3);
        // cout << "Матрица В:\n";
        // print_matrix(B, n2, n3);
    }

    int rows_stripeA = n1 / dims[0];
    double* horizontal_stripeA = new double[rows_stripeA * n2];
    for(int i = 0; i < rows_stripeA * n2; ++i) horizontal_stripeA[i] = 0.0;

    if (global_rank % dims[1] == 0) {
        MPI_Scatter(A, rows_stripeA * n2, MPI_DOUBLE, horizontal_stripeA, rows_stripeA * n2, MPI_DOUBLE, 0, comm_col);
    }
    MPI_Bcast(horizontal_stripeA, rows_stripeA * n2, MPI_DOUBLE, 0, comm_row);
    // for (int i = 0; i < size; ++i) {
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (i == global_rank) {
    //         cout << "----" << global_rank << "----" << "\n" << "Полоса матрицы А:\n";
    //         print_matrix(horizontal_stripeA, rows_stripeA, n2);
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }

    int cols_stripeB = n3 / dims[1];
    double* vertical_stripeB = new double[n2 * cols_stripeB];
    for(int i = 0; i < n2 * cols_stripeB; ++i) vertical_stripeB[i] = 0.0;
    if (global_rank / dims[1] == 0) {
        distribute_matrix_by_lines(B, vertical_stripeB, n2, cols_stripeB, n3, &comm_row, size);
    }
    MPI_Bcast(vertical_stripeB, cols_stripeB * n2, MPI_DOUBLE, 0, comm_col);
    // for (int i = 0; i < size; ++i) {
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (i == global_rank) {
    //         cout << "----" << global_rank << "----" << "\n" << "Полоса матрицы B:\n";
    //         print_matrix(vertical_stripeB, n2, cols_stripeB);
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }

    double* submatrixC = new double[rows_stripeA * cols_stripeB];
    for(int i = 0; i < rows_stripeA * cols_stripeB; ++i) submatrixC[i] = 0.0;

    matrix_multiply(horizontal_stripeA, vertical_stripeB, submatrixC, rows_stripeA, n2, cols_stripeB);
    // for (int i = 0; i < size; ++i) {
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (i == global_rank) {
    //         cout << "----" << global_rank << "----" << "\n" << "Подматрица С:\n";
    //         print_matrix(submatrixC, rows_stripeA, cols_stripeB);
    //     }
    // }

    double* C = nullptr;
    int *recvcounts = new int[size];
    int *displs = new int[size];
    if (global_rank == 0) {
        C = new double[n1 * n3];
        for(int i = 0; i < n1 * n3; ++i) C[i] = 0.0;
        for(int i = 0; i < size; ++i) {
            recvcounts[i] = 1;
            displs[i] = (i / dims[1]) * dims[1] * rows_stripeA + (i % dims[1]);
        }
    }
    MPI_Datatype submatrix_type, final_submatrix_type;
    MPI_Type_vector(rows_stripeA, cols_stripeB, n3, MPI_DOUBLE, &submatrix_type);
    MPI_Type_commit(&submatrix_type);

    MPI_Type_create_resized(submatrix_type, 0, cols_stripeB * sizeof(double), &final_submatrix_type);
    MPI_Type_commit(&final_submatrix_type);

    MPI_Gatherv(submatrixC, rows_stripeA * cols_stripeB, MPI_DOUBLE, C, recvcounts, displs, final_submatrix_type, 0, MPI_COMM_WORLD);

    MPI_Type_free(&final_submatrix_type);
    MPI_Type_free(&submatrix_type);

    // if (global_rank == 0) {
    //     cout << "Результат:\n";
    //     print_matrix(C, n1, n3);
    // }

    double end_time = MPI_Wtime();

    if (global_rank == 0) {
        double parallel_time = end_time - start_time;
        cout << "Время параллельного умножения: " << parallel_time << " секунд\n";

        start_time = MPI_Wtime();
        double* C_serial = new double[n1 * n3];
        for(int i = 0; i < n1 * n3; ++i) C_serial[i] = 0.0;
        matrix_multiply(A, B, C_serial, n1, n2, n3);
        end_time = MPI_Wtime();
        double serial_time = end_time - start_time;
        cout << "Время последовательного умножения: " << serial_time << " секунд\n";

        if (verify_result(C, C_serial, n1, n3)) {
            cout << "Результат правильный.\n";
        } else {
            cout << "Результат НЕверен.\n";
        }

        delete[] C_serial;
    }

    delete[] horizontal_stripeA;
    delete[] vertical_stripeB;
    delete[] submatrixC;

    if (global_rank == 0) {
        delete[] A;
        delete[] B;
        delete[] C;
    }

    MPI_Finalize();
    return 0;
}
