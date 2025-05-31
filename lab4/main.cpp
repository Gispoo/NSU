#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <cstdlib>

using namespace std;

// mpicxx -O2 -o main main.cpp
// mpirun -np 2 ./main
// ssh -i new_key -p 22 hpcuser145@84.237.117.10 -X
// scp -i new_key ./lab4/main.cpp hpcuser145@84.237.117.10:~/lab4
// ~/sw/bin/mpecc -mpilog -O2 -o main main.cpp
// ~/sw/bin/jumpshot

double tolerance = 1e-5;
double a = 1e5;
double epsilon = 1e-11;
double Dx;
double x0;
double d;

int Nx;
int Ny;
int Nz;

double hx;
double hy;
double hz;

void init(int size, char **argv, int rank)
{
    Dx = 2.0 / size;
    x0 = -1.0 + rank * Dx;

    Nx = atoi(argv[1]) / size;
    Ny = atoi(argv[2]);
    Nz = atoi(argv[3]);

    hx = Dx / (Nx - 1);
    hy = 2.0 / (Ny - 1);
    hz = 2.0 / (Nz - 1);

    d = 1 / (2 / (hx * hx) + 2 / (hy * hy) + 2 / (hz * hz) + a);
}

bool verify_result(int rank, const vector<double> &A)
{
    bool solution = true;
    for (int i = 0; i < Nx; ++i)
    {
        for (int j = 0; j < Ny; ++j)
        {
            for (int k = 0; k < Nz; ++k)
            {
                double x = x0 + i * hx;
                double y = -1.0 + j * hy;
                double z = -1.0 + k * hz;
                if (abs(A[i * Ny * Nz + j * Nz + k] - (x * x + y * y + z * z)) > tolerance)
                {
                    // printf("%d: i: %d, j: %d, k: %d\nphi: %lf vs %lf\n", rank, i, j, k, A[i * Ny * Nz + j * Nz + k], (x * x + y * y + z * z));
                    solution = false;
                }
            }
        }
    }
    return solution;
}

void calculate(vector<double> &phi_next, vector<double> &phi, vector<double> &bottom_wall, int bottom_shift, vector<double> &top_wall, int top_shift, double &local_delta, int i)
{
    for (int j = 1; j < Ny - 1; ++j)
    {
        for (int k = 1; k < Nz - 1; ++k)
        {
            double x = x0 + i * hx;
            double y = -1 + j * hy;
            double z = -1 + k * hz;

            phi_next[i * Ny * Nz + j * Nz + k] =
                d * ((top_wall[top_shift + j * Nz + k] + bottom_wall[bottom_shift + j * Nz + k]) / (hx * hx) +
                     (phi[i * Ny * Nz + (j + 1) * Nz + k] + phi[i * Ny * Nz + (j - 1) * Nz + k]) / (hy * hy) +
                     (phi[i * Ny * Nz + j * Nz + k + 1] + phi[i * Ny * Nz + j * Nz + k - 1]) / (hz * hz) -
                     (6 - a * (x * x + y * y + z * z)));

            local_delta = max(local_delta, abs(phi_next[i * Ny * Nz + j * Nz + k] - phi[i * Ny * Nz + j * Nz + k]));
        }
    }
}

void init_matrix(vector<double> &phi)
{
    for (int i = 0; i < Nx; ++i)
    {
        for (int j = 0; j < Ny; ++j)
        {
            for (int k = 0; k < Nz; ++k)
            {
                double x = x0 + i * hx;
                double y = -1 + j * hy;
                double z = -1 + k * hz;

                if (fabs(x - (-1.0)) < tolerance || fabs(x - 1.0) < tolerance ||
                    fabs(y - (-1.0)) < tolerance || fabs(y - 1.0) < tolerance ||
                    fabs(z - (-1.0)) < tolerance || fabs(z - 1.0) < tolerance)
                {
                    phi[i * Ny * Nz + j * Nz + k] = x * x + y * y + z * z;
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cout << "not enough arguments\n";
        return 0;
    }

    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double start_time = MPI_Wtime();

    init(size, argv, rank);

    vector<double> phi(Nx * Ny * Nz, 0.0);

    init_matrix(phi);
    vector<double> phi_next = phi;
    printf("%d/%d: Nx: %d, x0: %lf, hx: %lf\n", rank, size, Nx, x0, hx);

    double global_delta = 1;

    int iteration = 0;
    for (; global_delta > epsilon && iteration < 100; ++iteration)
    {
        double local_delta = 0.0;
        vector<double> bottom_wall(Ny * Nz, 0.0);
        vector<double> top_wall(Ny * Nz, 0.0);

        MPI_Request sending_the_bottom_wall, getting_the_bottom_wall;
        MPI_Request sending_the_top_wall, getting_the_top_wall;

        if (rank != 0)
        {
            MPI_Isend(phi.data(), Ny * Nz, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &sending_the_bottom_wall);
            MPI_Irecv(bottom_wall.data(), Ny * Nz, MPI_DOUBLE, rank - 1, 2, MPI_COMM_WORLD, &getting_the_bottom_wall);
        }

        if (rank != size - 1)
        {
            MPI_Isend(phi.data() + (Nx - 1) * Ny * Nz, Ny * Nz, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD, &sending_the_top_wall);
            MPI_Irecv(top_wall.data(), Ny * Nz, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &getting_the_top_wall);
        }

        for (int i = 1; i < Nx - 1; ++i)
        {
            int bottom_shift = (i - 1) * Ny * Nz;
            int top_shift = (i + 1) * Ny * Nz;
            calculate(phi_next, phi, phi, bottom_shift, phi, top_shift, local_delta, i);
        }

        MPI_Status sending_status, getting_status;

        if (rank != 0)
        {
            MPI_Wait(&sending_the_bottom_wall, &sending_status);
            MPI_Wait(&getting_the_bottom_wall, &getting_status);

            int bottom_shift = 0;
            int top_shift = Ny * Nz;
            int i = 0;
            calculate(phi_next, phi, bottom_wall, bottom_shift, phi, top_shift, local_delta, i);
        }

        if (rank != size - 1)
        {
            MPI_Wait(&sending_the_top_wall, &sending_status);
            MPI_Wait(&getting_the_top_wall, &getting_status);

            int bottom_shift = (Nx - 2) * Ny * Nz;
            int top_shift = 0;
            int i = Nx - 1;
            calculate(phi_next, phi, phi, bottom_shift, top_wall, top_shift, local_delta, i);
        }

        phi = phi_next;
        MPI_Allreduce(&local_delta, &global_delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        // if (fabs(local_delta - global_delta) > tolerance) {
        //     printf("%d: Iteration = %d, delta up (+%lf)\n", rank, iteration, global_delta - local_delta);
        // }
    }
    double end_time = MPI_Wtime();

    for (int i = 0; i < size; ++i)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        if (i == rank)
        {
            // cout << "\n" << rank << ": Iteration " << iteration << ", delta = " << global_delta << "\n";

            if (verify_result(rank, phi))
            {
                cout << rank << ": Solution is correct\n";
            }
            else
            {
                cout << rank << ": Solution is INcorrect\n";
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0)
        cout << "\nLead time: " << end_time - start_time << "\n";

    MPI_Finalize();

    return 0;
}