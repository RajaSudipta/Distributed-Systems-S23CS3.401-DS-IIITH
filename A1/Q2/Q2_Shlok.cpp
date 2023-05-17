#include <iostream>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <mpi.h>


using namespace std;

void print(unordered_map<int,vector<int>>& particles, int k);
void take_input(int arr[],int k);
map<pair<int,int>,vector<int>> solve(unordered_map<int,vector<int>>& particles, int n,int m,int start, int end, unordered_map<int,vector<int>>& prev, unordered_map<int,vector<int>>& next);
void add_val(int temp[], int n,int& index, map<pair<int,int>,vector<int>>& collisions,unordered_map<int,vector<int>>& particles);
void cal(unordered_map<int,vector<int>>& arr,map<pair<int,int>,vector<int>>& collisions,int n,int m);
void change_dir(unordered_map<int,vector<int>>& arr, int l1, int l2, int n, int m);

int main(int argc, char** argv){
    int n ,m ,k ,t;
    int rank,size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int par[4];
    int *arr = NULL;
    if(rank == 0)
    {
        cin>>n;
        cin>>m;
        cin>>k;
        cin>>t;

        par[0] = n;
        par[1] = m;
        par[2] = k;
        par[3] = t;

        arr = new int[4*k];
        take_input(arr,k);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(par,4,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    n = par[0];
    m = par[1];
    k = par[2];
    t = par[3];

    int rows_per_pro = ceil(((double)(m))/size);
    int max_rows_per_pro = rows_per_pro;

    if((m - (rank-1)*rows_per_pro) <= 0)
        rows_per_pro = 0;
    else if((m - (rank-1)*rows_per_pro) < rows_per_pro)
        rows_per_pro =  m - (rank-1)*rows_per_pro;

    if(arr == NULL)
        arr = new int[4*k];
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(arr,4*k,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    int start = rank * max_rows_per_pro;
    int end = start + rows_per_pro - 1;

    cout << "rank: " << rank << ", rows_per_pro: " << rows_per_pro << ", max_rows_per_pro: " << max_rows_per_pro << ", start: " << start << ", end: " << end << endl;


    unordered_map<int,vector<int>> particles;
    int index = 0;

    for(int l = 0; l<4*k; l+=4)
        if(arr[l+1] >= start && arr[l+1]<= end) // filling the map with only own region particles
        {
            particles[index] = {arr[l],arr[l+1],arr[l+2],arr[l+3]};
            index++;
        }

    // if(rank == 2)
    // {
    //        cout << "Process rank: " << rank << endl;
    //     for(auto it: particles)
    //     {
    //         cout << "Particle no. = " << it.first << ", <x, y, dx, dy> = " << it.second[0] << ", " << it.second[1] << ", " << it.second[2] << ", " << it.second[3] << endl;
    //     }
    //     cout << "************Printing complete*************" << rank << endl;
    // }
    

    delete[] arr;

    // if(start>end)
    // {
    //     // MPI_Finalize();
    //     return 0;
    // }
    // cout<<"Broadcasted sucessfully"<<endl;
    map<pair<int,int>,vector<int>> collisions; 
    for(int i=0;i<t && start<m;i++) // start < m to stop process to not go beyond it's territory. can apply rows_per_pro != 0
    {
        
            cal(particles,collisions,n,m); // collision detection
            int count_prev = 0, count_next = 0; // count_prev = no of particles coming to me from upper, count_next = no of particles coming to me from lower
            unordered_map<int,vector<int>> prev; // from my territory which particles have gone upper 
            unordered_map<int,vector<int>> next; // from my territory which particles have gone lower 
            collisions = solve(particles, n, m, start, end, prev, next);
            
            // cout<<"Rank: "<<rank<<"For "<<i<<" th iteration:"<<endl;
            // print(particles,k);

            if(rank!=0)// If Previous array exists
            {
                MPI_Recv(&count_prev, 1, MPI_INT, rank-1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cout<<"Iteration: "<<i<<" rank:"<<rank<<" recieved from:"<<rank-1<<" count is:"<<count_prev<<endl;
                if(count_prev)
                {
                    int* temp = new int[4*count_prev];
                    MPI_Recv(temp, 4*count_prev, MPI_INT, rank-1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    add_val(temp,count_prev,index,collisions,particles); // after receiving particles from upper, updating particles config and collision
                    delete[] temp;
                }
                int count = prev.size();
                MPI_Send(&count, 1, MPI_INT, rank-1, i, MPI_COMM_WORLD); // sending the upper region partciles which have gone from me to upper
                cout<<"Iteration: "<<i<<" rank:"<<rank<<" sent to:"<<rank-1<<" count is:"<<count<<endl;
                if(count)
                {
                    int *temp = new int[4*count];
                    int j=0;
                    for(auto t1: prev)
                    {
                        vector<int> val = t1.second;
                        temp[j] = val[0];
                        temp[j+1] = val[1];
                        temp[j+2] = val[2];
                        temp[j+3] = val[3];
                        j+=4;
                    }
                    MPI_Send(temp, 4*count, MPI_INT, rank-1, i, MPI_COMM_WORLD);
                    delete[] temp;
                }
            }

            if(end+1<m) // If next process has some rows then talk
            {
                int count = next.size();
                cout<<"Iteration: "<<i<<" rank:"<<rank<<" sent to:"<<rank+1<<" count is:"<<count<<endl;
                MPI_Send(&count, 1, MPI_INT, rank+1, i, MPI_COMM_WORLD);
                if(count)
                {
                    int *temp = new int[4*count];
                    int j=0;
                    for(auto t1: next)
                    {
                        vector<int> val = t1.second;
                        temp[j] = val[0];
                        temp[j+1] = val[1];
                        temp[j+2] = val[2];
                        temp[j+3] = val[3];
                        j+=4;
                    }
                    MPI_Send(temp, 4*count, MPI_INT, rank+1, i, MPI_COMM_WORLD);
                    delete[] temp;
                }

                MPI_Recv(&count_next, 1, MPI_INT, rank+1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(count_next)
                {
                    int* temp = new int[4*count_next];
                    MPI_Recv(temp, 4*count_next, MPI_INT, rank+1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    add_val(temp,count_next,index,collisions,particles);
                    delete[] temp;
                }
                cout<<"Iteration: "<<i<<" rank:"<<rank<<" recieved from:"<<rank+1<<" count is:"<<count_next<<endl;

            }
        

        // MPI_Barrier(MPI_COMM_WORLD);
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    // cout<<"All iterations complete"<<endl;
    MPI_Barrier(MPI_COMM_WORLD);
    // if(rank!=0)
    // {
        int count1 = particles.size();
        MPI_Send(&count1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // sending all particle details to rank zero
        if(count1)
        {
            // cout<<rank<<endl;
            int *temp = new int[4*count1];
            int j=0;
            for(auto t1: particles)
            {
                vector<int> val = t1.second;
                temp[j] = val[0];
                temp[j+1] = val[1];
                temp[j+2] = val[2];
                temp[j+3] = val[3];
                // cout<<temp[j]<<" "<<temp[j+1]<<" "<<temp[j+2]<<" "<<temp[j+3]<<endl;
                j+=4;
            }
            MPI_Send(temp, 4*count1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            delete[] temp;
        }
    // }
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0)
    {
        unordered_map<int,vector<int>> particle; // create new particle map
        int r = 0;
        int rem = k;
        while(rem)
        {
            int count;
            MPI_Recv(&count, 1, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receiving all particle details in rank zero
            cout<<"recieved "<<count<<" from rank"<<r<<endl;
            if(count)
            {
                int* temp = new int[4*count];
                MPI_Recv(temp, 4*count, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // for(int l=0;l<4*count;l+=4)
                //     cout<<temp[l]<<" "<<temp[l+1]<<" "<<temp[l+2]<<" "<<temp[l+3]<<endl;
                add_val(temp,count,index,collisions,particle);
                delete[] temp;
            }
            rem -= count;
            
            r++; // rank increment to recienve from process
        }

        cout<<endl;
        print(particle,k);
    }
    
    MPI_Finalize();
}

void print(unordered_map<int,vector<int>>& particles, int k)
{
    set<pair<pair<pair<int,int>,int>,char>> m;
    for(auto i:particles)
    {
        vector<int> arr = i.second;
        int l = 0;
        char ch;
        pair<int,int> p = {arr[l],arr[l+1]};
        if(arr[l+2] == 0 && arr[l+3] == -1)
            ch = 'D';
        else if(arr[l+2] == 0 && arr[l+3] == 1)
            ch = 'U';
        else if(arr[l+2] == -1 && arr[l+3] == 0)
            ch = 'L';
        else
            ch = 'R';
        m.insert({{p,i.first},ch});
        // cout<<i.first<<" "<<p.first<<" "<<p.second<<" "<<ch<<endl;
    } 

    for(auto i:m)
    {
        cout<<i.first.first.first<<" "<<i.first.first.second<<" "<<i.second<<endl;
    }  
}

map<pair<int,int>,vector<int>> solve(unordered_map<int,vector<int>>& particles, int n,int m,int start, int end, unordered_map<int,vector<int>>& prev, unordered_map<int,vector<int>>& next)
{
    map<pair<int,int>,vector<int>> collisions;
    unordered_map<int,vector<int>> temp;
    for(auto i:particles)
    {

        int index = i.first;
        vector<int> arr = i.second;
        int l  = 0;

        arr[l] += arr[l+2];
        arr[l+1] += arr[l+3];
        if( arr[l] < 0 )
        {
            arr[l] = 0;
            arr[l+2] = -arr[l+2];
            arr[l] += arr[l+2];
        }
        else if(arr[l]>=n)
        {
            arr[l] = n-1;
            arr[l+2] = -arr[l+2];
            arr[l] += arr[l+2];
        }
        if(arr[l+1]<0)
        {
            arr[l+1] = 0;
            arr[l+3] = -arr[l+3];
            arr[l+1] += arr[l+3];
        }
        else if(arr[l+1]>=m)
        {
            arr[l+1] = m-1;
            arr[l+3] = -arr[l+3];
            arr[l+1] += arr[l+3];
        }

        if(arr[l+1] < start)
            prev[index] = arr; // if y index is < start, gone from my territory to upper cell
        else if(arr[l+1] > end)
            next[index] = arr; // if y index is > end, gone from my territory to lower cell
        else
        {
            collisions[{arr[l],arr[l+1]}].push_back(index); // rest are collision
            temp[index] = arr; // updating whichever particles are in my region
        } 
        // temp[index] = arr;
    }
    particles = temp; // updating whichever particles are in my region
    // for(auto i:prev)
    //     particles.erase(i.first);
    
    // for(auto i:next)
    //     particles.erase(i.first);
    return collisions;
}

void add_val(int temp[], int n,int& index, map<pair<int,int>,vector<int>>& collisions,unordered_map<int,vector<int>>& particles)
{
    for(int l=0;l<4*n;l+=4)
    {
        particles[index] = {temp[l],temp[l+1],temp[l+2],temp[l+3]};
        collisions[{temp[l],temp[l+1]}].push_back(index); // not required though
        index++;
    }
}

void cal(unordered_map<int,vector<int>>& arr,map<pair<int,int>,vector<int>>& collisions,int n,int m)
{
    for(auto i:collisions)
    {
        pair<int,int> key = i.first;
        vector<int> val = i.second;
        if(val.size()<=1 || val.size()>2)
            continue;
        if((arr[val[0]][2] + arr[val[1]][2] == 0) && (arr[val[0]][3] + arr[val[1]][3] == 0)) //collison occured
        {

            // cout<<"conflict occured for:"<<val[0]<<" "<<val[1]<<endl;
            int l  =val[0];
            // cout<<arr[l][0]<<" "<<arr[l][1]<<" "<<arr[l][2]<<" "<<arr[l][3]<<endl;
            int l1 = val[1];
            // cout<<arr[l1][0]<<" "<<arr[l1][1]<<" "<<arr[l1][2]<<" "<<arr[l1][3]<<endl;
            change_dir(arr,val[0],val[1],n,m);
        }
    }
}

void change_dir(unordered_map<int,vector<int>>& arr, int l1, int l2, int n, int m)
{
    if(arr[l1][2] == -1 && arr[l1][3] == 0) // Going left
    {
        if(arr[l1][1] == 0 || arr[l1][1] == m-1)
            return;
        arr[l1][2] = 0;
        arr[l1][3] = 1;

        arr[l2][2] = 0;
        arr[l2][3] = -1;
    }
    else if(arr[l1][2] == 1 && arr[l1][3] == 0) // Going right
    {
        if(arr[l1][1] == 0 || arr[l1][1] == m-1)
            return;
        arr[l1][2] = 0;
        arr[l1][3] = -1;

        arr[l2][2] = 0;
        arr[l2][3] = 1;
    }
    else if(arr[l1][2] == 0 && arr[l1][3] == 1) // Going Up
    {
        if(arr[l1][0] == 0 || arr[l1][0] == n-1)
            return;
        arr[l1][2] = 1;
        arr[l1][3] = 0;

        arr[l2][2] = -1;
        arr[l2][3] = 0;
    }
    else //Going down
    {
        if(arr[l1][0] == 0 || arr[l1][0] == n-1)
            return;
        arr[l1][2] = -1;
        arr[l1][3] = 0;
        arr[l2][2] = 1;
        arr[l2][3] = 0;
    }
}

void take_input(int arr[], int k)
{
    for(int l=0; l < 4*k; l+=4)
    {
        int i,j,dx,dy;
        char ch;
        cin>>i;
        cin>>j;
        cin>>ch;
        if(ch == 'D')
        {
            dx = 0;
            dy = -1;
        }
        else if(ch == 'U')
        {
            dx = 0;
            dy = 1;
        }
        else if(ch == 'L')
        {
            dx  = -1;
            dy = 0;
        }
        else
        {
            dx = 1;
            dy = 0;
        }
        arr[l] = i;
        arr[l+1] = j;
        arr[l+2] = dx;
        arr[l+3] = dy;
    }
}