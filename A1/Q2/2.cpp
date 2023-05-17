#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;
typedef vector<int> VI;

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

map<int, int> m_reverse_direction = {{'R', 'L'}, {'L', 'R'}, {'U', 'D'}, {'D', 'U'}};
int M, N, T, K, portion_length;

struct Particle
{
    int id, x, y, direction;
    bool dir_change;
    static map<int, VI> m_move;
    static map<int, int> m_change_direction;

    Particle()
    {
    }
    Particle(int param1, int param2, int param3, int param4)
    {
        y = param3;
        id = param1;
        direction = param4;
        x = param2;
    }
    VI serialize()
    {
        return VI{id, x, y, direction};
    }
    void move_ahead()
    {
        this->y += m_move[this->direction][0];
        this->x += m_move[this->direction][1];
        dir_change = true;
        if (this->y >= M)
            this->y = M - 2;
        else if (this->y < 0)
            this->y = 1;
        else if (this->x >= N)
            this->x = N - 2;
        else if (this->x < 0)
            this->x = 1;
        else
            dir_change = false;
        if (dir_change)
            this->direction = m_reverse_direction[this->direction];
    }
    void collide()
    {
        this->direction = m_change_direction[this->direction];
    }
};

map<int, int> Particle::m_change_direction = {{'R', 'D'}, {'L', 'U'}, {'U', 'R'}, {'D', 'L'}};
map<int, VI> Particle::m_move = {{'L', {-1, 0}}, {'D', {0, -1}}, {'R', {1, 0}}, {'U', {0, 1}}};
int lowerlimit;
int upperlimit;
vector<Particle> list_crossed_upperlimit, list_crossed_lowerlimit;

void shift_particles_ahead(map<int, Particle> &my_particles)
{
    VI crossed_particles;

    for (auto &itr : my_particles)
    {
        Particle &p = itr.second;
        int flag = 1;
        p.move_ahead();
        // if changed process boundary
        if (p.x < lowerlimit)
        {
            list_crossed_lowerlimit.push_back(p);
        }
        else if (p.x >= upperlimit)
        {
            list_crossed_upperlimit.push_back(p);
        }
        else
        {
            flag = 0;
        }
        if (flag)
            crossed_particles.push_back(itr.first);
    }
    for (auto i : crossed_particles)
    {
        my_particles.erase(i);
    }
}


int main(int argc, char *argv[])
{
    int size;
    int my_rank;
    int comm_size;
    int name_length;
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    char name[MPI_MAX_OBJECT_NAME];
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_get_name(comm, name, &name_length);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(comm, &comm_size);

    map<int, Particle> my_particles; // particle_id:: vector: particle_id(again!),x,y,(char)dir
    int process_offset;
    vector<int> v(5);
    int x;
    char z;
    int y;

    if (my_rank == 0)
    {
        cin >> v[0] >> v[1] >> v[2] >> v[3];
        v[4] = ceil((double)v[1] / size);
    }
    MPI_Bcast(v.data(), 5, MPI_INT, 0, MPI_COMM_WORLD);
    // M = v[0];
    // N = v[1];
    // K = v[2];
    // T = v[3];
    // portion_length = v[4];
    M = v.at(0);
    portion_length = v.at(4);
    K = v.at(2);
    N = v.at(1);
    T= v.at(3);

    if (my_rank == 0)
    {
        map<int, vector<Particle>> process_particle_map;
        int particle_id = 0;

        for (int id = 0; id < v[2]; id++)
        {
            cin >> x >> y >> z;
            process_particle_map[y / portion_length].push_back(Particle(id, y, x, (int)z));
            particle_id++;
        }
        for (int my_rank = 0; my_rank < size; my_rank++)
        {
            int size = process_particle_map[my_rank].size();
            MPI_Send(&size, 1, MPI_INT, my_rank, 0, MPI_COMM_WORLD);
            for (int it = size; it > 0; it--)
            {
                Particle &p = process_particle_map[my_rank][it - 1];
                MPI_Send(p.serialize().data(), 4, MPI_INT, my_rank, 0, MPI_COMM_WORLD);
            }
        }
    }
    int size_r;
    MPI_Recv(&size_r, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int temp[4];
    for (int x = size_r; x > 0; x--)
    {
        MPI_Recv(temp, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        my_particles.insert({temp[0], Particle(temp[0], temp[1], temp[2], temp[3])});
    }

    // setting my offset, i.e. the range in with a particular process will handle the particle data.
    process_offset = my_rank * portion_length;
    lowerlimit = process_offset;
    portion_length = min(portion_length, N - lowerlimit);
    upperlimit = lowerlimit + portion_length;

    for (int x = 0; x < T; x++)
    {
        list_crossed_lowerlimit.clear();
        list_crossed_upperlimit.clear();
        shift_particles_ahead(my_particles);

        if (my_rank != size - 1)
        {
            size_r = list_crossed_upperlimit.size();
            MPI_Send(&size_r, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
            for (int x = size_r; x > 0; x--)
            {
                Particle &p = list_crossed_upperlimit[x - 1];
                MPI_Send(p.serialize().data(), 4, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
            }
        }
        if (my_rank != 0)
        {
            size_r = list_crossed_lowerlimit.size();
            MPI_Send(&size_r, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
            for (int x = size_r; x > 0; x--)
            {
                Particle &p = list_crossed_lowerlimit[x - 1];
                MPI_Send(p.serialize().data(), 4, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
            }
        }

        if (my_rank != size - 1)
        {
            MPI_Recv(&size_r, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            for (int x = size_r; x > 0; x--)
            {
                MPI_Recv(temp, 4, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                my_particles.insert({temp[0], Particle(temp[0], temp[1], temp[2], temp[3])});
            }
        }
        if (my_rank != 0)
        {
            MPI_Recv(&size_r, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            for (int x = size_r; x > 0; x--)
            {
                MPI_Recv(temp, 4, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                my_particles.insert({temp[0], Particle(temp[0], temp[1], temp[2], temp[3])});
            }
        }

        // make them collide
        map<VI, VI> m;
        for (auto itr : my_particles)
        {
            Particle &p = itr.second;
            VI coordinates{p.x, p.y};
            m[coordinates].push_back(p.id);
        }
        for (auto &[i, j] : m)
        {
            if (j.size() == 2)
            {
                Particle &p1 = my_particles[j[0]];
                Particle &p2 = my_particles[j[1]];
                if (p1.y != 0 && p1.y != M - 1 && p1.x != 0 && p1.x != N - 1 && m_reverse_direction[p1.direction] == p2.direction)
                {
                    p1.collide();
                    p2.collide();
                }
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    // print_particles(my_rank,my_particles);
    vector<Particle> part;
    for (auto i : my_particles)
    {
        part.push_back(i.second);
    }

    size_r = part.size();
    MPI_Send(&size_r, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    for (int x = size_r; x > 0; x--)
    {
        Particle &p = part[x - 1];
        MPI_Send(p.serialize().data(), 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (my_rank == 0)
    {
        set<VI> ans;
        for (int my_rank = 0; my_rank < size; my_rank++)
        {
            MPI_Recv(&size_r, 1, MPI_INT, my_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            for (int x = size_r; x > 0; x--)
            {
                MPI_Recv(temp, 4, MPI_INT, my_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                ans.insert({temp[2], temp[1], temp[3]});
            }
        }

        // cout << endl;
        for (auto i : ans)
        {
            printf("%d %d %c\n", i[0], i[1], i[2]);
        }
    }

    MPI_Finalize();
    return 0;
}