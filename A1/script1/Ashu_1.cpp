#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <mpi.h>
using namespace std;

int K = 1000;
int M = 16; 
int N = 16; 

void offset_to_xy(int N, int M, int myOffset,int &x,int &y){
	x = floor(myOffset/M);
	y = myOffset%M;
}

bool mandelbrot(double real ,double imag) {
	double zReal = real;
	double zImag = imag;
    double i2, r2; 
	for (int i = 0; i <= K; ++i) {
		r2 = zReal * zReal;
		i2 = zImag * zImag;
		if (r2 + i2 > 4.0){ 
		    break;
		}
		zImag = 2.0 * zReal * zImag + imag;
		zReal = r2 - i2 + real;
	}
	return (r2 + i2 <= 4.0)?true:false;
}

void someFucnt(int N, int M, int offset, bool *myChunk, int chunkSize){
	double x_lowerbound = -1.5;
	double x_upperbound = 1.0;
	
	double y_lowerbound = -1.0;
	double y_upperbound = 1.0;

	double dx = (x_upperbound - x_lowerbound)/(M - 1);
	double dy = (y_upperbound - y_lowerbound)/(N - 1);
	
	for(int k = 0;k<chunkSize;k++){
		int i,j;
		offset_to_xy(N,M,offset+k,i,j);
		double x = x_lowerbound + j*dx; 
		double y = y_upperbound - i*dy; 
		myChunk[k] = mandelbrot(x,y);
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

	//int meta[4];
	vector<int> meta(5);
	int totalSize; 
	int chunkSize;
	int myOffset;
	bool *myChunk, *serialized;//,*myChunk,;
	if(rank == root){
		cin>>M>>N>>K;
		totalSize =  N * M;
		chunkSize = ceil((double)totalSize /(double)size); // . better take ceil!!!
		totalSize = chunkSize * size;
		
		meta[0] = N;
		meta[1] = M;
		meta[2] = K;
		meta[3] = chunkSize;
		meta[4] = totalSize;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(meta.data(),5,MPI_INT,root,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank != root){
		N = meta[0];
		M = meta[1];
		K = meta[2];
		totalSize =  meta[4];
		chunkSize = meta[3];
		//printf("I am rank:%d, recieved: %d, %d , %d \n",rank, meta[0],meta[1],meta[2]);
	}
	//vector<bool> serialized(totalSize);.\shri
	serialized = new bool[totalSize];
	myOffset = rank * chunkSize;
	// if(myOffset + chunkSize > totalSize){
	// 	chunkSize = totalSize - myOffset;
	// }
	//vector<bool> myChunk(chunkSize);
	myChunk = new bool[chunkSize];
	someFucnt(N,M,myOffset,myChunk,chunkSize);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(myChunk,chunkSize,MPI_CXX_BOOL,serialized,chunkSize, MPI_CXX_BOOL,root,MPI_COMM_WORLD ) ;
    MPI_Barrier(MPI_COMM_WORLD);
	if(rank == root){
		vector<vector<bool> > v(N+1,vector<bool>(M+1,false));
		for(int k = 0;k<totalSize;k++){
			int x,y;
			offset_to_xy(N,M,k,x,y);
			v[x][y] = serialized[k];
		}

		for(int i =0;i<N;i++){
			for(int j =0;j<M;j++){
				cout<<v[i][j]<<" ";
			}
			cout<<endl;
		}
	}
	MPI_Finalize(); 
	delete []serialized;
	delete []myChunk;
	return 0;
}