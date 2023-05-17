#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

// 4 6 4 15
// 0 1 R
// 2 1 L
// 3 3 U
// 1 0 D

// 10 12 16 100
// 0 1 R
// 2 1 L
// 3 3 U
// 9 9 L
// 6 8 D
// 4 8 L
// 7 3 R
// 5 6 U
// 0 1 U
// 2 1 D
// 3 3 R
// 9 9 U
// 6 8 L
// 4 8 D
// 7 3 U
// 5 6 R

// U, D, L, R
int m1[4] = {0, 0, -1, 1};
int m2[4] = {1, -1, 0, 0};

void print_particles(unordered_map<int, vector<int>>& mp)
{
    for(auto it: mp)
    {
        int x = it.second[0];
        int y = it.second[1];
        int dx = it.second[2];
        int dy = it.second[3];

        if(dx == 0 && dy == 1)
        {
            cout << x << " " << y << " " << 'U' << endl; 
        }
        else if(dx == 0 && dy == -1)
        {
            cout << x << " " << y << " " << 'D' << endl; 
        }
        if(dx == -1 && dy == 0)
        {
            cout << x << " " << y << " " << 'L' << endl; 
        }
        if(dx == 1 && dy == 0)
        {
            cout << x << " " << y << " " << 'R' << endl; 
        }
    }
}

void change_particle_direction(unordered_map<int, vector<int>>& mp, int n, int m, int p1_index, int p2_index)
{
    int p1_x = mp[p1_index][0];
    int p1_y = mp[p1_index][1];
    int p1_dx = mp[p1_index][2];
    int p1_dy = mp[p1_index][3];

    if(mp[p1_index][2] == -1 && mp[p1_index][3] == 0) // 1st particle going left, so due to head on collision, 2nd particle will go right
    {
        if(mp[p1_index][1] == 0 || mp[p1_index][1] == m-1) // if the collision is happening on uppermost or lowermost boundary, no way to go up or down for one particle
        {
            // do nothing
        }
        else
        {
            // left direction changed to up clockwise
            mp[p1_index][2] = 0; 
            mp[p1_index][3] = 1;

            // right direction changed to up clockwise
            mp[p2_index][2] = 0; 
            mp[p2_index][3] = -1;
        }
    }
    else if(mp[p1_index][2] == 1 && mp[p1_index][3] == 0) // 1st particle going right, so due to head on collision, 2nd particle will go left
    {
        if(mp[p1_index][1] == 0 || mp[p1_index][1] == m-1) // if the collision is happening on uppermost or lowermost boundary, no way to go up or down for one particle
        {
            // do nothing
        }
        else
        {
            // right direction changed to down clockwise
            mp[p1_index][2] = 0; 
            mp[p1_index][3] = -1;

            // right direction changed to up clockwise
            mp[p2_index][2] = 0; 
            mp[p2_index][3] = 1;
        }
    }
    else if(mp[p1_index][2] == 0 && mp[p1_index][3] == 1) // 1st particle going up, so due to head on collision, 2nd particle will go down
    {
        if(mp[p1_index][0] == 0 || mp[p1_index][0] == n-1) // if the collision is happening on leftmost or rightmost boundary, no way to go left or right for one particle
        {
            // do nothing
        }
        else
        {
            // up direction changed to right clockwise
            mp[p1_index][2] = 1; 
            mp[p1_index][3] = 0;

            // down direction changed to left clockwise
            mp[p2_index][2] = -1; 
            mp[p2_index][3] = 0;
        }
    }
    else if(mp[p1_index][2] == 0 && mp[p1_index][3] == -1) // 1st particle going down, so due to head on collision, 2nd particle will go up
    {
        if(mp[p1_index][0] == 0 || mp[p1_index][0] == n-1) // if the collision is happening on leftmost or rightmost boundary, no way to go left or right for one particle
        {
            // do nothing
        }
        else
        {
            // down direction changed to left clockwise
            mp[p1_index][2] = -1; 
            mp[p1_index][3] = 0;

            // up direction changed to right clockwise
            mp[p2_index][2] = 1; 
            mp[p2_index][3] = 0;
        }
    }
}

void resolve_collision(map<pair<int, int>, vector<int>>& collision_stat, unordered_map<int, vector<int>>& mp, int n, int m)
{
    map<pair<int, int>, vector<int>> :: iterator it;
    for(it = collision_stat.begin(); it != collision_stat.end(); it++)
    {
        vector<int> particle_vec = it->second;

        if(particle_vec.size() == 2)
        {
            int p1_dx = mp[particle_vec[0]][2];
            int p1_dy = mp[particle_vec[0]][3];

            int p2_dx = mp[particle_vec[1]][2];
            int p2_dy = mp[particle_vec[1]][3];

            if((p1_dx == -p2_dx) && (p1_dy == -p2_dy)) // head on collision
            {
                change_particle_direction(mp, n, m, particle_vec[0], particle_vec[1]);
            }
        }
        else
        {
            // do nothing
        }
    }
}

