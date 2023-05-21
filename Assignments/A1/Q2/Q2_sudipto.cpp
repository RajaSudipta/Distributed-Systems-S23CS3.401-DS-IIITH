#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <mpi.h>
using namespace std;

map<int,vector<int>> m_move = {{'R',{1,0}},{'L',{-1,0}},{'U',{0,1}},{'D',{0,-1}}};
map<int,int> m_change_direction = {{'R','D'},{'L','U'},{'U','R'},{'D','L'}};
map<int,int> m_reverse_direction = {{'R','L'},{'L','R'},{'U','D'},{'D','U'}};
int M,N,T,K; 

struct Particle{
    int id, x, y, direction;
    bool dir_change;
    Particle(){

    }
    Particle(int id, int x, int y, int direction){
        this->id = id;
        this->x = x;
        this->y = y;
        this->direction = direction;
    }
    vector<int> serialize(){
        return vector<int>{id,x,y,direction};
    }
    void move_ahead(){
        this->y += m_move[this->direction][0];
        this->x += m_move[this->direction][1];
        dir_change = true;
        if(this->y >= M)
            this->y = M-2;
        else if(this->y < 0)
            this->y = 1;
        else if(this->x >= N)
            this->x = N-2;
        else if(this->x < 0)
            this->x = 1;
        else 
            dir_change = false;
        if(dir_change)
            this->direction = m_reverse_direction[this->direction];
    }
};

int lowerlimit;
int upperlimit;
vector<Particle> list_crossed_upperlimit, list_crossed_lowerlimit;

void move_particles_ahead(map<int,Particle> &my_particles){
    vector<int> crossed_particles;

    for(auto &itr : my_particles){
        Particle &p = itr.second;
        int flag = 1;
        p.move_ahead();
        //if changed process boundary
        if(p.x < lowerlimit){
            list_crossed_lowerlimit.push_back(p); 
        }else if(p.x >= upperlimit){
            list_crossed_upperlimit.push_back(p);
        }else{
            flag = 0;
        }
        if(flag)
            crossed_particles.push_back(itr.first);
    }
    for(auto i : crossed_particles){
        my_particles.erase(i);
    }
}

