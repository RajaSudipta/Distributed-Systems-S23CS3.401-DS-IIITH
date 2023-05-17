#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <mpi.h>
using namespace std;

struct Particle
{
    int id, x, y, direction;
    Particle()
    {
    }
    Particle(int id, int x, int y, int direction)
    {
        this->id = id;
        this->x = x;
        this->y = y;
        this->direction = direction;
    }
    int *serialize()
    {
        int *a = new int[4];
        a[0] = id;
        a[1] = x;
        a[2] = y;
        a[3] = direction;
        return a;
    }
};

int M, N, T, K;

int lowerlimit;
int upperlimit;
vector<Particle> list_crossed_upperlimit, list_crossed_lowerlimit;
map<int, vector<int>> m_move = {{'R', {1, 0}}, {'L', {-1, 0}}, {'U', {0, 1}}, {'D', {0, -1}}};
map<int, int> m_change_direction = {{'R', 'D'}, {'L', 'U'}, {'U', 'R'}, {'D', 'L'}};
map<int, int> m_reverse_direction = {{'R', 'L'}, {'L', 'R'}, {'U', 'D'}, {'D', 'U'}};

void move_particles_ahead(map<int, Particle> &my_particles)
{
    vector<int> crossed_particles;

    for (auto &itr : my_particles)
    {
        Particle &p = itr.second;
        int flag = 0;
        p.y += m_move[p.direction][0];
        p.x += m_move[p.direction][1];

        // Handling Exceeding boundaries
        int x1 = p.y, y1 = p.x;

        if (p.y < 0)
        {
            p.y = 1;
        }
        else if (p.y >= M)
        {
            p.y = M - 2;
        }

        if (p.x < 0)
        {
            p.x = 1;
        }
        else if (p.x >= N)
        {
            p.x = N - 2;
        }

        // Changing Direction if collided with boundary
        if (p.y != x1 || p.x != y1)
            p.direction = m_reverse_direction[p.direction];

        // if changed process boundary
        if (p.x < lowerlimit)
        {
            flag = 1;
            list_crossed_lowerlimit.push_back(p);
        }
        else if (p.x >= upperlimit)
        {
            flag = 1;
            list_crossed_upperlimit.push_back(p);
        }
        if (flag)
            crossed_particles.push_back(itr.first);
    }
    for (auto i : crossed_particles)
    {
        my_particles.erase(i);
    }
}

bool check_for_collision(Particle &p1, Particle &p2)
{
    int x = p1.y;
    int y = p1.x;

    if (x != 0 && x != M - 1 && y != 0 && y != N - 1 && m_reverse_direction[p1.direction] == p2.direction)
    {
        return true;
    }
    return false;
}
void display_function(vector<vector<int>>&ans)
{
    for (auto i : ans)
    {
        cout << i[0] << " " << i[1] << " " << (char)i[2] << endl;
    }
}

