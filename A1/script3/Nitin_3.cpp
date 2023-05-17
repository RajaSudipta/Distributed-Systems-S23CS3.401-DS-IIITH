#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> ppi;
// extra delay can be due to long long in prefix sum array or unnecessary barrier
// convert int to long long wherever reqired, A utility function to get sum of 
// array elements freq[i] to freq[j]
long long sum_(vector<long long> &prefix_sum_holder, int i, int j)
{
    return prefix_sum_holder[j + 1] - prefix_sum_holder[i];
}
vector<long long> prefix_sum2(int *freq, int N)
{
    long long sum = 0;
    vector<long long> prefix_sum_holder;
    prefix_sum_holder.push_back(sum);
    for (int k = 0; k < N; k++)
    {
        sum += freq[k];
        prefix_sum_holder.push_back(sum);
    }
    return prefix_sum_holder;
}
int get_start(int process_number, int per_process_size)
{
    return process_number * per_process_size;
}
int get_end(int process_number, int per_process_size)
{
    return (process_number + 1) * per_process_size;
}
int get_j(int i, int length)
{ // to get (j) for corresponding i
    return i + length - 1;
}
int get_diagonal_size(int i, int length)
{ // to get (number of diagonal elements) for corresponding i
    return i - length + 1;
}
int optimal_search_tree(int *freq_arr, int n, int my_rank, int cell_size_per_process, int num_process, vector<vector<int>> &root, vector<long long> &prefix_sum_holder)
{
    vector<vector<int>> cost(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
    {
        cost[i][i] = freq_arr[i];
    }
    for (int length = 2; length <= n; length++)
    {
        int diagonal_size = get_diagonal_size(n, length);
        int total_working_process = min(diagonal_size, num_process);
        int per_process_size = ceil((double)diagonal_size / total_working_process);
        int arr_len_with_dummy = per_process_size * num_process;
        // if process is rank is more than total_working_process, then each of
        // the process should get to create 1 elemets only for sub_cost_arr or sub_root_arr
        int *sub_cost_arr = (int *)malloc(sizeof(int) * per_process_size);
        int *sub_root_arr = (int *)malloc(sizeof(int) * per_process_size);
        int *cost_arr = (int *)malloc(sizeof(int) * arr_len_with_dummy);
        int *root_arr = (int *)malloc(sizeof(int) * arr_len_with_dummy);
        int counter = 0;
        int c;
        int start_pos = get_start(my_rank, per_process_size);
        int end_pos = get_end(my_rank, per_process_size);
        end_pos = min(end_pos, diagonal_size);
        int j;
        for (int i = start_pos; i < end_pos; i++)
        {
            sub_cost_arr[counter] = INT_MAX;
            j = get_j(i, length);
            sub_root_arr[counter] = -1;
            int off_set_sum = sum_(prefix_sum_holder, i, j);
            for (int r = i; r <= j; r++)
            {
                c = 0;
                if (r > i)
                {
                    c += cost[i][r - 1];
                }
                if (r < j)
                {
                    c += cost[r + 1][j];
                }
                c += off_set_sum;
                if (c < sub_cost_arr[counter])
                {
                    sub_cost_arr[counter] = c;
                    sub_root_arr[counter] = r;
                }
            }
            counter++;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(sub_cost_arr, per_process_size, MPI_INT, cost_arr, per_process_size, MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(sub_root_arr, per_process_size, MPI_INT, root_arr, per_process_size, MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        int k = 0;
        for (int i = 0; i < diagonal_size; i++)
        {
            j = get_j(i, length);
            cost[i][j] = cost_arr[k];
            root[i][j] = root_arr[k++];
        }
        free(sub_cost_arr);
        free(sub_root_arr);
        free(cost_arr);
        free(root_arr);
    }
    return cost[0][n - 1];
}
bool valid_index(int i, int j, int size)
{
    if (j < i || i < 0 || j < 0 || i >= size || j >= size)
    {
        return false;
    }
    return true;
}
void fill_parent(vector<vector<int>> &root, map<int, int> &node_parent, int *sorted_key_arr, int i, int j, int arr_size, int parent)
{
    if (valid_index(i, j, arr_size) == false)
    {
        return;
    }
    if (i == j)
    {
        node_parent[sorted_key_arr[i]] = parent;
        return; // handles mapping of leaf node and parent
    }
    int r = root[i][j];
    if (r == -1)
    {
        return; // invalid parent, no node has -1 as it's value
    }
    else
    {
        node_parent[sorted_key_arr[r]] = parent;
        int sub_tree_parent = sorted_key_arr[r];
        fill_parent(root, node_parent, sorted_key_arr, i, r - 1, arr_size, sub_tree_parent);
        fill_parent(root, node_parent, sorted_key_arr, r + 1, j, arr_size, sub_tree_parent);
    }
}
void merge_subarray(vector<pii> &key_freq, int elements_per_proc, int num_process)
{
    vector<priority_queue<ppi, vector<ppi>, greater<ppi>>> pq(num_process);
    priority_queue<ppi, vector<ppi>, greater<ppi>> pq_root;
    for (int i = 0; i < num_process; i++)
    {
        int k = i * elements_per_proc;
        for (int j = 0; (j < elements_per_proc) && ((k + j) < key_freq.size()); j++)
        {
            int key = key_freq[k + j].first;
            int freq = key_freq[k + j].second;
            int heap_num = i;
            pq[i].push({key, {freq, heap_num}});
        }
        if (!pq[i].empty())
        {
            pq_root.push(pq[i].top());
            pq[i].pop();
        }
    }
    key_freq.clear();
    while (!pq_root.empty())
    {
        ppi temp = pq_root.top();
        pq_root.pop();
        int key = temp.first;
        int freq = temp.second.first;
        int heap_number = temp.second.second;
        key_freq.push_back({key, freq});
        if (!pq[heap_number].empty())
        {
            pq_root.push(pq[heap_number].top());
            pq[heap_number].pop();
        }
    }
}

int main(int argc, char **argv)
{
    int process_Rank, num_process, N;
    int key, freq;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);
    if ((0 == process_Rank))
    {
        cin >> N;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    int elements_per_proc = ceil((double)N / num_process);
    int total_num_ele = elements_per_proc * num_process;
    int *key_then_freq = NULL;
    if ((0 == process_Rank))
    {
        int i = 0;
        key_then_freq = (int *)malloc(2 * total_num_ele * sizeof(int));
        for (; i < 2 * N;)
        {
            cin >> key;
            cin >> freq;
            key_then_freq[i++] = key;
            key_then_freq[i++] = freq;
        }
        for (; i < 2 * total_num_ele;)
        {
            key_then_freq[i++] = INT_MAX;
            key_then_freq[i++] = INT_MAX;
        }
    }
    int *sub_arr = NULL;
    sub_arr = (int *)malloc(2 * elements_per_proc * sizeof(int));
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(key_then_freq, 2 * elements_per_proc, MPI_INT, sub_arr, 2 * elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    vector<pii> holder_key_freq;
    for (int i = 0; i < 2 * elements_per_proc;)
    {
        int key = sub_arr[i++];
        int freq = sub_arr[i++];
        holder_key_freq.push_back({key, freq});
    }
    sort(holder_key_freq.begin(), holder_key_freq.end());
    for (int i = 0; i < holder_key_freq.size(); i++)
    {
        int key = holder_key_freq[i].first;
        sub_arr[2 * i] = key;
        int freq = holder_key_freq[i].second;
        sub_arr[2 * i + 1] = freq;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(sub_arr, 2 * elements_per_proc, MPI_INT, key_then_freq, 2 * elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    holder_key_freq.clear();
    vector<pii> key_freq;
    if (0 == process_Rank)
    {
        for (int i = 0; i < 2 * total_num_ele;)
        {
            int key = key_then_freq[i++];
            int freq = key_then_freq[i++];
            if (freq != INT_MAX)
            {
                key_freq.push_back({key, freq});
            }
        }
        merge_subarray(key_freq, elements_per_proc, num_process);
    }
    int t;
    int *flattened_key_freq = (int *)malloc(2 * sizeof(int) * N);
    if (0 == process_Rank)
    {
        int i = 0;
        for (int t = 0; t < key_freq.size(); t++)
        {
            flattened_key_freq[i] = key_freq[t].first;
            i++;
            flattened_key_freq[i] = key_freq[t].second;
            i++;
        }
    }
    key_freq.clear();
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(flattened_key_freq, 2 * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    int *sorted_key = (int *)malloc(sizeof(int) * N);
    int *freq_array = (int *)malloc(sizeof(int) * N);
    int r = 0;
    for (int i = 0; i < (2 * N);)
    {

        sorted_key[r] = flattened_key_freq[i];
        i++;
        freq_array[r] = flattened_key_freq[i];
        i++;
        r++;
    }
    free(flattened_key_freq);
    vector<long long> prefix_sum_holder = prefix_sum2(freq_array, N);
    vector<vector<int>> root(N, vector<int>(N, -1));
    int val = optimal_search_tree(freq_array, N, process_Rank, elements_per_proc, num_process, root, prefix_sum_holder);
    if ((0 == process_Rank))
    {
        cout << val << endl;
        map<int, int> node_parent; // node_parent
        fill_parent(root, node_parent, sorted_key, 0, N - 1, N, 0);
        node_parent[sorted_key[root[0][N - 1]]] = 0;
        for (auto &a : node_parent)
        {
            cout << a.second << " ";
        }
    }
    MPI_Finalize();
    return 0;
}
