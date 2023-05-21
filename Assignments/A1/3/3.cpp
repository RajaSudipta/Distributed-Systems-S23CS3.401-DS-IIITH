#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;

// 2
// 10 34
// 12 50

// 3
// 10 34
// 12 8
// 20 50

// 4
// 10 34
// 12 8
// 20 50
// 25 60

// 5
// 10 34
// 12 8
// 20 50
// 25 60
// 30 65

// 8
// 10 34
// 12 8
// 20 50
// 25 60
// 30 65
// 35 70
// 40 75
// 45 80

// 8
// 5 7
// 3 6
// 7 5
// 6 17
// 1 16
// 4 8
// 2 13
// 8 28

long long int sum(int freq_arr[], int i, int j)
{
    long long int sum = 0;
    int last_index = -1;
    for (int k = i; k <= j; k++)
    {
        int fq = freq_arr[k];
        sum = sum + freq_arr[k];
        last_index = k;
    }
    return sum;
}

void bcast_sorted_key(int *sorted_key_arr, int n)
{
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(sorted_key_arr, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
}

void bcast_freq_arr(int *sorted_freq_arr, int n)
{
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(sorted_freq_arr, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
}

long long int optimal_search_tree(int n, int my_rank, int cell_size_per_process, int size, vector<vector<int>> &root, bool status, int *key_arr, int *freq_arr, bool debug)
{
    long long int cost[n][n];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cost[i][j] = 0;
        }
    }
    int i = 0;
    while (i < n)
    {
        cost[i][i] = freq_arr[i];
        i++;
    }

    for (int length = 2; length <= n; length++)
    {
        int size_of_diagonal = n - length + 1;
        int effective_no_of_processes = min(size_of_diagonal, size);
        int part_width = ceil((double)size_of_diagonal / (double)effective_no_of_processes);
        long long int *small_arr = new long long int[part_width];
        long long int *big_arr = new long long int[part_width * size];

        int *small_root_arr = new int[part_width];
        int *big_root_arr = new int[part_width * size];
        int kk = 0;

        for (int i = my_rank * part_width; i < min(((my_rank + 1) * part_width), size_of_diagonal); i++)
        {
            int j = i + length - 1;
            long long int minm = LLONG_MAX;
            int rt = -1;
            long long int offset_sum = sum(freq_arr, i, j);
            // cout << "offset_sum: " << offset_sum << endl;
            for (int r = i; r <= j; r++)
            {
                // if(my_rank == 0)
                // {
                //     if(r > i)
                //     {
                //         cout << "cost[" << i << "][" << r-1 << "] = " << cost[i][r-1] << endl;
                //     }
                //     if(r < j)
                //     {
                //         cout << "cost[" << r+1 << "][" << j << "] = " << cost[r+1][j] << endl;
                //     }
                // }
                long long int c = ((r > i) ? cost[i][r - 1] : 0) + ((r < j) ? cost[r + 1][j] : 0) + offset_sum;

                if (c < minm)
                {
                    minm = c;
                    rt = r;
                }
            }
            small_arr[kk] = minm;
            small_root_arr[kk] = rt;
            kk++;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(small_arr, part_width, MPI_LONG_LONG, big_arr, part_width, MPI_LONG_LONG, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        // if(my_rank == 0)
        // {
        //     cout << "********** Printing the diagonal of length = " << size_of_diagonal << " **********" << endl;
        //     for(int i=0; i<size_of_diagonal; i++)
        //     {
        //         cout << big_arr[i] << " ";
        //     }
        //     cout << endl << "********** END of printing **********" << endl << endl;
        // }


        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(small_root_arr, part_width, MPI_INT, big_root_arr, part_width, MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        int k = 0;
        for (int i = 0; i < n - length + 1; i++)
        {
            int j = i + length - 1;
            cost[i][j] = big_arr[k];
            root[i][j] = big_root_arr[k];
            k++;
        }

        // if(my_rank == 0)
        // {
        //     cout << "************* Printing the cost array *************" << endl;
        //     for(int i=0; i<n; i++)
        //     {
        //         for(int j=0; j<i; j++)
        //         {
        //             cout << "XX" << " ";
        //         }
        //         for(int j=i; j<n; j++)
        //         {
        //             cout << cost[i][j] << " ";
        //         }
        //         cout << endl;
        //     }
        //     cout << "************* END of printing *************" << endl << endl;
        // }

        delete[] small_arr;
        delete[] big_arr;
        delete[] small_root_arr;
        delete[] big_root_arr;
    }
    return cost[0][n - 1];
}

void fill_parent(int *sorted_key_arr, int i, int j, int arr_size, int parent, bool status, vector<vector<int>> &root, map<int, int> &node_parent, bool debug)
{
    if (i == j)
    {
        node_parent[sorted_key_arr[i]] = parent;
        return; // handles mapping of leaf node and parent
    }
    if (j < i || i < 0 || j < 0 || i >= arr_size || j >= arr_size)
    {
        return;
    }
    int r = root[i][j];
    if (r != -1)
    {
        node_parent[sorted_key_arr[r]] = parent;
        fill_parent(sorted_key_arr, i, r - 1, arr_size, sorted_key_arr[r], true, root, node_parent, false);
        fill_parent(sorted_key_arr, r + 1, j, arr_size, sorted_key_arr[r], true, root, node_parent, false);
    }
}

vector<pair<int, int>> merge_subarray(vector<pair<int, int>> &key_freq, int elements_per_proc, int size_Of_Cluster)
{
    vector<pair<int, int>> ans;
    vector<priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>>> pq(size_Of_Cluster);
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>> pq_root;
    int i, k;
    for (i = 0;;)
    {
        if (i >= size_Of_Cluster)
        {
            break;
        }
        k = i * elements_per_proc;
        int j;
        for (j = 0;;)
        {
            if ((j >= elements_per_proc))
            {
                break;
            }
            if ((k + j) >= key_freq.size())
            {
                break;
            }
            pq[i].push(make_pair(key_freq[k + j].first, make_pair(i, key_freq[k + j].second)));

            j++;
        }
        if (!pq[i].empty())
        {
            pair<int, pair<int, int>> temp = pq[i].top();
            pq[i].pop();
            pq_root.push(temp);
        }
        i++;
    }
    while (true)
    {
        if (pq_root.empty())
        {
            break;
        }
        ans.push_back(make_pair(pq_root.top().first, pq_root.top().second.second));
        if (pq[pq_root.top().second.first].empty())
        {
            pq_root.pop();
            continue;
        }
        else
        {
            pq_root.push(pq[pq_root.top().second.first].top());
            pq[pq_root.top().second.first].pop();
        }

        pq_root.pop();
    }
    return ans;
}

void run_helper(int n, int size, int my_rank)
{
    int cell_size_per_process = ceil((double)(n) / (double)(size));
    int bigger_num_of_elements = size * cell_size_per_process;
    int bigger_arr_size = 2 * bigger_num_of_elements;
    int *key_freq_arr;
    int partial_arr_size = 2 * cell_size_per_process;
    int *partial_res = new int[partial_arr_size];

    if (my_rank == 0)
    {
        key_freq_arr = new int[bigger_arr_size];
        memset(key_freq_arr, INT_MAX, bigger_arr_size);
        for (int i = 0; i < bigger_arr_size; i++)
        {
            key_freq_arr[i] = INT_MAX;
        }
        for (int i = 0; i < n; i++)
        {
            int k, f;
            cin >> k;
            cin >> f;
            key_freq_arr[2 * i] = k;
            key_freq_arr[2 * i + 1] = f;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(key_freq_arr, partial_arr_size, MPI_INT, partial_res, partial_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    vector<pair<int, int>> vec; // (key, freq)
    for (int i = 0; i < cell_size_per_process; i++)
    {
        vec.push_back({partial_res[2 * i], partial_res[2 * i + 1]});
    }

    sort(vec.begin(), vec.end());

    for (int i = 0; i < vec.size(); i++)
    {
        partial_res[2 * i] = vec[i].first;
        partial_res[2 * i + 1] = vec[i].second;
    }

    vec.clear();

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(partial_res, partial_arr_size, MPI_INT, key_freq_arr, partial_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    int *sorted_key_arr = new int[n];
    int *sorted_freq_arr = new int[n];

    if ((my_rank == 0))
    {
        vector<pair<int, int>> key_freq_vec;
        for (int i = 0; i < n; i++)
        {
            key_freq_vec.push_back(make_pair(key_freq_arr[2 * i], key_freq_arr[2 * i + 1]));
        }

        key_freq_vec = merge_subarray(key_freq_vec, cell_size_per_process, size);

        for (int i = 0; i < n; i++)
        {
            sorted_key_arr[i] = key_freq_vec[i].first;
            sorted_freq_arr[i] = key_freq_vec[i].second;
        }
    }

    bcast_sorted_key(sorted_key_arr, n);
    bcast_freq_arr(sorted_freq_arr, n);

    vector<vector<int>> root(n, vector<int>(n, -1));
    long long int val = optimal_search_tree(n, my_rank, cell_size_per_process, size, root, true, sorted_key_arr, sorted_freq_arr, false);

    if (my_rank == 0)
    {

        map<int, int> node_parent;
        fill_parent(sorted_key_arr, 0, n - 1, n, 0, true, root, node_parent, false);
        node_parent[sorted_key_arr[root[0][n - 1]]] = 0;

        cout << val << endl;

        for (auto &a : node_parent)
        {
            cout << a.second << " ";
        }
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    int n;
    MPI_Init(&argc, &argv);

    int my_rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0)
    {
        cin >> n;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    run_helper(n, size, my_rank);

    MPI_Finalize();

    return 0;
}