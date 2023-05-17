#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

// mpic++ mandelbrot.cpp
// mpirun -np 6 ./a.out 30 30 10
// mpirun -np 6 ./a.out 16 16 1000
// mpirun -np 6 ./a.out 20  15 1000
// mpirun -np 6 ./a.out 15 20 1000
// mpirun -np 7 ./a.out 16 16 1000 // problem with 7, not divisible,  check it

// void compute_mandelbrot_subset(int* result, int N, int M, int max_iter, int start, int end) 
void compute_mandelbrot_subset(bool* result, int N, int M, int max_iter, int start, int end) 
{
    // −1.5 − i and 1 + i
    double x_begin = -1.5;
    double y_begin = -1;
    double x_end = 1;
    double y_end = 1;

    // cout << "N: " << N << ", M: " << M << endl;

    double x_step = (x_end - x_begin) / (M - 1);
    double y_step = (y_end - y_begin) / (N - 1);

    // cout << "x_step: " << x_step << ", y_step: " << y_step << endl;

    // complex<double> c, z;

    // cout << "start: " << start << ", end: " << end-1 << endl;

    for (int i=start; i<end; i++) 
    {
        // cout << "hi" << endl;
        // cout << "flat: " << i << endl;
        // cout << "i: " << i/M << ", j: " << i%M << endl;

        double xx = x_begin + (i % M) * x_step; // x = xbegin + j*dx
        double yy = y_end - (i / M) * y_step; // y = yend - i*dy
        // cout << "x: " << xx << ", y: " << yy << endl;

        complex<double> c = complex<double>(
                // x_begin + (i % N) * x_step,
                // y_begin + (i / N) * y_step

                x_begin + (i % M) * x_step, // x = xbegin + j*dx
                y_end - (i / M) * y_step // y = yend - i*dy

        );

        complex<double> z = 0; 
        int j = 0;

        // If we iterate K times and find that the magnitude of z K is still less than or equal to 2, 
        // we can conclude c is a point in the Mandelbrot set.
        while (norm(z) <= 2.0 && j < max_iter) 
        {
            z = z*z + c;
            j++;
        }

        // cout << "value: " << j << endl;

        if(j == max_iter)
        {
            result[i-start] = 1;
        }
        else
        {
            result[i-start] = 0;
        }
    }

    // cout << "printing the array" << endl;
    // for(int i=start; i<end; i++)
    // {
    //     cout << result[i] << " ";
    // }
    // cout << endl;

}

void run_helper(int N, int M, int max_iter, int size, int my_rank) 
{
    int result_size = N * M;
    // int* result;
    bool* result;
    // int part_width = result_size / size;
    int part_width = ceil((double)(result_size) / (double)(size));
    int start = my_rank * part_width;
    // int* partial_result = new int[part_width];
    // int* partial_result;
    bool* partial_result;
    // memset(partial_result, 0, part_width);

    if (my_rank == 0 ) 
    {
        // result = new int[result_size];
        result = new bool[result_size];
        // memset(result, 0, result_size);
    }

    int end;
    if(start+part_width >= result_size)
    {
        end = result_size;
        // partial_result = new int[end-start];
        partial_result = new bool[end-start];
    }
    else
    {
        end = start+part_width;
        // partial_result = new int[part_width];
        partial_result = new bool[part_width];
    }
    // cout << "start: " << start << ", end: " << end << endl;
    // compute_mandelbrot_subset(partial_result, N, M, max_iter, start, start+part_width);
    compute_mandelbrot_subset(partial_result, N, M, max_iter, start, end);
    // MPI_Gather(partial_result, part_width, MPI_INT, result, part_width, MPI_INT, 0, MPI_COMM_WORLD);
    // cout << "before gateher " << "start: " << start << ", end: " << end << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Gather(partial_result, (end-start), MPI_INT, result, (end-start), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(partial_result, (end-start), MPI_CXX_BOOL, result, (end-start), MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // cout << "gather success for process: " << my_rank << endl; 


    if (my_rank == 0) 
    {
        cout << endl;
        for(int i=0; i<result_size; i++)
        {
            // int xx = i/M;
            int yy = i%M;

            cout << result[i];

            if(yy == M-1)
            {
                cout << endl;
            }
        }
        delete[] result;
    }

    delete[] partial_result;
}


// You have to write a program using MPI that computes the Mandelbrot set after K iterations for N ∗ M points
// spaced uniformly in the region of the complex plane bounded by −1.5 − i and 1 + i in a distributed manner.
int main(int argc, char *argv[])
{
    // the number of points along x axis: N
    // number ofpoints along y axis: M 
    // number of iterations: K
    // 1 <= N ∗ M ∗ K <= 1000000
    int N, M, K; 
    // cin >> N >> M >> K;

    // N = stoi(argv[1]);
    // M = stoi(argv[2]);
    // K = stoi(argv[3]);

    MPI_Init(NULL, NULL);       

    int my_rank;
    int size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0)
    {
        // cin >> N >> M >> K;
        cin >> M >> N >> K;
    } 
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast( &N, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast( &M, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast( &K, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Barrier(MPI_COMM_WORLD);

    // cout << my_rank << " " << size << endl;
    
    run_helper(N, M, K, size, my_rank);

    MPI_Finalize();

    return 0;
}
