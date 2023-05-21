#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
typedef pair<int, int> pii;
#define ROOT_PROCESS 0
void sum(long long &sum_, int freq_holder[], int i, int j)
{
    sum_ = 0;
    for (int k = j; k >= i; k--)
    {
        sum_ = sum_ + freq_holder[k];
    }
}
int *allocate_space(int size)
{
    int *arr = new int[size];
    return arr;
}
void get_cost_array(int *sub_array, int sub_part_size, int *merged_array)
{
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(sub_array, sub_part_size, MPI_INT, merged_array, sub_part_size, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
}
void get_root_array(int *sub_root_arr, int sub_part_size, int *merged_root_arr)
{
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(sub_root_arr, sub_part_size, MPI_INT, merged_root_arr, sub_part_size, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
}
void get_sorted_pair_arr(int *partial_res, int partial_arr_size, int *key_freq_holder)
{
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(partial_res, partial_arr_size, MPI_INT, key_freq_holder, partial_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
}
int optimal_search_tree(int *key_arr, int *freq_holder, int n, int my_process_rank, int size_per_process, bool flag, int size, vector<vector<int>> &root)
{
    vector<vector<int>> cost = vector<vector<int>>(n, vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i != j)
            {
                cost[i][j] = 0;
            }
            else
            {
                cost[i][i] = freq_holder[i];
            }
        }
    }
    int *sub_array = NULL;
    int *merged_array = NULL;
    int *sub_root_arr = NULL;
    int *merged_root_arr = NULL;
    int size_of_diagonal = 0, eff_num_processes = 0, sub_part_size = 0;
    for (int length = 2; length <= n; length++)
    {
        size_of_diagonal = n - length + 1;
        eff_num_processes = min(size_of_diagonal, size);
        sub_part_size = ceil((double)size_of_diagonal / (double)eff_num_processes);
        sub_array = allocate_space(sub_part_size);
        sub_root_arr = allocate_space(sub_part_size);
        merged_array = allocate_space(sub_part_size * size);
        merged_root_arr = allocate_space(sub_part_size * size);
        int kk = 0;
        int lower_limit = my_process_rank * sub_part_size;
        int upper_limit = size_of_diagonal;
        upper_limit = min(upper_limit, ((my_process_rank + 1) * sub_part_size));
        for (int i = lower_limit; i < upper_limit; i++)
        {
            int j = i + length - 1;
            int minm = INT_MAX;
            int rt = -1;
            long long offset_sum;
            sum(offset_sum, freq_holder, i, j);
            for (int r = i; r <= j; r++)
            {
                long long c = offset_sum;
                c += ((r < j) ? cost[r + 1][j] : 0);
                c += ((r > i) ? cost[i][r - 1] : 0);
                if (c < minm)
                {
                    minm = c;
                    rt = r;
                }
            }
            sub_array[kk] = minm;
            sub_root_arr[kk] = rt;
            kk++;
        }
        get_cost_array(sub_array, sub_part_size, merged_array);
        get_root_array(sub_root_arr, sub_part_size, merged_root_arr);

        int k = 0, i = 0;
        int j;
        while (i < n - length + 1)
        {
            j = i + length - 1;
            cost[i][j] = merged_array[k];
            root[i][j] = merged_root_arr[k++];
            i++;
        }
    }
    return cost[0][n - 1];
}

void fill_parent(int i, vector<vector<int>> &root, int *sorted_key_arr, bool debugger_on, map<int, int> &node_parent, int j, int arr_size, int parent)
{
    bool expression = (j < i) || (i < 0) || (j < 0) || (i >= arr_size) || (j >= arr_size);
    int int_equivalent = ((int)expression);
    switch (int_equivalent)
    {
    case 1:
        break;
    case 0:
        if (i == j)
        {
            node_parent[sorted_key_arr[i]] = parent;
        }
        int r = root[i][j];
        if (r == -1)
        {
            break;
        }
        if (r != -1)
        {
            node_parent[sorted_key_arr[r]] = parent;
            fill_parent(i, root, sorted_key_arr, false, node_parent, r - 1, arr_size, sorted_key_arr[r]);
            fill_parent(r + 1, root, sorted_key_arr, false, node_parent, j, arr_size, sorted_key_arr[r]);
        }
        break;
    }
    return;
}
vector<pair<int, int>> merge_subarray(vector<pair<int, int>> &key_freq_holder, bool debugger_on, int ele_per_proc, int size_Of_Cluster)
{
    vector<priority_queue<pair<int, pii>, vector<pair<int, pii>>, greater<pair<int, pii>>>> pq;
    pq.resize(size_Of_Cluster);
    vector<pair<int, int>> ans;
    priority_queue<pair<int, pii>, vector<pair<int, pii>>, greater<pair<int, pii>>> pq_root;
    int i = 0, k;
    while (true)
    {
        if (i >= size_Of_Cluster)
        {
            break;
        }
        k = i * ele_per_proc;
        int j = 0;
        while (true)
        {
            if (!((j < ele_per_proc) && ((k + j) < key_freq_holder.size())))
            {
                break;
            }
            pii temps1_ = make_pair(i, key_freq_holder[k + j].second);
            pair<int, pii> temps2_ = make_pair(key_freq_holder[k + j].first, temps1_);
            pq[i].push(temps2_);
            j++;
        }
        if (pq[i].empty())
        {
            ;
        }
        else
        {
            pair<int, pii> temp = pq[i].top();
            pq_root.push(temp);
            pq[i].pop();
        }
        i++;
    }
    do
    {
        if (pq_root.empty())
        {
            break;
        }
        pii temp_ = {pq_root.top().first, pq_root.top().second.second};
        ans.push_back(temp_);
        if (pq[pq_root.top().second.first].empty())
        {
            pq_root.pop();
        }
        else
        {
            pq_root.push(pq[pq_root.top().second.first].top());
            pq[pq_root.top().second.first].pop();
            pq_root.pop();
        }
    } while (true);
    return ans;
}
void display_function(int cost_val, map<int, int> &node_parent)
{
    cout << cost_val << endl;
    for (auto &a : node_parent)
    {
        cout << a.second << " ";
    }
}
void solver(int n, int size, int my_process_rank)
{
    int size_per_process = ceil((double)(n) / (size));
    int bigger_num_of_elements = size * size_per_process;
    int bigger_arr_size = 2 * bigger_num_of_elements;
    int *key_freq_holder = NULL;
    int partial_arr_size = 2 * size_per_process;
    int *partial_res = NULL;
    partial_res = allocate_space(partial_arr_size);
    if (my_process_rank == ROOT_PROCESS)
    {
        key_freq_holder = allocate_space(bigger_arr_size);
        memset(key_freq_holder, INT_MAX, bigger_arr_size);
        for (int i = 0; i < bigger_arr_size; i++)
        {
            key_freq_holder[i] = INT_MAX;
        }
        int k, f, i = 0;
        while (true)
        {
            if (!(i < n))
            {
                break;
            }
            cin >> k >> f;
            key_freq_holder[2 * i] = k;
            key_freq_holder[2 * i + 1] = f;
            i++;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(key_freq_holder, partial_arr_size, MPI_INT, partial_res, partial_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    vector<pii> vec; // (key, freq)
    int i = 0;
    while (true)
    {
        if (i >= size_per_process)
        {
            break;
        }
        pii temps_ = make_pair(partial_res[2 * i], partial_res[2 * i + 1]);
        vec.push_back(temps_);
        i++;
    }
    sort(vec.begin(), vec.end());
    i = 0;
    while (true)
    {
        if (i >= vec.size())
        {
            break;
        }
        partial_res[2 * i] = vec[i].first;
        partial_res[2 * i + 1] = vec[i].second;
        i++;
    }
    vec.clear();
    get_sorted_pair_arr(partial_res, partial_arr_size, key_freq_holder);
    int *sorted_key_arr = NULL;
    int *sorted_freq_holder = NULL;
    sorted_key_arr = allocate_space(n);
    sorted_freq_holder = allocate_space(n);

    if ((my_process_rank == ROOT_PROCESS))
    {
        int i = 0;
        vector<pii> key_freq_vec;
        do
        {
            if (i >= n)
            {
                break;
            }
            key_freq_vec.push_back(make_pair(key_freq_holder[2 * i], key_freq_holder[2 * i + 1]));
            i++;
        } while (true);

        key_freq_vec = merge_subarray(key_freq_vec, false, size_per_process, size);
        i = 0;
        do
        {
            if (i >= n)
            {
                break;
            }
            sorted_key_arr[i] = key_freq_vec[i].first;
            sorted_freq_holder[i] = key_freq_vec[i].second;
            i++;
        } while (true);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(sorted_key_arr, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(sorted_freq_holder, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    vector<vector<int>> root = vector<vector<int>>(n, vector<int>(n, -1));
    int val = optimal_search_tree(sorted_key_arr, sorted_freq_holder, n, my_process_rank, size_per_process, false, size, root);
    if (my_process_rank == ROOT_PROCESS)
    {
        map<int, int> node_parent;
        fill_parent(0, root, sorted_key_arr, false, node_parent, n - 1, n, 0);
        node_parent[sorted_key_arr[root[0][n - 1]]] = 0;
        display_function(val, node_parent);
    }
}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int my_process_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_process_rank);
    int n;
    if (my_process_rank == ROOT_PROCESS)
    {
        cin >> n;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    solver(n, size, my_process_rank);
    MPI_Finalize();
    return 0;
}

/*
GFG Input:
2
10 34
12 50

GFG Output:
Optimal cost = 118
Node: 10 Parent: 12
Node: 12 Parent: 0

GFG Input;
3
10 34
12 8
20 50

GFG Output:
Optimal cost = 142
Node: 10 Parent: 20
Node: 12 Parent: 10
Node: 20 Parent: 0


Sudipta Input:
4
10 34
12 8
20 50
25 60

Sudipta Output:
Optimal cost = 262
Node: 10 Parent: 20
Node: 12 Parent: 10
Node: 20 Parent: 0
Node: 25 Parent: 20


Sudipta Input:
5
10 34
12 8
20 50
25 60
30 65

Sudipta Output:
Optimal cost = 424
Node: 10 Parent: 20
Node: 12 Parent: 10
Node: 20 Parent: 25
Node: 25 Parent: 0
Node: 30 Parent: 25

Sudipta Input:
8
10 34
12 8
20 50
25 60
30 65
35 70
40 75
45 80
Sudipta Output:
Optimal cost = 1079
Node: 10 Parent: 20
Node: 12 Parent: 10
Node: 20 Parent: 30
Node: 25 Parent: 20
Node: 30 Parent: 0
Node: 35 Parent: 40
Node: 40 Parent: 30
Node: 45 Parent: 40

PDF Input:
8
5 7
3 6
7 5
6 17
1 16
4 8
2 13
8 28

PDF Output:
238
Node: 1 Parent: 2
Node: 2 Parent: 6
Node: 3 Parent: 4
Node: 4 Parent: 2
Node: 5 Parent: 4
Node: 6 Parent: 0
Node: 7 Parent: 8
Node: 8 Parent: 6

*/