map<pair<int,int>,vector<int>> update_particles(unordered_map<int,vector<int>>& particle_map, int n, int m, int start, int end, unordered_map<int,vector<int>>& prev, unordered_map<int,vector<int>>& next)
{
    map<pair<int,int>,vector<int>> new_collision_stat;
    unordered_map<int,vector<int>> temp;

    for(auto &it: particle_map) // by reference for permanent changes
    {
        int particle_index = it.first;
        int x = it.second[0];
        int y = it.second[1];
        int dx = it.second[2];
        int dy = it.second[3];

        it.second[0] = it.second[0] + it.second[2]; // x = x + dx
        it.second[1] = it.second[1] + it.second[3]; // y = y + dy

        if(it.second[0] < 0) // reached leftmost boundary, so at time t reverse its direction and set at point 0, and at t+1, move it by dx
        {
            // x = 0;
            // dx = -dx;
            // x = x + dx;
            // it.second[0] = x;
            // it.second[2] = dx;

            it.second[0] = 0;
            it.second[2] = -it.second[2];
            it.second[0] = it.second[0] + it.second[2];
        }
        else if(it.second[0] >= n) // reached rightmost boundary, so at time t reverse its direction and set at point n-1, and at t+1, move it by dx
        {
            // x = n-1;
            // dx = -dx;
            // x = x + dx;
            // it.second[0] = x;
            // it.second[2] = dx;

            it.second[0] = n-1;
            it.second[2] = -it.second[2];
            it.second[0] = it.second[0] + it.second[2];
        }
        else if(it.second[1] < 0) // reached uppermost boundary, so at time t reverse its direction and set at point 0, and at t+1, move it by dy
        {
            // y = 0;
            // dy = -dy;
            // y = y + dy;
            // it.second[1] = y;
            // it.second[3] = dy;

            it.second[1] = 0;
            it.second[3] = -it.second[3];
            it.second[1] = it.second[1] + it.second[3];
        }
        if(it.second[1] >= m) // reached lowermost boundary, so at time t reverse its direction and set at point m-1, and at t+1, move it by dy
        {
            // y = m-1;
            // dy = -dy;
            // y = y + dy;
            // it.second[1] = y;
            // it.second[3] = dy;

            it.second[1] = m-1;
            it.second[3] = -it.second[3];
            it.second[1] = it.second[1] + it.second[3];
        }
        
        if(it.second[1] < start)
        {
            prev[particle_index] = it.second; // if y index is < start, gone from my territory to upper cell
        }
            
        else if(it.second[1] > end)
        {
            next[particle_index] = it.second; // if y index is > end, gone from my territory to lower cell
        }
        else
        {
            new_collision_stat[make_pair(it.second[0], it.second[1])].push_back(particle_index); // rest are collision
            temp[particle_index] = it.second; // updating whichever particles are in my region
        } 
    }
    particle_map = temp;
    return new_collision_stat;
}

void add_val(int temp[], int n,int& index, map<pair<int,int>,vector<int>>& collisions, unordered_map<int,vector<int>>& particles)
{
    for(int l=0;l<4*n;l+=4)
    {
        particles[index] = {temp[l],temp[l+1],temp[l+2],temp[l+3]};
        collisions[{temp[l],temp[l+1]}].push_back(index); // not required though
        index++;
    }
}

