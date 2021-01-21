# watchdog_project

This is a watchdog process simulation that keep tracks of each process and keep them alive.

executor.cpp works in the background and creates watchdog and sends the signals according to input.txt.
watchdog.cpp creates all processes and when its one of child processes is terminated, watchdog recreates it.
process.cpp is a simple process prints a message when it receives a signal.

How to compile and run example:
g++ executor.cpp -std=c++14 -o executor
./executor 5 input.txt &
g++ process.cpp -std=c++14 -o process
g++ watchdog.cpp -std=c++14 -o watchdog
./watchdog 5 process_out.txt watchdog_out.txt
