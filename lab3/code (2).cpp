
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <mpi.h>
#include <algorithm> // для std::min

using namespace std;

// --- Функции для работы с матрицами ---
vector<vector<double>> create_matrix(int rows, int cols, double fill_value = -1.0) {
    vector<vector<double>> matrix(rows, vector<double>(cols));
    if (fill_value == -1.0) { // Используем -1.0 как признак необходимости заполнения случайными числами
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix[i][j] = dis(gen);
            }
        }
    } else {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix[i][j] = fill_value;
            }
        }
    }
    return matrix;
}

vector<vector<double>> matrix_multiply(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int n1 = A.size();
    int n2 = A[0].size();
    int n3 = B[0].size();

    vector<vector<double>> C(n1, vector<double>(n3, 0.0));

    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n3; ++j) {
            for (int k = 0; k < n2; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

bool verify_result(const vector<vector<double>>& C_parallel, const vector<vector<double>>& C_serial, double tolerance = 1e-5) {
    if (C_parallel.size() != C_serial.size() || C_parallel[0].size() != C_serial[0].size()) {
        return false;
    }

    for (size_t i = 0; i < C_parallel.size(); ++i) {
        for (size_t j = 0; j < C_parallel[0].size(); ++j) {
            if (abs(C_parallel[i][j] - C_serial[i][j]) > tolerance) {
                return false;
            }
        }
    }
    return true;
}


// --- MPI-функции для 2D-решетки ---
void get_2d_process_grid(int size, int& p1, int& p2) {
    // Находим ближайшие делители для формирования решетки.  Стремимся к квадратной форме.
    p1 = 0;
    p2 = 0;
    for (int i = 1; i <= (int)sqrt(size); ++i) {
        if (size % i == 0) {
            p1 = i;
        }
    }
    if (p1 == 0) { // size - простое число
        p1 = 1;
        p2 = size;
    } else {
        p2 = size / p1;
    }
}

void get_process_coords(int rank, int p1, int p2, int& row, int& col) {
    row = rank / p2;
    col = rank % p2;
}

// --- Основная функция параллельного умножения ---
vector<vector<double>> parallel_matrix_multiply(const vector<vector<double>>& A, const vector<vector<double>>& B,
                                              int p1, int p2, MPI_Comm comm, int rank, int row_coord, int col_coord) {
    int n1 = A.size();
    int n2 = A[0].size();
    int n3 = B[0].size();

    int rows_per_process = n1 / p1;
    int cols_per_process = n3 / p2;

    // 1. Распределение полос A по строкам решетки (x, 0)
    vector<vector<double>> A_strip(rows_per_process, vector<double>(n2));
    if (col_coord == 0) {
        for (int i = 0; i < rows_per_process; ++i) {
            for (int j = 0; j < n2; ++j) {
                A_strip[i][j] = A[row_coord * rows_per_process + i][j];
            }
        }
    }

    MPI_Bcast(A_strip.data()->data(), rows_per_process * n2, MPI_DOUBLE, row_coord * p2, comm);

    // 2. Распределение полос B по столбцам решетки (0, y)
    vector<vector<double>> B_strip(n2, vector<double>(cols_per_process));
    if (row_coord == 0) {
        for (int i = 0; i < n2; ++i) {
            for (int j = 0; j < cols_per_process; ++j) {
                B_strip[i][j] = B[i][col_coord * cols_per_process + j];
            }
        }
    }
    MPI_Bcast(B_strip.data()->data(), n2 * cols_per_process, MPI_DOUBLE, col_coord, comm);


    // 3. Вычисление подматрицы C на каждом процессе
    vector<vector<double>> C_submatrix = create_matrix(rows_per_process, cols_per_process, 0.0);
    for (int i = 0; i < rows_per_process; ++i) {
        for (int j = 0; j < cols_per_process; ++j) {
            for (int k = 0; k < n2; ++k) {
                C_submatrix[i][j] += A_strip[i][k] * B_strip[k][j];
            }
        }
    }

    // 4. Сбор подматриц C в матрицу C на процессе 0 с использованием GatherV
    vector<vector<double>> C;
    if (rank == 0) {
        C.resize(n1, vector<double>(n3, 0.0));
    }

    int sendcounts[p1 * p2];
    int displs[p1 * p2];

    for (int i = 0; i < p1; ++i) {
        for (int j = 0; j < p2; ++j) {
            int process_rank = i * p2 + j;
            sendcounts[process_rank] = rows_per_process * cols_per_process;
            displs[process_rank] = (i * rows_per_process * n3) + (j * cols_per_process);
        }
    }

    double* recvbuf = nullptr;
    if (rank == 0) {
        recvbuf = C.data()->data();
    }

    MPI_Gatherv(C_submatrix.data()->data(), rows_per_process * cols_per_process, MPI_DOUBLE,
                  recvbuf, sendcounts, displs, MPI_DOUBLE, 0, comm);


    return C;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Размеры матриц (можно менять для экспериментов)
    int n1 = 512;
    int n2 = 512;
    int n3 = 512;

    // Инициализация матриц на процессе 0
    vector<vector<double>> A, B;
    if (rank == 0) {
        A = create_matrix(n1, n2);
        B = create_matrix(n2, n3);
    }

    // Определение размера решетки
    int p1, p2;
    get_2d_process_grid(size, p1, p2);

    if (rank == 0) {
        cout << "Using a " << p1 << " x " << p2 << " process grid." << endl;
    }

    // Определение координат процесса в решетке
    int row_coord, col_coord;
    get_process_coords(rank, p1, p2, row_coord, col_coord);

    // Проверка, что число процессов подходит для решетки
    if (size != p1 * p2) {
        if (rank == 0) {
            cerr << "Error: Number of processes (" << size << ") does not match the grid size (" << p1 << " x " << p2 << ")." << endl;
        }
        MPI_Abort(comm, 1);
    }

    // Засекаем время начала
    double start_time = MPI_Wtime();

    // Параллельное умножение
    vector<vector<double>> C_parallel;
    if (rank==0){
      C_parallel = parallel_matrix_multiply(A, B, p1, p2, comm, rank, row_coord, col_coord);
    } else {
        A.resize(n1, vector<double>(n2)); // need to resize otherwise will give sigmentation fault
        B.resize(n2, vector<double>(n3));
        C_parallel = parallel_matrix_multiply(A, B, p1, p2, comm, rank, row_coord, col_coord);

    }


    // Засекаем время окончания
    double end_time = MPI_Wtime();

    // Выводим время выполнения на процессе 0
    if (rank == 0) {
        double parallel_time = end_time - start_time;
        cout << "Parallel Multiplication Time: " << parallel_time << " seconds" << endl;

        // Последовательное умножение (для проверки)
        start_time = MPI_Wtime();
        vector<vector<double>> C_serial = matrix_multiply(A, B);
        end_time = MPI_Wtime();
        double serial_time = end_time - start_time;
        cout << "Serial Multiplication Time: " << serial_time << " seconds" << endl;

        // Проверка результата
        if (verify_result(C_parallel, C_serial)) {
            cout << "Result is correct." << endl;
        } else {
            cout << "Result is incorrect." << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
