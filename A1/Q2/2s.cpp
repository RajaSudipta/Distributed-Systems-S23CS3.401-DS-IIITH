#include<bits/stdc++.h>
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

void print_particles(map<int, vector<int>>& mp)
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

void change_particle_direction(map<int, vector<int>>& mp, int n, int m, int p1_index, int p2_index)
{
    int p1_x = mp[p1_index][0];
    int p1_y = mp[p1_index][1];
    int p1_dx = mp[p1_index][2];
    int p1_dy = mp[p1_index][3];

    if(mp[p1_index][2] == -1 && mp[p1_index][3] == 0) // 1st particle going left, so due to head on collision, 2nd particle will go right
    {
        // if(mp[p1_index][0] == 0 || mp[p1_index][3] == m-1) // if the collision is happening on uppermost or lowermost boundary, no way to go up or down for one particle
        // {
        //     // do nothing
        // }
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
        // if(mp[p1_index][0] == 0 || mp[p1_index][3] == m-1) // if the collision is happening on uppermost or lowermost boundary, no way to go up or down for one particle
        // {
        //     // do nothing
        // }
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
        // if(mp[p1_index][0] == 0 || mp[p1_index][3] == n-1) // if the collision is happening on leftmost or rightmost boundary, no way to go left or right for one particle
        // {
        //     // do nothing
        // }
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
        // if(mp[p1_index][0] == 0 || mp[p1_index][3] == n-1) // if the collision is happening on leftmost or rightmost boundary, no way to go left or right for one particle
        // {
        //     // do nothing
        // }
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

void resolve_collision(map<pair<int, int>, vector<int>>& collision_stat, map<int, vector<int>>& mp, int n, int m)
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


void solve(map<int, vector<int>>& mp, int n, int m, int k, int t)
{
    map<pair<int, int>, vector<int>> collision_stat; // (<x, y>, vector<index number of particles>)
    int t1 = t;
    while(t--)
    {
        resolve_collision(collision_stat, mp, n, m);
        collision_stat.clear();
        int cnt = 0;

        for(auto &it: mp) // by reference for permanent changes
        {
            int particle_index = it.first;
            int x = it.second[0];
            int y = it.second[1];
            int dx = it.second[2];
            int dy = it.second[3];

            it.second[0] = it.second[0] + it.second[2];
            it.second[1] = it.second[1] + it.second[3];

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
            if(it.second[0] >= n) // reached rightmost boundary, so at time t reverse its direction and set at point n-1, and at t+1, move it by dx
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
            if(it.second[1] < 0) // reached uppermost boundary, so at time t reverse its direction and set at point 0, and at t+1, move it by dy
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
            collision_stat[{it.second[0], it.second[1]}].push_back(cnt);
            cnt++;
        }
        cout << "Iteration " << (t1-t) << endl;
        print_particles(mp);
    }
}


int main()
{
    // N , M - the size of the Grid, K the number of particles in the simulation and T the number of time steps.
    int n, m, k, t;
    cin >> n >> m >> k >> t;

    map<int, vector<int>> mp; // (particle no., vector<x, y, dx, dy>)

    for(int i=0; i<k; i++)
    {
        int x, y;
        char ch;
        cin >> x >> y >> ch;

        vector<int> v;
        v.push_back(x);
        v.push_back(y);
        v.push_back(m1[ch]);
        v.push_back(m2[ch]);

        if(ch == 'U')
        {
            mp[i] = vector<int>({x, y, m1[0], m2[0]});
        }
        else if(ch == 'D')
        {
            mp[i] = vector<int>({x, y, m1[1], m2[1]});
        }
        else if(ch == 'L')
        {
            mp[i] = vector<int>({x, y, m1[2], m2[2]});
        }
        else if(ch == 'R')
        {
            mp[i] = vector<int>({x, y, m1[3], m2[3]});
        }
    }

    for(auto it: mp)
    {
        cout << "Particle no. = " << it.first << ", <x, y, dx, dy> = " << it.second[0] << ", " << it.second[1] << ", " << it.second[2] << ", " << it.second[3] << endl;
    }

    solve(mp, n, m, k, t);

    return 0;

}