int main(int argc, char *argv[]) {
	int rank, size ,root = 0;
    int comm_size;
    int name_length;
	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	char name[MPI_MAX_OBJECT_NAME];
	MPI_Comm_get_name(comm, name, &name_length);
    MPI_Comm_size(comm, &comm_size);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	

    map<int,Particle> my_particles; //particle_id:: vector: particle_id(again!),x,y,(char)dir
    int portion_length, process_offset;
	int input_arguments[5];
    int lower_process = rank-1;
    int upper_process = rank+1;


	if(rank == root){
        cin>>input_arguments[0]>>input_arguments[1]>>input_arguments[2]>>input_arguments[3];
        input_arguments[4] = ceil((double)input_arguments[1]/size);    
        
        map<int,vector<Particle> > process_particle_map;
        
        int x;
        char z;
        int y;
        
        int particle_id = 0;

        for(int id = 0;id<input_arguments[2];id++){
            cin>>x>>y>>z;
            process_particle_map[y/portion_length].push_back(Particle(id,y,x,(int)z));
            particle_id++; 
        }
        for(int rank = 0;rank<size;rank++){
            int size = process_particle_map[rank].size();
            MPI_Send(&size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            for(int it = size;it>0;it--){
                Particle &p = process_particle_map[rank][it-1];
                MPI_Send(p.serialize().data(), 4, MPI_INT, rank, 0, MPI_COMM_WORLD);
            }
        }
	}

    int size_r;
    MPI_Recv(&size_r, 1, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int temp[4];
    for(int x = size_r;x>0;x--){
        MPI_Recv(temp, 4, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        my_particles.insert({temp[0],Particle(temp[0],temp[1],temp[2],temp[3])});
    }
	MPI_Bcast(input_arguments,5,MPI_INT,root,MPI_COMM_WORLD);
    
    N = input_arguments[1];
    T = input_arguments[3];
    K = input_arguments[2];
    M = input_arguments[0];
    portion_length = input_arguments[4];

    //setting my offset, i.e. the range in with a particular process will handle the particle data.
    process_offset = rank * portion_length;
    lowerlimit = process_offset;
    portion_length = min(portion_length, N - lowerlimit); 
    upperlimit = lowerlimit + portion_length;

    for(int x = 0; x<T;x++){
        list_crossed_lowerlimit.clear();
        list_crossed_upperlimit.clear();  
        move_particles_ahead(my_particles);
       
        if(rank!=size-1){
            size_r = list_crossed_upperlimit.size();
            MPI_Send(&size_r, 1, MPI_INT, upper_process, 0, MPI_COMM_WORLD);
            for(int x = size_r;x>0;x--){
                Particle &p = list_crossed_upperlimit[x-1];
                MPI_Send(p.serialize().data(), 4, MPI_INT, upper_process, 0, MPI_COMM_WORLD);
            }
        }
        if(rank!=0){
            size_r = list_crossed_lowerlimit.size();
            MPI_Send(&size_r, 1, MPI_INT, lower_process, 0, MPI_COMM_WORLD);
            for(int x = size_r;x>0;x--){
                Particle &p = list_crossed_lowerlimit[x-1];
                MPI_Send(p.serialize().data(), 4, MPI_INT, lower_process, 0, MPI_COMM_WORLD);
            }
        }

        if(rank!=size-1){
            MPI_Recv(&size_r, 1, MPI_INT, upper_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            for(int x = size_r;x>0;x--){
                MPI_Recv(temp, 4, MPI_INT, upper_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                my_particles.insert({temp[0],Particle(temp[0],temp[1],temp[2],temp[3])});
            }
        }
        if(rank!=0){
            MPI_Recv(&size_r, 1, MPI_INT, lower_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            for(int x = size_r;x>0;x--){
                MPI_Recv(temp, 4, MPI_INT, lower_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                my_particles.insert({temp[0],Particle(temp[0],temp[1],temp[2],temp[3])});
            }
        }

        //make them collide
        map<vector<int>,vector<int>> m;
        for(auto itr : my_particles){
            Particle &p = itr.second;
            m[{p.x,p.y}].push_back(p.id); 
        }
        for(auto i : m){
            if (i.second.size() == 2){
                Particle &p1 = my_particles[i.second[0]];
                Particle &p2 = my_particles[i.second[1]];
                if(p1.y!=0 && p1.y!=M-1 && p1.x!=0 && p1.x!=N-1 && m_reverse_direction[p1.direction]==p2.direction){
                    p1.direction = m_change_direction[p1.direction];
                    p2.direction = m_change_direction[p2.direction];
                }
            }
        }
       
    }
    MPI_Barrier(MPI_COMM_WORLD);
    //print_particles(rank,my_particles);
    vector<Particle> part;
    for(auto i : my_particles){
        part.push_back(i.second);
    }

    int s = part.size();
    MPI_Send(&s, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
    while(s){
        Particle &p = part[s-1];
        MPI_Send(p.serialize().data(), 4, MPI_INT, root, 0, MPI_COMM_WORLD);
        s--;
    }

    if(rank == root){
        map<int,Particle > m_temp_list_particles;
        int process_rank = 0;
        while(process_rank<size){
            size_r = 0;
            MPI_Recv(&size_r, 1, MPI_INT, process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int temp[4];
            while(size_r){
                MPI_Recv(temp, 4, MPI_INT, process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                m_temp_list_particles.insert({temp[0],Particle(temp[0],temp[1],temp[2],temp[3])});
                size_r--;
            }
            process_rank++;
        }
        vector<vector<int>> ans;
        for(auto i: m_temp_list_particles){
            ans.push_back({i.second.y,i.second.x,i.second.direction});
        }
        sort(ans.begin(),ans.end());
        for(auto i : ans){
            cout<<i[0]<<" "<<i[1]<<" "<<(char)i[2]<<endl;
        }
        cout<<"########################################################################\n\n\n";
    }

	MPI_Finalize(); 
	return 0;
}