# Question 1

- The total time complexity <b> O(((M * N)/P) * K)</b>
- The total message complexity <b> O(P) </b>
- The space requirements <b> O(M*N) </b>

### The Program is tested on following test cases contained in the file.

### 1.txt contains input:
#### 16 16 1000
### With Expected and actual Output to be:
#### 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 
#### 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
#### 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 
#### 0 0 0 0 0 0 1 1 1 1 0 0 0 0 0 0 
#### 0 0 0 0 0 0 1 1 1 1 1 0 0 0 0 0 
#### 0 0 0 0 0 1 1 1 1 1 1 1 0 0 0 0 
#### 0 0 0 1 0 1 1 1 1 1 1 1 0 0 0 0 
#### 0 0 1 1 1 1 1 1 1 1 1 1 0 0 0 0 
#### 0 0 1 1 1 1 1 1 1 1 1 1 0 0 0 0 
#### 0 0 0 1 0 1 1 1 1 1 1 1 0 0 0 0 
#### 0 0 0 0 0 1 1 1 1 1 1 1 0 0 0 0 
#### 0 0 0 0 0 0 1 1 1 1 1 0 0 0 0 0 
#### 0 0 0 0 0 0 1 1 1 1 0 0 0 0 0 0 
#### 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 
#### 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
#### 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 


# Question 2

- The total time complexity <b> O(K * T)</b>
- The total message complexity <b> O(P * T) </b>
- The space requirements <b> O(K) </b>

### The Program is tested on following test cases contained in the file.

### 1.txt contains input:
#### 10 12 16 100
#### 0 1 R
#### 2 1 L
#### 3 3 U
#### 9 9 L
#### 6 8 D
#### 4 8 L
#### 7 3 R
#### 5 6 U
#### 0 1 U
#### 2 1 D
#### 3 3 R
#### 9 9 U
#### 6 8 L
#### 4 8 D
#### 7 3 U
#### 5 6 R
### With Expected and actual Output to be:
#### 0 6 L
#### 0 9 D
#### 2 11 U
#### 3 3 R
#### 3 6 L
#### 3 7 D
#### 4 10 D
#### 5 3 R
#### 5 4 D
#### 6 4 U
#### 7 5 U
#### 7 9 R
#### 7 10 R
#### 8 6 L
#### 9 1 D
#### 9 10 R 


### Running Time analysis is as below:

### Testing for Number of process:  1
Testing Test case: 1.txt
### Execution Time:

real	0m0.563s
user	0m0.027s
sys	0m0.053s
# #########################################
Testing for Number of process:  2
#### Testing Test case: 1.txt

### Execution Time:

real	0m0.504s
user	0m0.032s
sys	0m0.142s
# #########################################
Testing for Number of process:  3
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.547s
user	0m0.050s
sys	0m0.203s
# #########################################
Testing for Number of process:  4
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.572s
user	0m0.046s
sys	0m0.370s
# #########################################
Testing for Number of process:  5
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.610s
user	0m0.122s
sys	0m0.534s
# #########################################
Testing for Number of process:  6
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.635s
user	0m0.106s
sys	0m0.862s
# #########################################
Testing for Number of process:  7
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.637s
user	0m0.178s
sys	0m0.968s
# #########################################
Testing for Number of process:  8
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.671s
user	0m0.257s
sys	0m1.351s
# #########################################
Testing for Number of process:  9
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.660s
user	0m0.161s
sys	0m1.642s
# #########################################
Testing for Number of process:  10
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.658s
user	0m0.183s
sys	0m1.973s
# #########################################
Testing for Number of process:  11
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.667s
user	0m0.285s
sys	0m2.369s
# #########################################
Testing for Number of process:  12
#### Testing Test case: 1.txt
### Execution Time:

real	0m0.716s
user	0m0.398s
sys	0m2.778s
# #########################################

# Question 3

- The total time complexity <b> O((N^2/P) * N)</b>
- The total message complexity <b> O(N * P) </b>
- The space requirements <b> O(M*N) </b>

### The Program is tested on following test cases contained in the file.

### 1.txt contains input:
#### 8
#### 5 7
#### 3 6
#### 7 5
#### 6 17
#### 1 16
#### 4 8
#### 2 13
#### 8 28
### With Expected and actual Output to be:
#### 238
#### 2 6 4 2 4 0 8 6 


### Running Time analysis is as below:

### Testing for Number of process:  1
Testing Test case: 
1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.484s
user	0m0.017s
sys	0m0.055s
# #########################################
Testing for Number of process:  2
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.546s
user	0m0.013s
sys	0m0.141s
# #########################################
Testing for Number of process:  3
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.578s
user	0m0.048s
sys	0m0.255s
# #########################################
Testing for Number of process:  4
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.561s
user	0m0.089s
sys	0m0.343s
# #########################################
Testing for Number of process:  5
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.572s
user	0m0.111s
sys	0m0.493s
# #########################################
Testing for Number of process:  6
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.640s
user	0m0.263s
sys	0m0.762s
# #########################################
Testing for Number of process:  7
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.633s
user	0m0.142s
sys	0m0.988s
# #########################################
Testing for Number of process:  8
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.675s
user	0m0.238s
sys	0m1.379s
# #########################################
Testing for Number of process:  9
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.666s
user	0m0.303s
sys	0m1.581s
# #########################################
Testing for Number of process:  10
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.666s
user	0m0.300s
sys	0m1.950s
# #########################################
Testing for Number of process:  11
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.686s
user	0m0.264s
sys	0m2.466s
# #########################################
Testing for Number of process:  12
#### Testing Test case: 1.txt
### Output:
#### 238
#### 2 6 4 2 4 0 8 6 
### Execution Time:

real	0m0.697s
user	0m0.474s
sys	0m2.694s
# #########################################
