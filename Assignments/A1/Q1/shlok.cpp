#include<iostream>
#include<math.h>
#include <mpi.h>
using namespace std;

int cal(double x, double y, int k);
void sqr(double& x, double& y);
double mag(double x, double y);

int main(int argc, char** argv){
    int n ,m ,k;
    int rank,size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int val[3];
    if(rank == 0)
    {
        cin>>n;
        cin>>m;
        cin>>k;
        val[0] = m;
        val[1] = n;
        val[2] = k;
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(val,3,MPI_INT,0,MPI_COMM_WORLD);
    // MPI_Barrier(MPI_COMM_WORLD);
    n = val[0];
    m = val[1];
    k = val[2];

    int ele_per_process = ceil((n*m)/(double)size);
    int max_size  = ele_per_process;
    // if(ele_per_process == 0)
    //     ele_per_process  = 1;
    
    if(((n*m)%size)!=0 && (rank == size-1))
        ele_per_process = (n*m)%ele_per_process;
    // cout<<rank<<" "<<ele_per_process<<endl;
    bool *arr = new bool[ele_per_process];
    int start = rank * max_size;
    int r = start/m;
    int c = start%m;
    int l = 0;

    // cout << "n: " << n << ", m: " << m << endl;

    double dx = 2.5/(m-1);
    double dy = 2.0/(n-1);

    // cout << "x_step: " << dx << ", y_step: " << dy << endl;

    // cout<<r<<" "<<c<<" "<<ele_per_process<<endl;
    for(int i = r ; i<n && l<ele_per_process ; i++)
    {
        for(int j = c; j<m && l<ele_per_process;j++)
        {
            // cout << "i: " << i << ", j: " << j << endl;
            double y = 1 - i*dy;
            double x = -1.5 + j*dx;
            // cout << "x: " << x << ", y: " << y << endl;
            arr[l] = cal(x,y,k);
            // cout<<arr[l]<<" ";
            l++;
        }
        // cout<<endl;
        c=0;
    }

    bool *global_arr = NULL;
    int total_size = n*m;
    if(rank == 0)
        global_arr = new bool[total_size];
    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(arr, ele_per_process, MPI_CXX_BOOL, global_arr, max_size, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
    // MPI_Barrier(MPI_COMM_WORLD);


    if(rank == 0)
    {
        l=0;
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<m;j++)
            {
                cout<<global_arr[l++] << " ";
            }
            cout<<endl;
        }
        // delete[] global_arr;
    }

    delete arr;
    delete[] global_arr;
    arr = NULL;
    global_arr = NULL;
    MPI_Finalize();


    return 0;    
}



int cal(double cx, double cy, int k)
{
    double zx = 0.0,zy = 0.0;
    for(int i=0;i<k;i++)
    {
        sqr(zx,zy);
        zx = zx + cx;
        zy = zy + cy;
        if(mag(zx,zy) > 2.0)
            return 0;
    }
    if(mag(zx,zy) > 2.0)
            return 0;
    else
        return 1;
}

void sqr(double& x, double& y)
{
    double nx = x*x - y*y;
    double ny = 2*x*y;
    x = nx;
    y = ny;
}

double mag(double x, double y)
{
    return sqrt((double)(x*x + y*y));
}
