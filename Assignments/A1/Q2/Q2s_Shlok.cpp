#include<bits/stdc++.h>
using namespace std;

void solve(int arr[], int n, int m, int k, int t);
void cal(int arr[],map<pair<int,int>,vector<int>>& collisions, int n, int m);
void change_dir(int arr[],int l1, int l2, int n, int m);
void print(int arr[], int k);

int main()
{
    int n,m,k,t;
    cin>>n;
    cin>>m;
    cin>>k;
    cin>>t;
    // swap(n,m);
    int* arr = new int[4*k];
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
    solve(arr,n,m,k,t);
    cout<<endl;
    print(arr,k);
    delete[] arr; 
    return 0;
}

void solve(int arr[], int n, int m, int k, int t)
{
    map<pair<int,int>,vector<int>> collisions;
    for(int i=0; i<t;i++)
    {
        cal(arr,collisions,n,m);
        collisions.clear();
        for(int l = 0; l<4*k; l+=4)
        {
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
            collisions[{arr[l],arr[l+1]}].push_back(l);
        }
        cout<<"For "<<i<<"th iteration\n";
        print(arr,k);
    }
}

void cal(int arr[],map<pair<int,int>,vector<int>>& collisions,int n,int m)
{
    for(auto i:collisions)
    {
        pair<int,int> key = i.first;
        vector<int> val = i.second;
        if(val.size()<=1 || val.size()>2)
            continue;
        if((arr[val[0]+2] + arr[val[1]+2] == 0) && (arr[val[0]+3] + arr[val[1]+3] == 0)) //collison occured
        {

            cout<<"conflict occured for:"<<val[0]<<" "<<val[1]<<endl;
            int l  =val[0];
            cout<<arr[l]<<" "<<arr[l+1]<<" "<<arr[l+2]<<" "<<arr[l+3]<<endl;
            int l1 = val[1];
            cout<<arr[l1]<<" "<<arr[l1+1]<<" "<<arr[l1+2]<<" "<<arr[l1+3]<<endl;
            change_dir(arr,val[0],val[1],n,m);
        }
    }
}

void change_dir(int arr[], int l1, int l2, int n, int m)
{
    if(arr[l1+2] == -1 && arr[l1+3] == 0) // Going left
    {
        if(arr[l1+1] == 0 || arr[l1+1] == m-1)
            return;
        arr[l1+2] = 0;
        arr[l1+3] = 1;

        arr[l2+2] = 0;
        arr[l2+3] = -1;
    }
    else if(arr[l1+2] == 1 && arr[l1+3] == 0) // Going right
    {
        if(arr[l1+1] == 0 || arr[l1+1] == m-1)
            return;
        arr[l1+2] = 0;
        arr[l1+3] = -1;

        arr[l2+2] = 0;
        arr[l2+3] = 1;
    }
    else if(arr[l1+2] == 0 && arr[l1+3] == 1) // Going Up
    {
        if(arr[l1] == 0 || arr[l1] == n-1)
            return;
        arr[l1+2] = 1;
        arr[l1+3] = 0;

        arr[l2+2] = -1;
        arr[l2+3] = 0;
    }
    else //Going down
    {
        if(arr[l1] == 0 || arr[l1] == n-1)
            return;
        arr[l1+2] = -1;
        arr[l1+3] = 0;
        arr[l2+2] = 1;
        arr[l2+3] = 0;
    }
}

void print(int arr[], int k)
{
    map<pair<int,int>,char> m;
    for(int l=0; l<4*k; l+=4)
    {
        pair<int,int> p = {arr[l],arr[l+1]};
        if(arr[l+2] == 0 && arr[l+3] == -1)
            m[p] = 'D';
        else if(arr[l+2] == 0 && arr[l+3] == 1)
            m[p] = 'U';
        else if(arr[l+2] == -1 && arr[l+3] == 0)
            m[p] = 'L';
        else
            m[p] = 'R';
    } 

    for(auto i:m)
    {
        cout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
    }  
}