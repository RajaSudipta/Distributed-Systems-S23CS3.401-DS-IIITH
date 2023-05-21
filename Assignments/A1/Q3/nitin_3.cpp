#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
// A utility function to get sum of array elements freq[i] to freq[j]
long long sum(vector<long long> &prefix_sum_holder, int i, int j)
{
    return prefix_sum_holder[j + 1] - prefix_sum_holder[i];
}
// prefix sum calculated to find freq sum from i to j
vector<long long> prefix_sum(vector<pair<int, int>> &key_freq)
{
    long long sum = 0;
    vector<long long> prefix_sum_holder;
    prefix_sum_holder.push_back(sum);
    for (int k = 0; k < key_freq.size(); k++)
    {
        sum += key_freq[k].second;
        prefix_sum_holder.push_back(sum);
    }
    return prefix_sum_holder;
}
int optimalSearchTree(vector<pair<int, int>> key_freq, vector<long long> &prefix_sum_holder, vector<vector<int>> &root)
{
    int n = key_freq.size();
    int cost[n][n];
    /* cost[i][j] = Optimal cost of binary search tree that can be formed from keys[i] to keys[j].
    cost[0][n-1] will store the resultant cost */
    /* root[i][j] = tells index(r) of key array which becomes root node.
    then parent[key[root[i][r-1]]]=key[root[i][j]] and  parent[key[root[r+1][j]]]=key[root[i][j]] */
    // For a single key, cost is equal to frequency of the key
    int *prev_row = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        cost[i][i] = key_freq[i].second;
        prev_row[i] = cost[i][i];
        // root[i][i] = something; //needed?
    }
    int *curr_row = NULL;
    // Now we need to consider chains of length 2, 3, ... . L is chain length.
    for (int L = 2; L <= n; L++)
    {
        // i is row number in cost[][]
        for (int i = 0; i <= n - L + 1; i++)
        {
            // Get column number j from row number i and chain length L
            int j = i + L - 1;
            cost[i][j] = INT_MAX;
            int off_set_sum = sum(prefix_sum_holder, i, j);
            // Try making all keys in interval keys[i..j] as root
            for (int r = i; r <= j; r++)
            {
                int c = 0;
                if (r > i)
                {
                    c += cost[i][r - 1];
                }
                if (r < j)
                {
                    c += cost[r + 1][j];
                }
                c += off_set_sum;
                if (c < cost[i][j])
                {
                    cost[i][j] = c;
                    root[i][j] = r;
                }
            }
        }
    }
    return cost[0][n - 1];
}
void fill_parent(vector<vector<int>> &root, map<int, int> &node_parent, vector<pair<int, int>> &key_freq, int i, int j, int parent)
{
    if (i == j)
    {
        node_parent[key_freq[i].first] = parent;
        return; // handles mapping of leaf node and parent
    }
    if (j < i || i < 0 || j < 0 || i >= key_freq.size() || j >= key_freq.size())
    {
        return;
    }
    int r = root[i][j];
    if (r != -1)
    {
        node_parent[key_freq[r].first] = parent;
        fill_parent(root, node_parent, key_freq, i, r - 1, key_freq[r].first);
        fill_parent(root, node_parent, key_freq, r + 1, j, key_freq[r].first);
    }
}
void display_vector(vector<pair<int,int>> &temp){
    for(auto &a:temp){
        cout<<"{"<<a.first<<" "<<a.second<<"} ";
    }
    cout<<endl;
}
typedef pair<int, pair<int,int>> ppi;
vector<pair<int,int>> merge_subarray(vector<pair<int,int>>&key_freq,int elements_per_proc,int size_Of_Cluster){
    vector<pair<int,int>>ans;
    vector<priority_queue<ppi, vector<ppi>, greater<ppi> >> pq(size_Of_Cluster);
    priority_queue<ppi, vector<ppi>, greater<ppi> > pq_root;
    for(int i=0;i<size_Of_Cluster;i++){
        int k=i*elements_per_proc;
        for(int j=0;(j<elements_per_proc)&&((k+j)<key_freq.size());j++){
            pq[i].push({key_freq[k+j].first,{key_freq[k+j].second,i}});
        }
        if(!pq[i].empty()){
            pq_root.push(pq[i].top());
            cout<<"*******************"<<endl;
            cout<<"key:"<<pq[i].top().first<<endl;
            cout<<"freq:"<<pq[i].top().second.first<<endl;
            cout<<"heap num:"<<pq[i].top().second.second<<endl;
            cout<<"*******************"<<endl;
            pq[i].pop();
        }
        
        // pq[i].pop();
        //i will be 1 or 0
    }
    while(!pq_root.empty()){
        ppi temp=pq_root.top();
        pq_root.pop();
        int key=temp.first;
        int freq=temp.second.first;
        int heap_number=temp.second.second;
        ans.push_back({key,freq});
        if(!pq[heap_number].empty()){
            pq_root.push(pq[heap_number].top());
            pq[heap_number].pop();
        }
    }
    return ans;
/*
mpirun -np 4 ./a.out 
5 234 657 231 8769 2367 78654 12 2345 678 243
Before sorting by: 0
{234 657} {231 8769} 
After sorting by: 0
{231 8769} {234 657} 
*******************
key:231
freq:8769
heap num:0
*******************
*******************
key:12
freq:2345
heap num:1
*******************
*******************
key:678
freq:243
heap num:2
*******************
Before sorting by: 1
{2367 78654} {12 2345} 
After sorting by: 1
{12 2345} {2367 78654} 
Before sorting by: 2
{678 243} {2147483647 2147483647} 
After sorting by: 2
{678 243} {2147483647 2147483647} 
Before sorting by: 3
{2147483647 2147483647} {2147483647 2147483647} 
After sorting by: 3
{2147483647 2147483647} {2147483647 2147483647} 

a.out:49760 terminated with signal 11 at PC=56030084b2db SP=7ffe7e948510.  Backtrace:
./a.out(+0x72db)[0x56030084b2db]
./a.out(+0x5f97)[0x560300849f97]
./a.out(+0x4d0e)[0x560300848d0e]
./a.out(+0x40ee)[0x5603008480ee]
./a.out(+0x2ff9)[0x560300846ff9]
./a.out(+0x37e4)[0x5603008477e4]
/lib/x86_64-linux-gnu/libc.so.6(+0x29d90)[0x7fecf3aa1d90]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80)[0x7fecf3aa1e40]
./a.out(+0x2425)[0x560300846425]

===================================================================================
=   BAD TERMINATION OF ONE OF YOUR APPLICATION PROCESSES
=   PID 49762 RUNNING AT nitin-linux
=   EXIT CODE: 9
=   CLEANING UP REMAINING PROCESSES
=   YOU CAN IGNORE THE BELOW CLEANUP MESSAGES
===================================================================================
YOUR APPLICATION TERMINATED WITH THE EXIT STRING: Killed (signal 9)
This typically refers to a problem with your application.
Please see the FAQ page for debugging suggestions
*/
    
    // vector<pair<int,int>>ans;
    // sort(key_freq.begin(),key_freq.end());
    // ans=key_freq;
    // return ans;
}
int main(int argc, char **argv)
{
    int process_Rank, size_Of_Cluster;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);
    int N;
    if ((0 == process_Rank))
    {
        cin >> N;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    int key, freq;
    // vector<pair<int, int>> key_freq;
    int elements_per_proc = ceil((double)N / size_Of_Cluster);
    int total_num_ele = elements_per_proc * size_Of_Cluster;
    int *key_then_freq = NULL;
    key_then_freq = (int *)malloc(2 * total_num_ele * sizeof(int));
    // vector<int>key_then_freq;
    if ((0 == process_Rank))
    {
        int i = 0;
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
            // add a check if freq is INT_MAX or not
        }
        // for (i = 0; i < 2 * total_num_ele;)
        // {
        //     cout<<key_then_freq[i++]<<" ";
        //     cout<<key_then_freq[i++]<<" ";
        // }
    }
    // // MPI_sort(key_freq.begin(), key_freq.end());
    int *sub_arr = NULL;
    sub_arr = (int *)malloc(2 * elements_per_proc * sizeof(int));
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Scatter(key_then_freq, 2 * total_num_ele, MPI_INT, sub_arr, 2*elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(key_then_freq, 2 * elements_per_proc, MPI_INT, sub_arr, 2*elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    vector<pair<int, int>> holder_key_freq;
    for (int i = 0; i < 2 * elements_per_proc;)
    {
        int key = sub_arr[i++];
        int freq = sub_arr[i++];
        holder_key_freq.push_back({key, freq});
    }
    cout<<"Before sorting by: "<<process_Rank<<endl;
    display_vector(holder_key_freq);
    sort(holder_key_freq.begin(), holder_key_freq.end());
    cout<<"After sorting by: "<<process_Rank<<endl;
    display_vector(holder_key_freq);
    for (int i = 0; i < holder_key_freq.size(); i++)
    {
        sub_arr[2 * i] = holder_key_freq[i].first;
        sub_arr[2 * i + 1] = holder_key_freq[i].second;
    }
    holder_key_freq.clear();
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Gather(sub_arr, 2 * elements_per_proc, MPI_INT, key_then_freq, 2 * total_num_ele, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(sub_arr, 2 * elements_per_proc, MPI_INT, key_then_freq, 2 * elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // // MPI_sort(key_freq);
    vector<pair<int, int>> key_freq;
    if ((0 == process_Rank))
    {
        for (int i = 0; i < 2 * total_num_ele;)
        {
            int key = key_then_freq[i++];
            int freq = key_then_freq[i++];
            if (freq != INT_MAX)
                key_freq.push_back({key, freq});
        }
        key_freq=merge_subarray(key_freq,elements_per_proc,size_Of_Cluster);
        cout<<"Merged After sorting by: "<<process_Rank<<endl;
        display_vector(key_freq);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if ((0 == process_Rank))
    {
        map<int, int> node_parent; // node_parent
        vector<long long> prefix_sum_holder = prefix_sum(key_freq);
        vector<vector<int>> root(N, vector<int>(N, -1));
        cout << "Cost of Optimal BST is " << optimalSearchTree(key_freq, prefix_sum_holder, root) << endl;

        cout << "Printing the root 2d array" << endl;
        for(int i=0; i<N; i++)
        {
            for(int j=0; j<N; j++)
            {
                cout << root[i][j] << " ";
            }
            cout << endl;
        }
        fill_parent(root, node_parent, key_freq, 0, key_freq.size() - 1, 0);
        node_parent[key_freq[root[0][N - 1]].first] = 0;
        for (auto &a : node_parent)
        {
            cout << "Node: " << a.first << " Parent: " << a.second << endl;
        }
    }
    MPI_Finalize();
    return 0;
}