int main(int argc, char *argv[])
{
    int rank, size, root = 0;
    int comm_size;
    int name_length;
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    char name[MPI_MAX_OBJECT_NAME];
    MPI_Comm_get_name(comm, name, &name_length);
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    map<int, Particle> my_particles; // particle_id:: vector: particle_id(again!),x,y,(char)dir
    int portion_length, process_offset;
    int input_arguments[5];
    int lower_process = rank - 1;
    int upper_process = rank + 1;

    if (rank == root)
    {
        // cin>>input_arguments[0]>>input_arguments[1]>>input_arguments[2]>>input_arguments[3];
        cin >> input_arguments[0];
        cin >> input_arguments[1];
        cin >> input_arguments[2];
        cin >> input_arguments[3];
        portion_length = ceil((double)input_arguments[1] / size);
        input_arguments[4] = portion_length;

        map<int, vector<Particle>> process_particle_map;
        char z;
        int x, y;

        int particle_id = 0;
        while (particle_id < input_arguments[2])
        {
            cin >> x >> y >> z;
            process_particle_map[y / portion_length].push_back(Particle(particle_id, y, x, (int)z));
            particle_id++;
        }
        int process_rank = 0;
        while (process_rank < size)
        {
            int size = process_particle_map[process_rank].size();
            MPI_Send(&size, 1, MPI_INT, process_rank, 0, MPI_COMM_WORLD);
            while (size)
            {
                Particle &p = process_particle_map[process_rank][size - 1];
                int *v = p.serialize();
                MPI_Send(v, 4, MPI_INT, process_rank, 0, MPI_COMM_WORLD);
                delete v;
                size--;
            }
            process_rank++;
        }
    }

    int size_r;
    MPI_Recv(&size_r, 1, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int temp[4];
    while (size_r)
    {
        MPI_Recv(temp, 4, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        my_particles.insert({temp[0], Particle(temp[0], temp[1], temp[2], temp[3])});
        size_r--;
    }
    MPI_Bcast(input_arguments, 5, MPI_INT, root, MPI_COMM_WORLD);
    portion_length = *(input_arguments + 4);
    N = *(input_arguments + 1);
    M = *(input_arguments + 0);
    T = *(input_arguments + 3);
    K = *(input_arguments + 2);

    // setting my offset, i.e. the range in with a particular process will handle the particle data.
    process_offset = rank * portion_length;
    portion_length = min(portion_length, N - process_offset);
    lowerlimit = process_offset;
    upperlimit = lowerlimit + portion_length;

    while (T)
    {
        move_particles_ahead(my_particles);

        if (rank != size - 1)
        {
            int size = list_crossed_upperlimit.size();
            MPI_Send(&size, 1, MPI_INT, upper_process, 0, MPI_COMM_WORLD);
            while (size)
            {
                Particle &p = list_crossed_upperlimit[size - 1];
                int *v = p.serialize();
                MPI_Send(v, 4, MPI_INT, upper_process, 0, MPI_COMM_WORLD);
                delete v;
                size--;
            }
            list_crossed_upperlimit.clear();
        }
        if (rank != 0)
        {
            int size = list_crossed_lowerlimit.size();
            MPI_Send(&size, 1, MPI_INT, lower_process, 0, MPI_COMM_WORLD);
            while (size)
            {
                Particle &p = list_crossed_lowerlimit[size - 1];
                int *v = p.serialize();
                MPI_Send(v, 4, MPI_INT, lower_process, 0, MPI_COMM_WORLD);
                delete v;
                size--;
            }
            list_crossed_lowerlimit.clear();
        }

        if (rank != size - 1)
        {
            int size;
            MPI_Recv(&size, 1, MPI_INT, upper_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            while (size)
            {
                MPI_Recv(temp, 4, MPI_INT, upper_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                my_particles.insert({temp[0], Particle(temp[0], temp[1], temp[2], temp[3])});
                size--;
            }
        }
        if (rank != 0)
        {
            int size;
            MPI_Recv(&size, 1, MPI_INT, lower_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            while (size)
            {
                MPI_Recv(temp, 4, MPI_INT, lower_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                my_particles.insert({temp[0], Particle(temp[0], temp[1], temp[2], temp[3])});
                size--;
            }
        }

        // make them collide
        map<vector<int>, vector<int>> m;
        for (auto itr : my_particles)
        {
            Particle &p = itr.second;
            m[{p.x, p.y}].push_back(p.id);
        }
        for (auto i : m)
        {
            if (i.second.size() == 2)
            {
                // int p1 = i.second[0];
                // int p2 = i.second[1];
                if (check_for_collision(my_particles[i.second[0]], my_particles[i.second[1]]))
                {
                    // int &dir1 = my_particles[p1].direction;
                    // int &dir2 = my_particles[p2].direction;
                    int &dir1 = my_particles[i.second[0]].direction;
                    int &dir2 = my_particles[i.second[1]].direction;
                    dir1 = m_change_direction[dir1];
                    dir2 = m_change_direction[dir2];
                }
            }
        }

        T--;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // print_particles(rank,my_particles);
    vector<Particle> part;
    for (auto i : my_particles)
    {
        part.push_back(i.second);
    }

    int s = part.size();
    MPI_Send(&s, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
    while (s)
    {
        Particle &p = part[s - 1];
        int *v = p.serialize();
        MPI_Send(v, 4, MPI_INT, root, 0, MPI_COMM_WORLD);
        delete v;
        s--;
    }

    if (rank == root)
    {
        vector<Particle> m_temp_list_particles;
        int temp[4];
        int process_rank = 0;
        while (process_rank < size)
        {
            size_r = 0;
            MPI_Recv(&size_r, 1, MPI_INT, process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            while (size_r)
            {
                MPI_Recv(temp, 4, MPI_INT, process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                m_temp_list_particles.push_back(Particle(temp[0], temp[1], temp[2], temp[3]));
                size_r--;
            }
            process_rank++;
        }
        vector<vector<int>> ans;
        for (auto i : m_temp_list_particles)
        {
            ans.push_back({i.y, i.x, i.direction});
        }
        sort(ans.begin(), ans.end());
        display_function(ans);
        // for (auto i : ans)
        // {
        //     cout << i[0] << " " << i[1] << " " << (char)i[2] << endl;
        // }
    }

    MPI_Finalize();
    return 0;
}