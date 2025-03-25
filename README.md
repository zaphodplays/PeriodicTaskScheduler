# C++ Periodic Task Scheduler , Author - Amar Shukla, email - amaresh.shukla@gmail.com


# INSTRUCTIONS FOR INSTALLING boost AND nlohmann::json
# This scheduler is built and tested on macOs(15.1.1 )
# The Scheduler uses third party libraries boost::asio and nlohmann::json
# 1. Please install nlohmann using this command (on macos): brew install nlohmann-json  
# 2. Please install boost using this command (on macos): brew install boost


# BUILD INSTRUCTIONS
# from inside your workspace folder, run command - 
# clang++ -std=c++17 ./Metric.cpp ./Task.cpp ./NetworkTask.cpp ./SystemTask.cpp ./ConnectToServerTask.cpp ./Scheduler.cpp ./RunScheduler.cpp -o ./RunScheduler

# RUN INSTRUCTIONS
# provide a config file to the scheduler and start it using this command - 
# ./RunScheduler scheduler_config.json 
# type 'exit' on the terminal whenever you want to exit
# the scheduler will exit and save its state to ./scheduler_state.json
# on restarting the scheduler as before(with the config file), the scheduler will sync the state of all tasks from the saved state file

# ------------------------------------------------------------------------------------------------------------------------------------------------------- #

# GENERAL INFO #

# Scheduler class - responsible for management of creation, scheduling and execution of all tasks
# reads a config file and creates and adds tasks for scheduling
# runs and executes tasks on their specific periods
# can be stopped and saves state for each task with its metrics. saves the state as a json file in the working directory. The statefile name is specified in the scheduler config json.
# can be restarted , loads the saved state for each tasks and runs them according to the saved state(picks up where it left off)
# handles execution of different tasks as async operations on multiple threads 

# Task class - Abstract base class for different types of tasks
# contains Task specific logic for execution of low level tasks like running a network ping or a system command
# collects and stores metrics on Task specific data

# ConnectToServer class - a concrete example task that tries a tcp connection to a provided (domain, port) and collects the connection time as a metric

# scheduler_config.json
# This is provided as input argument when the scheduler is run
# This config file is used to provide all configuration and task specific information to the scheduler
# It contains config information like the state file location and the number of threads used in thread pool
# It contains config information about each task like its period and specific tasks information for each tasks like 'domain' for a network task etc

# scheduler_state.json
# This is the saved state of the scheduler when it is stopped by the user
# It contains saved data about each task - namely its most recent metrics and its remaining_time for the next run
# on restarting the scheduler, the scheduler looks for the statefile(as specified in config) and on finding a statefile, loads the state for all tasks like metrics and remaining_time
# this way all tasks continue running as if there was no interruption in terms of their collected metrics and their timings

# RunScheduler provides the main where the Scheduler is invoked from, It also does simplistic user input monitoring to exit the scheduler when the user presses 'exit' 




