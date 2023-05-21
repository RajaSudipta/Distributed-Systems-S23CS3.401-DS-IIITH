#!/bin/bash
echo "Sudipta testing"
mpic++ Sudipta_3.cpp -o ss.o
> ss.txt
# mpirun -np 2 ./ss.o << 1.txt

for (( i = 1; i <= 12; i++ )) 
do
    echo "Nuber of process: " $i 
   for (( j = 1 ; j <= 6; j++ ))
   do
        filename="${j}.txt"
        echo $filename
        mpirun -np $i ./ss.o < $filename >> ss.txt
        echo "#########################################" >> ss.txt
  done
done

echo
echo "Ashutosh testing"
mpic++ Ashu_3.cpp -o aa.o
> aa.txt
# mpirun -np 2 ./ss.o << 1.txt

for (( i = 1; i <= 12; i++ )) 
do
    echo "Nuber of process: " $i
   for (( j = 1 ; j <= 6; j++ ))
   do
        filename="${j}.txt"
        echo $filename
        mpirun -np $i ./aa.o < $filename >> aa.txt
        echo "" >> aa.txt
        echo "#########################################" >> aa.txt
  done
done

echo
echo "Nitin testing"
mpic++ Nitin_3.cpp -o nn.o
> nn.txt
# mpirun -np 2 ./ss.o << 1.txt

for (( i = 1; i <= 12; i++ )) 
do
    echo "Nuber of process: " $i
   for (( j = 1 ; j <= 6; j++ ))
   do
        filename="${j}.txt"
        echo $filename
        mpirun -np $i ./nn.o < $filename >> nn.txt
        echo "" >> nn.txt
        echo "#########################################" >> nn.txt
  done
done

