 #!/usr/bin/python3

# python3 runner_script.py /opt/hadoop-3.2.1/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar input.txt /input /output3 mapper.py reducer.py
# python3 runner_script.py /opt/hadoop-3.2.1/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar input.txt /input/ /output3 "./"
# python3 ./q3/runner_script.py /opt/hadoop-3.2.1/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar ./q3/input.txt /input/ /output3 "./q3/"

import sys
import os

jar_file__path = sys.argv[1]
local_input_file_path = sys.argv[2] 
input_dir_path = sys.argv[3] 
output_dir_path = sys.argv[4] 
mapper_path = sys.argv[5]
reducer_path = sys.argv[5]

print("\n********************************************")
os.system('hdfs dfs -rm -R ' + output_dir_path)
os.system('hdfs dfs -rm -R ' + input_dir_path)
print("============================================\n")

os.system('hdfs dfs -mkdir ' + input_dir_path)
print("\n********************************************")
print(input_dir_path + " successfully created")
print("============================================\n")

os.system('hdfs dfs -put ' + local_input_file_path + ' ' + input_dir_path)
print("\n********************************************")
print(local_input_file_path + " successfully copied to " + input_dir_path)
print("============================================\n")

input_file =  local_input_file_path.split('/')[-1]
# input_file = input_file.split('\\')[-1]
# input_file = input_file.split('//')[-1]

final_command = 'hadoop jar ' + jar_file__path + ' -input ' + input_dir_path + input_file + ' -output ' + output_dir_path + ' -file '+ mapper_path + 'mapper.py -mapper ' + '"python3 mapper.py"' + ' -file ' + reducer_path + 'reducer.py -reducer ' + '"python3 reducer.py"'
print("\n************************************************************************************************************************************")
print("Final Command: " + final_command + "\n")
print("====================================================================================================================================\n")
os.system(final_command)
print("\n************************************************************************************************************************************")
print("\nFinal Result: ")
os.system('hdfs dfs -cat ' + output_dir_path + '/part*')
print("====================================================================================================================================\n")