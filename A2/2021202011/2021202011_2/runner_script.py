#!/usr/bin/python3

# python3 runner_script.py /opt/hadoop-3.2.1/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar input.txt /input/ /output2 "./"
# python3 ./q2/runner_script.py /opt/hadoop-3.2.1/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar ./q2/input.txt /input/ /output2 "./q2/"

# python3 runner_script.py /opt/hadoop-3.2.1/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar 1.in /input/ /output2 "./"

import sys
import os

jar_file__path = sys.argv[1]
local_input_file_path = sys.argv[2] 
input_dir_path = sys.argv[3] 
output_dir_path = sys.argv[4] 
mapper_path = sys.argv[5]
reducer_path = sys.argv[5]

input_file_name =  local_input_file_path.split('/')[-1]
# input_file = input_file.split('\\')[-1]
# input_file = input_file.split('//')[-1]
intermediate_file_name = 'intermediate.txt'

os.system('python3 ' + reducer_path + 'input_formatter.py ' + local_input_file_path)
print("\n********************************************")
print("input_formatter.py successfully executed")
print("============================================\n")


print("\n********************************************")
os.system('hdfs dfs -rm -R ' + output_dir_path)
os.system('hdfs dfs -rm -R ' + input_dir_path)
print("============================================\n")


os.system('hdfs dfs -mkdir ' + input_dir_path)
print("\n********************************************")
print(input_dir_path + " successfully created")
print("============================================\n")

os.system('hdfs dfs -put ' + intermediate_file_name + ' ' + input_dir_path)
print("\n********************************************")
print(intermediate_file_name + " successfully copied to " + input_dir_path)
print("============================================\n")


final_command = 'hadoop jar ' + jar_file__path + ' -input ' + input_dir_path + intermediate_file_name + ' -output ' + output_dir_path + ' -file '+ mapper_path + 'mapper.py -mapper ' + '"python3 mapper.py"' + ' -file ' + reducer_path + 'reducer.py -reducer ' + '"python3 reducer.py"'
print("\n************************************************************************************************************************************")
print('Final Command: ' + final_command)
print("====================================================================================================================================\n")
os.system(final_command)
print("\n************************************************************************************************************************************")
print("Final Result:")
os.system('hdfs dfs -cat ' + output_dir_path + '/part*')
print("====================================================================================================================================\n")
