#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <mpi.h>
using namespace std;

struct Particle{
    int id, x, y, direction;
    Particle(){

    }
    Particle(int id, int x, int y, int direction){
        this->id = id;
        this->x = x;
        this->y = y;
        this->direction = direction;
    }
    int * serialize(){
        int * a = new int[4];
        a[0] = id;
        a[1] = x;
        a[2] = y;
        a[3] = direction;
        return a;
    }
};

int M,N,T,K; 

int lowerlimit;
int upperlimit;
vector<Particle> list_crossed_upperlimit, list_crossed_lowerlimit;
map<int,vector<int>> m_move = {{'R',{1,0}},{'L',{-1,0}},{'U',{0,1}},{'D',{0,-1}}};
map<int,int> m_change_direction = {{'R','D'},{'L','U'},{'U','R'},{'D','L'}};
map<int,int> m_reverse_direction = {{'R','L'},{'L','R'},{'U','D'},{'D','U'}};


void receive_particles(int SENDER, map<int,Particle> &m_list_particles){
    int size;
    MPI_Recv(&size, 1, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int temp[4];
    while(size){
        MPI_Recv(temp, 4, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        m_list_particles.insert({temp[0],Particle(temp[0],temp[1],temp[2],temp[3])});
        size--;
    }
}


void advance_one_time(map<int,Particle> &m_list_particles){
    vector<int> remove_list;

    for(auto &itr : m_list_particles){
        Particle &p = itr.second;
        int flag = 0;
        p.y += m_move[p.direction][0];
        p.x += m_move[p.direction][1];

        //Handling Exceeding boundaries
        int x1 = p.y , y1 = p.x;

        if(p.y < 0){
            p.y = 1;
        }else if(p.y >= M){
            p.y = M-2;
        }

        if(p.x < 0){
            p.x = 1;
        }else if(p.x >= N){
            p.x = N-2;
        }
        
        //Changing Direction if collided with boundary
        if(p.y!=x1 || p.x!=y1)
            p.direction = m_reverse_direction[p.direction];


        //if changed process boundary
        if(p.x < lowerlimit){
            flag = 1;
            list_crossed_lowerlimit.push_back(p); 
        }else if(p.x >= upperlimit){
            flag = 1;
            list_crossed_upperlimit.push_back(p);
        }
        if(flag)
            remove_list.push_back(itr.first);
    }
    for(auto i : remove_list){
        m_list_particles.erase(i);
    }
}


bool check_for_collision(Particle &p1, Particle &p2){
    int x = p1.y;
    int y = p1.x;
    
    if(x!=0 && x!=M-1 && y!=0 && y!=N-1 && m_reverse_direction[p1.direction]==p2.direction){
            return true;
    }
    return false;
}

int main(int argc, char *argv[]) {
	int rank, size ,root = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	int comm_size;
	char name[MPI_MAX_OBJECT_NAME];
	int name_length;

	MPI_Comm_size(comm, &comm_size);
	MPI_Comm_get_name(comm, name, &name_length);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    map<int,Particle> m_list_particles; //particle_id:: vector: particle_id(again!),x,y,(char)dir
    int chunkSize, myOffset;
    int offsets[size], positions[3*size];
	int meta_data[5];
    int lower_process = rank-1;
    int upper_process = rank+1;


	if(rank == root){
        //meta_data { N, M, number of Particles(K), Time , ChunkSize}
		//tanking input
        cin>>meta_data[0]>>meta_data[1]>>meta_data[2]>>meta_data[3];
        
        //calculate the size of my portion
        chunkSize = ceil((double)meta_data[1]/size);
        meta_data[4] = chunkSize;    
        
        int particle_id = 0;
        int x,y;
        char z;
        //temporary map to hold list of particles that are to be distriibuted to certain process.
        map<int,vector<Particle> > m_processes_particles;

        while(particle_id<meta_data[2]){
            cin>>x>>y>>z;
            m_processes_particles[y/chunkSize].push_back(Particle(particle_id,y,x,(int)z));
            particle_id++; 
        }
        int process_id = 0;
        while(process_id<size){
            int size = m_processes_particles[process_id].size();
            MPI_Send(&size, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD);
            while(size){
                Particle &p = m_processes_particles[process_id][size-1];
                int * v = p.serialize();
                MPI_Send(v, 4, MPI_INT, process_id, 0, MPI_COMM_WORLD);
                delete v;
                size--;
            }
            process_id++;
        }
	}
    receive_particles(root, m_list_particles);
	MPI_Bcast(meta_data,5,MPI_INT,root,MPI_COMM_WORLD);
    M = meta_data[0];
    N = meta_data[1];
    K = meta_data[2];
    T = meta_data[3];
    chunkSize = meta_data[4];

    //setting my offset, i.e. the range in with a particular process will handle the particle data.
    myOffset = rank * chunkSize;
    chunkSize = min(chunkSize, N - myOffset); 
    lowerlimit = myOffset;
    upperlimit = lowerlimit + chunkSize;

    while(T){
        advance_one_time(m_list_particles);
       
        if(rank!=size-1){
            int size = list_crossed_upperlimit.size();
            MPI_Send(&size, 1, MPI_INT, upper_process, 0, MPI_COMM_WORLD);
            while(size){
                Particle &p = list_crossed_upperlimit[size-1];
                int * v = p.serialize();
                MPI_Send(v, 4, MPI_INT, upper_process, 0, MPI_COMM_WORLD);
                delete v;
                size--;
            }
            list_crossed_upperlimit.clear();  
        }
        if(rank!=0){
            int size = list_crossed_lowerlimit.size();
            MPI_Send(&size, 1, MPI_INT, lower_process, 0, MPI_COMM_WORLD);
            while(size){
                Particle &p = list_crossed_lowerlimit[size-1];
                int * v = p.serialize();
                MPI_Send(v, 4, MPI_INT, lower_process, 0, MPI_COMM_WORLD);
                delete v;
                size--;
            }
            list_crossed_lowerlimit.clear();
        }

        if(rank!=size-1){
            receive_particles(upper_process,m_list_particles);
        }
        if(rank!=0){
            receive_particles(lower_process,m_list_particles);
        }

        //make them collide
        map<vector<int>,vector<int>> m;
        for(auto itr : m_list_particles){
            Particle &p = itr.second;
            m[{p.x,p.y}].push_back(p.id); 
        }
        for(auto i : m){
            if (i.second.size() == 2){
                int p1 = i.second[0];
                int p2 = i.second[1];
                if(check_for_collision(m_list_particles[p1],m_list_particles[p2])){
                    int &dir1 = m_list_particles[p1].direction;
                    int &dir2 = m_list_particles[p2].direction;
                    dir1 = m_change_direction[dir1];
                    dir2 = m_change_direction[dir2];
                }
            }
        }

        T--;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    //print_particles(rank,m_list_particles);
    vector<Particle> part;
    for(auto i : m_list_particles){
        part.push_back(i.second);
    }

    int s = part.size();
    MPI_Send(&s, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
    while(s){
        Particle &p = part[s-1];
        int * v = p.serialize();
        MPI_Send(v, 4, MPI_INT, root, 0, MPI_COMM_WORLD);
        delete v;
        s--;
    }

    if(rank == root){
        map<int,Particle > m_temp_list_particles;
        int process_id = 0;
        while(process_id<size){
            receive_particles(process_id,m_temp_list_particles);
            process_id++;
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