void solve(unordered_map<int, vector<int>>& particle_map, int n, int m, int my_rank, int &index, int start, int end, int k, int t)
{
    map<pair<int, int>, vector<int>> collision_stat; // (<x, y>, vector<index number of particles>)
    for(int i=0; i<t && start<m; i+=1)
    {
        resolve_collision(collision_stat, particle_map, n, m);
        int particles_from_prev_process = 0; // particles_from_prev_process = no of particles coming to me from upper
        int particles_from_next_process = 0; // particles_from_next_process = no of particles coming to me from lower
        
        unordered_map<int,vector<int>> prev, next; // prev -> from my territory which particles have gone upper, next -> from my territory which particles have gone lower 

        collision_stat = update_particles(particle_map, n, m, start, end, prev, next);

        int sz = collision_stat.size();

        if(my_rank!=0)// If Previous array exists
        {
            int prev_process_rank = my_rank-1;
            int msg_tag = i;
            MPI_Recv(&particles_from_prev_process, 1, MPI_INT, prev_process_rank, msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout<<"Iteration: "<<i<<" rank:"<<my_rank<<" recieved from:"<<prev_process_rank<<" count is:"<<particles_from_prev_process<<endl;
            if(particles_from_prev_process > 0)
            {
                int* temp_arr = new int[4*particles_from_prev_process];
                MPI_Recv(temp_arr, 4*particles_from_prev_process, MPI_INT, prev_process_rank, msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                add_val(temp_arr, particles_from_prev_process, index,collision_stat, particle_map); // after receiving particles from upper, updating particles config and collision
                delete[] temp_arr;
            }

            int count_particles_for_upper = prev.size();
            MPI_Send(&count_particles_for_upper, 1, MPI_INT, prev_process_rank, msg_tag, MPI_COMM_WORLD); // sending the upper region partciles which have gone from me to upper
            cout<<"Iteration: "<<i<<" rank:"<<my_rank<<" sent to:"<<prev_process_rank<<" count is:"<<count_particles_for_upper<<endl;
            if(count_particles_for_upper > 0)
            {
                int kk = 0;
                int *temp_arr = new int[4*count_particles_for_upper];
                unordered_map<int,vector<int>>:: iterator it;
                for(it=prev.begin(); it!=prev.end(); it++)
                {
                    vector<int> val = it->second;
                    int p_index = it->first;
                    temp_arr[kk] = val[0];
                    temp_arr[kk+1] = val[1];
                    temp_arr[kk+2] = val[2];
                    temp_arr[kk+3] = val[3];
                    kk+=4;
                }
                MPI_Send(temp_arr, 4*count_particles_for_upper, MPI_INT, prev_process_rank, msg_tag, MPI_COMM_WORLD);
                delete[] temp_arr;
            }
        }

        if(end+1<m) // If next process has some rows then talk
        {
            int count_particles_for_lower = next.size();
            int next_process_rank = my_rank+1;
            int msg_tag = i;
            cout<<"Iteration: "<<i<<" rank:"<<my_rank<<" sent to:"<<next_process_rank<<" count is:"<<count_particles_for_lower<<endl;
            MPI_Send(&count_particles_for_lower, 1, MPI_INT, next_process_rank, msg_tag, MPI_COMM_WORLD);
            if(count_particles_for_lower > 0)
            {
                int *temp_arr = new int[4*count_particles_for_lower];
                int kk=0;
                unordered_map<int,vector<int>>:: iterator it;
                for(it=next.begin(); it!=next.end(); it++)
                {
                    vector<int> val = it->second;
                    int p_index = it->first;

                    temp_arr[kk] = val[0];
                    temp_arr[kk+1] = val[1];
                    temp_arr[kk+2] = val[2];
                    temp_arr[kk+3] = val[3];
                    kk+=4;
                }
                MPI_Send(temp_arr, 4*count_particles_for_lower, MPI_INT, next_process_rank, msg_tag, MPI_COMM_WORLD);
                delete[] temp_arr;
            }

            MPI_Recv(&particles_from_next_process, 1, MPI_INT, next_process_rank, msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(particles_from_next_process > 0)
            {
                int* temp_arr = new int[4*particles_from_next_process];
                MPI_Recv(temp_arr, 4*particles_from_next_process, MPI_INT, next_process_rank, msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                add_val(temp_arr, particles_from_prev_process, index,collision_stat, particle_map);
                delete[] temp_arr;
            }
            cout<<"Iteration: "<<i<<" rank:"<<my_rank<<" recieved from:"<<next_process_rank<<" count is:"<<count_particles_for_lower<<endl;
        }
    }

    /* sending update from all process to process zero */
    MPI_Barrier(MPI_COMM_WORLD);
    int count1 = particle_map.size();
    MPI_Send(&count1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // sending all particle details to rank zero
    if(count1 > 0)
    {
        // cout<<rank<<endl;
        int *temp_arr = new int[4*count1];
        int kk=0;
        unordered_map<int,vector<int>>:: iterator it;
        for(it=particle_map.begin(); it!=particle_map.end(); it++)
        {
            vector<int> val = it->second;
            int p_index = it->first;
            temp_arr[kk] = val[0];
            temp_arr[kk+1] = val[1];
            temp_arr[kk+2] = val[2];
            temp_arr[kk+3] = val[3];
            // cout<<temp_arr[kk]<<" "<<temp_arr[kk+1]<<" "<<temp_arr[kk+2]<<" "<<temp_arr[kk+3]<<endl;
            kk+=4;
        }
        MPI_Send(temp_arr, 4*count1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        delete[] temp_arr;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    /* Receiving update from all process in process zero */
    if(my_rank == 0)
    {
        unordered_map<int,vector<int>> new_particle_map; // create new particle map
        int rank_of_curr_process = 0;
        int rem = k;
        while(rem)
        {
            int count;
            MPI_Recv(&count, 1, MPI_INT, rank_of_curr_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receiving all particle details in rank zero
            cout<<"recieved "<<count<<" from rank"<<rank_of_curr_process<<endl;
            if(count > 0)
            {
                int* temp_arr = new int[4*count];
                MPI_Recv(temp_arr, 4*count, MPI_INT, rank_of_curr_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // for(int l=0;l<4*count;l+=4)
                //     cout<<temp_arr[l]<<" "<<temp_arr[l+1]<<" "<<temp_arr[l+2]<<" "<<temp_arr[l+3]<<endl;
                add_val(temp_arr, count, index, collision_stat, new_particle_map);
                delete[] temp_arr;
            }
            rem = rem - count;
            
            rank_of_curr_process++; // rank increment to recienve from process
        }

        // cout<<endl;
        // print_particles(new_particle_map);
    }
}

int main(int argc, char *argv[])
{
    // N , M - the size of the Grid, K the number of particles in the simulation and T the number of time steps.
    int my_rank;
    int size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int n, m, k, t;
    int *arr = NULL;

    if(my_rank == 0)
    {
        cin >> n >> m >> k >> t;
        arr = new int[4*k];

        for(int i=0; i<k; i++)
        {
            int x, y;
            char ch;
            cin >> x >> y >> ch;

            if(ch == 'U')
            {
                arr[4*i] = x;
                arr[4*i+1] = y;
                arr[4*i+2] = 0;
                arr[4*i+3] = 1;
            }
            else if(ch == 'D')
            {
                arr[4*i] = x;
                arr[4*i+1] = y;
                arr[4*i+2] = 0;
                arr[4*i+3] = -1;
            }
            else if(ch == 'L')
            {
                arr[4*i] = x;
                arr[4*i+1] = y;
                arr[4*i+2] = -1;
                arr[4*i+3] = 0;
            }
            else if(ch == 'R')
            {
                arr[4*i] = x;
                arr[4*i+1] = y;
                arr[4*i+2] = 1;
                arr[4*i+3] = 0;
            }
        }

        // for(int i=0; i<k; i++)
        // {
        //     cout << arr[4*i] << " " << arr[4*i+1] << " " <<  arr[4*i+2] << " " << arr[4*i+3] << endl;
        // }

    }
    
    /* broadcasting n,m,k,t, arr to all the processes */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n, 1 ,MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&m, 1 ,MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&k, 1 ,MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&t, 1 ,MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if(arr == NULL)
    {
        arr = new int[4*k];
    }
        
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(arr, 4*k, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // if(my_rank == 0)
    // {
    //     cout << "*************hi************" << endl;
    //     cout << "n: " << n << endl;
    //     cout << "m: " << m << endl;
    //     cout << "k: " << k << endl;
    //     cout << "t: " << t << endl;

    //     for(int i=0; i<k; i++)
    //     {
    //         cout << arr[4*i] << " " << arr[4*i+1] << " " <<  arr[4*i+2] << " " << arr[4*i+3] << endl;
    //     }

    //     // for(int i=0; i<4*k; i=i+4)
    //     // {
    //     //     cout << arr[i] << " " << arr[i+1] << " " <<  arr[i+2] << " " << arr[i+3] << endl;
    //     // }
    // }

    int rows_per_process = ceil(((double)(m))/(double)(size));
    int tmp = ceil(((double)(m))/(double)(k));
    int max_rows_per_process = rows_per_process;

    int estimated_rows_per_process = ((my_rank-1)*rows_per_process);
    if((m - estimated_rows_per_process) <= 0)
    {
        rows_per_process = 0;
    }
    else if((m - estimated_rows_per_process) < rows_per_process)
    {
        rows_per_process =  m - estimated_rows_per_process;
    }
        
    int start = my_rank * max_rows_per_process;
    int end = start + (rows_per_process - 1);

    // cout << "rank: " << my_rank << ", rows_per_pro: " << rows_per_process << ", max_rows_per_pro: " << max_rows_per_process << ", start: " << start << ", end: " << end << endl;


    int index = 0;
    unordered_map<int, vector<int>> particle_map; // (particle no., vector<x, y, dx, dy>)

    for(int i=0; i<4*k; i=i+4)
    {
        if(arr[i+1] >= start && arr[i+1]<= end)
        {
            particle_map[index] = vector<int>({arr[i],arr[i+1],arr[i+2],arr[i+3]});
            index += 1;
        }
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    // if(my_rank == 3)
    // {
    //        cout << "Process rank: " << my_rank << endl;
    //     for(auto it: particle_map)
    //     {
    //         cout << "Particle no. = " << it.first << ", <x, y, dx, dy> = " << it.second[0] << ", " << it.second[1] << ", " << it.second[2] << ", " << it.second[3] << endl;
    //     }
    //     cout << "************Printing complete*************" << my_rank << endl;
    // }
    // MPI_Barrier(MPI_COMM_WORLD);

    delete []arr;

    solve(particle_map, n, m, my_rank, index, start, end, k, t);

    MPI_Finalize();

    return 0;

}