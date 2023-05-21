#include<bits/stdc++.h>
#include<mpi.h>
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


int sum(int freq_arr[], int i, int j)
{
    int s = 0;
    for (int k = i; k <= j; k++)
    {
        s += freq_arr[k];
    }
    return s;
}

int optimal_search_tree(int *key_arr, int *freq_arr, int n, int my_rank, int cell_size_per_process, int size, vector<vector<int>> &root)
{
    int cost[n][n];

    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            cost[i][j] = 0;
        }
    }

    /* 1st diagonal */
    for(int i=0; i<n; i++)
    {
        cost[i][i] = freq_arr[i];
    }

    for(int length=2; length<=n; length++)
    {
        int size_of_diagonal = n-length+1;
        // int part_width = ceil((double)size_of_diagonal/(double)size);
        // int effective_no_of_processes = ceil((double)size_of_diagonal/(double)length);
        int effective_no_of_processes = min(size_of_diagonal, size);
        int part_width = ceil((double)size_of_diagonal/(double)effective_no_of_processes);
        int *small_arr = new int[part_width];
        int *big_arr = new int[part_width * size];

        int *small_root_arr = new int[part_width];
        int *big_root_arr = new int[part_width * size];
        int kk = 0;

        // if(my_rank == 0)
        // {
        //     cout << "chain length = " << length << endl;
        // }

        for(int i=my_rank*part_width; i<min(((my_rank+1)*part_width), size_of_diagonal); i++)
        {
            int j = i+length-1;
            // if(my_rank == 0)
            // {
            //     cout << "i: " << i << ", j: " << j << endl;
            // }
            // cost[i][j] = INT_MAX;
            int minm = INT_MAX;
            int rt = -1;
            int offset_sum = sum(freq_arr, i, j);
            // cout << "offset_sum = " << offset_sum << endl;
            for(int r=i; r<=j; r++)
            {
                // if(my_rank == 0)
                // {
                //     cout << "cost[" << i << "][" << r-1 << "] = " << cost[i][r-1] << endl;
                //     cout << "cost[" << r+1 << "][" << j << "] = " << cost[r+1][j] << endl;
                // }
                int c = ((r>i)? cost[i][r-1]:0) + ((r<j)? cost[r+1][j]:0) + offset_sum;
                // if(c < cost[i][j])
                // {
                if(c < minm)
                {
                    minm = c;
                    rt = r;
                }
            }
            small_arr[kk] = minm;
            small_root_arr[kk] = rt;
            // if(my_rank == 0)
            // {
            //     cout << "small_arr[" << kk << "] = " << small_arr[kk] << endl;
            // }
            kk++;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(small_arr, part_width, MPI_INT, big_arr, part_width, MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(small_root_arr, part_width, MPI_INT, big_root_arr, part_width, MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        if(my_rank == 0)
        {
            cout << "********** Printing the diagonal of length = " << size_of_diagonal << " **********" << endl;
            for(int i=0; i<size_of_diagonal; i++)
            {
                cout << big_arr[i] << " ";
            }
            cout << endl << "********** END of printing **********" << endl << endl;
        }

        int k = 0;
        for(int i=0; i<n-length+1; i++)
        {
            int j = i+length-1;
            cost[i][j] = big_arr[k];
            root[i][j] = big_root_arr[k];
            k++;
            // cout << "cost[" << i << "][" << j << "] = " << cost[i][j] << " ";
        }
        cout << endl;

        if(my_rank == 0)
        {
            cout << "************* Printing the cost array *************" << endl;
            for(int i=0; i<n; i++)
            {
                for(int j=0; j<i; j++)
                {
                    cout << "XX" << " ";
                }
                for(int j=i; j<n; j++)
                {
                    cout << cost[i][j] << " ";
                }
                cout << endl;
            }
            cout << "************* END of printing *************" << endl << endl;
        }
    }
    return cost[0][n-1];
}

void fill_parent(vector<vector<int>> &root, map<int, int> &node_parent, int *sorted_key_arr, int i, int j, int arr_size, int parent)
{
    if (i == j)
    {
        node_parent[sorted_key_arr[i]] = parent;
        // cout << "node: " << sorted_key_arr[i] << ", parent: " << parent << endl;
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
        // cout << "node: " << sorted_key_arr[r] << ", parent: " << parent << endl;
        fill_parent(root, node_parent, sorted_key_arr, i, r - 1, arr_size, sorted_key_arr[r]);
        fill_parent(root, node_parent, sorted_key_arr, r + 1, j, arr_size, sorted_key_arr[r]);
    }
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
            // cout<<"*******************"<<endl;
            // cout<<"key:"<<pq[i].top().first<<endl;
            // cout<<"freq:"<<pq[i].top().second.first<<endl;
            // cout<<"heap num:"<<pq[i].top().second.second<<endl;
            // cout<<"*******************"<<endl;
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
}

void display_vector(vector<pair<int,int>> &temp)
{
    for(auto &a:temp)
    {
        cout<<"{"<<a.first<<" "<<a.second<<"} ";
    }
    cout<<endl;
}

void run_helper(int n, int size, int my_rank)
{
    int cell_size_per_process = ceil((double)(n) / (double)(size));
    int bigger_num_of_elements = size * cell_size_per_process;
    int bigger_arr_size = 2 * bigger_num_of_elements;
    // int *key_freq_arr = new int[bigger_arr_size];
    int *key_freq_arr;
    // memset(key_freq_arr, INT_MAX, bigger_arr_size);
    int partial_arr_size = 2 * cell_size_per_process;
    int *partial_res = new int[partial_arr_size];

    if(my_rank == 0)
    {
        key_freq_arr = new int[bigger_arr_size];
        memset(key_freq_arr, INT_MAX, bigger_arr_size);
        for(int i=0; i<bigger_arr_size; i++)
        {
            key_freq_arr[i] = INT_MAX;
        }
        // for(int i=0; i<bigger_num_of_elements; i++)
        // {
        //     cout << key_freq_arr[2*i] << " -> " << key_freq_arr[2*i+1];
        //     cout << endl;
        // }
        for(int i=0; i<n; i++)
        {
            int k, f;
            cin >> k;
            cin >> f;
            key_freq_arr[2*i] = k;
            key_freq_arr[2*i+1] = f;
        }

        for(int i=0; i<n; i++)
        {
            cout << key_freq_arr[2*i] << " -> " << key_freq_arr[2*i+1];
            cout << endl;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(key_freq_arr, partial_arr_size, MPI_INT, partial_res, partial_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // cout << "Process " << my_rank << endl;
    // for(int i=0; i<cell_size_per_process; i++)
    // {
    //     cout << partial_res[2*i] << " -> " << partial_res[2*i+1] << endl;
    // }

    vector<pair<int, int>> vec; // (key, freq)
    for(int i=0; i<cell_size_per_process; i++)
    {
        vec.push_back({ partial_res[2*i], partial_res[2*i+1] });
    }

    // cout << "Before sort" << endl;
    // for(auto it: vec)
    // {
    //     cout << it.first << " -> " << it.second << endl;
    // }

    sort(vec.begin(), vec.end());

    // cout << "After sort" << endl;
    // for(auto it: vec)
    // {
    //     cout << it.first << " -> " << it.second << endl;
    // }

    for (int i=0; i<vec.size(); i++)
    {
        partial_res[2*i] = vec[i].first;
        partial_res[2*i+1] = vec[i].second;
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
        for (int i=0; i<n; i++)
        {
            key_freq_vec.push_back(make_pair(key_freq_arr[2*i], key_freq_arr[2*i+1]));
        }

        key_freq_vec = merge_subarray(key_freq_vec, cell_size_per_process, size);
        // cout<<"Merged After sorting by: "<<my_rank<<endl;
        // display_vector(key_freq_vec);

        for(int i=0; i<n; i++)
        {
            sorted_key_arr[i] = key_freq_vec[i].first;
            sorted_freq_arr[i] = key_freq_vec[i].second;
        }
    }

    MPI_Bcast(sorted_key_arr, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(sorted_freq_arr, n, MPI_INT, 0, MPI_COMM_WORLD);

    // cout << "Process = " << my_rank << endl;
    // cout << "Sorted Keys" << endl;
    // for(int i=0; i<n; i++)
    // {
    //     cout << sorted_key_arr[i] << " ";
    // }
    // cout << endl;

    // cout << "Sorted frequenecy" << endl;
    // for(int i=0; i<n; i++)
    // {
    //     cout << sorted_freq_arr[i] << " ";
    // }
    // cout << endl;

    vector<vector<int>> root(n, vector<int>(n, -1));
    int val = optimal_search_tree(sorted_key_arr, sorted_freq_arr, n, my_rank, cell_size_per_process, size, root);

    if(my_rank == 0)
    {
        // cout << "Printing the root 2d array" << endl;
        // for(int i=0; i<n; i++)
        // {
        //     for(int j=0; j<n; j++)
        //     {
        //         cout << root[i][j] << " ";
        //     }
        //     cout << endl;
        // }

        map<int, int> node_parent;
        fill_parent(root, node_parent, sorted_key_arr, 0, n-1, n, 0);
        node_parent[sorted_key_arr[root[0][n - 1]]] = 0;

        cout << "Optimal cost = " << val << endl;

        for (auto &a : node_parent)
        {
            cout << "Node: " << a.first << " Parent: " << a.second << endl;
        }

        cout << endl << val << endl;

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