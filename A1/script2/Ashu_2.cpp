#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <mpi.h>
using namespace std;



inline void offset_to_xy(int N, int M, int myOffset,int &x,int &y){
	x = myOffset/M;
	y = myOffset%M;
}

void print_particles(int rank, map<int,vector<int> > &m_list_particles){
    for(auto i : m_list_particles){
        printf("#RANK %d\n\tParticle_ID :%d,\tX,Y:(%d,%d)\tDir:%c\n",rank,i.first,i.second[2],i.second[1],(char)i.second[3]);
    }
};


void send_particles(int RECEIVER, vector<vector<int> > &list_particles){
    int size = list_particles.size();
    MPI_Send(&size, 1, MPI_INT, RECEIVER, 0, MPI_COMM_WORLD);
    while(size){
        int * temp = list_particles[size-1].data();
        MPI_Send(temp, 4, MPI_INT, RECEIVER, 0, MPI_COMM_WORLD);
        size--;
    }
}

void receive_particles(int SENDER, map<int,vector<int> > &m_list_particles){
    int size;
    MPI_Recv(&size, 1, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    while(size){
        int temp[4];
        MPI_Recv(temp, 4, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        m_list_particles[temp[0]]= vector<int>{temp[0],temp[1],temp[2],temp[3]};
        size--;
    }
}

void advance_one_time(map<int,vector<int> > &m_list_particles){
    for(auto &i : m_list_particles){
        int particle_id = i.first;
        int &y = i.second[1];
        int &x = i.second[2];
        int &dir = i.second[3];

        switch(dir){
            case 'R':
                x+=1;
                break;
            case 'L':
                x-=1;
                break;
            case 'U':
                y+=1;
                break;
            case 'D':
                y-=1;
                break;
        }
    }
}

void handle_border_recoll(int rank, int size, int N, int M, map<int,vector<int> > &m_list_particles){
    for(auto &i : m_list_particles){
        int particle_id = i.first;
        int &y = i.second[1];
        int &x = i.second[2];
        int &dir = i.second[3];
        
        //handling along the verticle axis
        if(y < 0){
            y = 1;
            dir = 'U';
        }else if(y >= N){
            y = N-2;
            dir = 'D';
        }

        //handling along the horizontal axis
        if(x < 0){
            x = 1;
            dir = 'R';
        }else if(x >= M){
            x = M-2;
            dir = 'L';
        }
    }
};

void move_particles_crossed(
            int rank,
            int chunkSize, 
            int myOffset, 
            map<int,vector<int> > &m_list_particles, 
            map<string,vector<vector<int>>> &m_crossed_particles)
{
    int lowerlimit = myOffset;
    int upperlimit = lowerlimit + chunkSize;
    vector<int> remove_list;
    for(auto &i : m_list_particles){
        int &y = i.second[1];

        if(y < lowerlimit){
            m_crossed_particles["lowerlimit"].push_back(i.second);
            remove_list.push_back(i.first);
        }else if(y >= upperlimit){
            m_crossed_particles["upperlimit"].push_back(i.second);
            remove_list.push_back(i.first);
        }
    }
    for(auto i : remove_list){
        m_list_particles.erase(i);
    }
}

bool do_collide(int N, int M, int p1, int p2, map<int,vector<int> > &m_list_particles){
    int x = m_list_particles[p1][2];
    int y = m_list_particles[p1][1];
    
    if(x!=0 && x!=M-1 && y!=0 && y!=N-1){
        int dir1 = m_list_particles[p1][3];
        int dir2 = m_list_particles[p2][3];
        //cout<<endl<<dir1<<"::"<<dir2<<endl;
        if((dir1 == 'U' && dir2 == 'D') || (dir1 == 'D' && dir2 == 'U')){
            return true;
        }
        if((dir1 == 'R' && dir2 == 'L') || (dir1 == 'L' && dir2 == 'R')){
            return true;
        }
    }
    return false;
}
void change_dir(int &a){
    switch(a){
        case 'U':
            a = 'R';
            break;
        case 'D':
            a = 'L';
            break;
        case 'R':
            a = 'D';
            break;
        case 'L':
            a = 'U';
    }
}
void handle_collisions(int N, int M, map<int,vector<int> > &m_list_particles){
    map<pair<int,int>,vector<int>> m;
    for(auto i : m_list_particles){
        pair<int,int> temp = {i.second[1],i.second[2]};
        m[temp].push_back(i.first); 
    }
    for(auto i : m){
        if (i.second.size() == 2){
            int p1 = i.second[0];
            int p2 = i.second[1];
            if(do_collide(N,M,p1,p2,m_list_particles)){
                change_dir(m_list_particles[p1][3]);
                change_dir(m_list_particles[p2][3]);
            }
        }
    }
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
    int K = 1000;
    int M = 32; 
    int N = 32; 
    int T = 32; 
    map<int,vector<int> > m_list_particles; //particle_id:: vector: particle_id(again!),x,y,(char)dir
    int chunkSize, myOffset;
    int offsets[size], positions[3*size];
	int meta_data[5];
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
        map<int,vector<vector<int> > > m_processes_particles;

        while(particle_id<meta_data[2]){
            cin>>x>>y>>z;
            m_processes_particles[y/chunkSize].push_back({particle_id,y,x,(int)z});
            particle_id++; 
        }

        int process_id = 0;
        while(process_id<size){
            send_particles(process_id,m_processes_particles[process_id]);
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

    while(T){
        advance_one_time(m_list_particles);
        handle_border_recoll(rank,size,N,M,m_list_particles);
        map<string,vector<vector<int>>> m_crossed_particles;
        move_particles_crossed(rank, chunkSize, myOffset, m_list_particles, m_crossed_particles);
        if(rank!=size-1)
            send_particles(rank+1, m_crossed_particles["upperlimit"]);
        if(rank!=0)
            send_particles(rank-1, m_crossed_particles["lowerlimit"]);
        if(rank!=size-1)
            receive_particles(rank+1,m_list_particles);
        if(rank!=0)
            receive_particles(rank-1,m_list_particles);

        handle_collisions(N,M,m_list_particles);

        T--;
    }
    //print_particles(rank,m_list_particles);
    vector<vector<int>> part;
    for(auto i : m_list_particles){
        part.push_back(i.second);
    }
    send_particles(root, part);

    if(rank == root){
        map<int,vector<int> > m_temp_list_particles;
        int process_id = 0;
        while(process_id<size){
            receive_particles(process_id,m_temp_list_particles);
            process_id++;
        }
        vector<vector<int>> ans;
        for(auto i: m_temp_list_particles){
            ans.push_back({i.second[2],i.second[1],i.second[3]});
        }
        sort(ans.begin(),ans.end());
        cout << endl;
        for(auto i : ans){
            cout<<i[0]<<" "<<i[1]<<" "<<(char)i[2]<<endl;
        }
        // cout<<"########################################################################\n\n\n";
    }

	MPI_Finalize(); 
	return 0;
}
