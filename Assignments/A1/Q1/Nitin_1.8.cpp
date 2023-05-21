#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
int get_start(int process_num, int per_process_size)
{
    return process_num * per_process_size;
}
int get_end(int process_num, int per_process_size)
{
    return (process_num + 1) * per_process_size;
}
double get_delta(double end, double start, int parts)
{
    return (end - start) / (parts - 1);
}
void compute_mandelbrot_subset(int *result, int N, int M, int K, int process_num, int per_process_size)
{
    int start = get_start(process_num, per_process_size);
    int end = get_end(process_num, per_process_size);
    double start_x = -1.5, start_y = -1, end_x = 1, end_y = 1; // −1.5 − i and 1 + i
    double delta_x = get_delta(end_x, start_x, M);
    double delta_y = get_delta(end_y, start_y, N);
    for (int i = start; i < end; i++)
    {
        complex<double> z = 0;
        double comp_val = end_y - (i / M) * delta_y;
        double real_val = start_x + (i % M) * delta_x;
        complex<double> c = complex<double>(real_val, comp_val);
        int j = 0;
        for (; j < K && norm(z) <= 2.0; j++)
        {
            z = z * z + c;
        }
        if ((j == K) && (norm(z) <= 2.0))
        {
            result[i - start] = 1;
        }
        else if ((j < K) && (norm(z) <= 2.0))
        {
            result[i - start] = 1;
        }
        else if ((j == K) && (norm(z) > 2.0))
        {
            result[i - start] = 0;
        }
        else if ((j < K) && (norm(z) > 2.0))
        {
            result[i - start] = 0;
        }
    }
}

int main(int argc, char **argv)
{
    int process_num, size_Of_Cluster;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_num);
    int M, N, K, P;
    if ((0 == process_num))
    {
        cin >> M;
        cin >> N;
        cin >> K;
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int *result;
    int per_process_size = ceil((double)(N * M) / (double)(size_Of_Cluster));
    int result_size_with_dummy_data = per_process_size * size_Of_Cluster;
    int *temp_result = (int *)malloc(per_process_size * sizeof(int));
    if (process_num == 0)
    {
        result = (int *)malloc(result_size_with_dummy_data * sizeof(int));
    }
    compute_mandelbrot_subset(temp_result, N, M, K, process_num, per_process_size);
    int start = get_start(process_num, per_process_size);
    int end = get_end(process_num, per_process_size);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(temp_result, (end - start), MPI_INT, result, (end - start), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (process_num == 0)
    {
        for (int i = 0; i < N * M; i++)
        {
            cout << result[i] << " ";
            if ((i % M) == M - 1)
            {
                cout << endl;
            }
        }
        free(result);
    }
    free(temp_result);
}