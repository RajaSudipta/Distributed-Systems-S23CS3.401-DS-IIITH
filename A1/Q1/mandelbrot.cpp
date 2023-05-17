#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

// mpic++ mandelbrot.cpp
// mpirun -np 6 ./a.out 30 30 10

void compute_mandelbrot_subset(int* result, int N, int M, int max_iter, int start, int end) 
{
    int result_size = N * M;

    // −1.5 − i and 1 + i
    double x_begin = -1.5, y_begin = -1;
    double x_end = 1, y_end = 1;

    // double x_step = (x_end - x_begin) / (N - 1);
    // double y_step = (y_end - y_begin) / (M - 1);

    double x_step = (x_end - x_begin) / (N - 1);
    double y_step = (y_end - y_begin) / (M - 1);

    int i, j;
    complex<double> c, z;

    cout << "start: " << start << ", end: " << end-1 << endl;
    for (i = start; i < end; i++) 
    {
        c = complex<double>(
                x_begin + (i % N) * x_step,
                y_begin + (i / N) * y_step

                // x_begin + (i % M) * x_step, // x = xbegin + j*dx
                // y_end - (i / M) * y_step // y = yend - i*dy
        );
        z = 0; j = 0;

        // If we iterate K times and find that the magnitude of z K is still less than 2, 
        // we can conclude c is a point in the Mandelbrot set.
        while (norm(z) <= 2.0 && j < max_iter) 
        {
            z = z*z + c;
            j++;
        }
        if(j == max_iter)
        {
            result[i-start] = 1;
        }
        else
        {
            result[i-start] = 0;
        }
        // result[i-start] = j % result_size;
    }

    // for (i = start; i < end; i++) 
    // {
    //     double x = x_begin + (i % M) * x_step;
    //     double y = y_end - (i / M) * y_step;

    //     double zx = 0.0, zy = 0.0;
    //     for(int i=0; i<max_iter; i++)
    //     {
    //         sqr(zx, zy);
    //         zx = zx + x;
    //         zy = zy + y;
    //         if(mag(zx, zy) >= 2.0)
    //         {
    //             result[i-start] = 0;
    //             break;
    //         }
    //     }

    //     if(mag(zx, zy) >= 2.0)
    //     {
    //         result[i-start] = 0;
    //     }
    //     else
    //     {
    //         result[i-start] = 1;
    //     }
    // }

}

void run(int N, int M, int max_iter, int my_rank, int size) 
{
    int result_size = N * M;
    int* result;
    // int part_width = result_size / size;
    int part_width = ceil((double)(result_size) / (double)(size));
    int start = my_rank * part_width;
    // if(start !=0)
    // {
    //     // cout << "start: " << start << endl;
    //     // cout << "executed" << endl;
    //     start += my_rank;
    //     // cout << "start: " << start << endl;
    // }
    int* partial_result = new int[part_width];
    // memset(partial_result, 0, part_width);

    if (my_rank == 0 ) 
    {
        result = new int[result_size];
        // memset(result, 0, result_size);
    }

    int end;
    if(start+part_width > result_size)
    {
        end = result_size;
    }
    else
    {
        end = start+part_width;
    }
    // cout << "start: " << start << ", end: " << end << endl;
    // compute_mandelbrot_subset(partial_result, N, M, max_iter, start, start+part_width);
    compute_mandelbrot_subset(partial_result, N, M, max_iter, start, end);
    MPI_Gather(partial_result, part_width, MPI_INT, result, part_width, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) 
    {
        cout << endl;
        for(int i=0; i<result_size; i++)
        {
            int xx = i/N;
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

    N = stoi(argv[1]);
    M = stoi(argv[2]);
    K = stoi(argv[3]);

    MPI_Init(NULL, NULL);       

    int my_rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // cout << my_rank << " " << size << endl;
    
    run(N, M, K, my_rank, size);

    MPI_Finalize();

    return 0;
}
