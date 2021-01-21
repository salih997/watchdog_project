/*! 
    \file watchdog.cpp
    @author Salih Bedirhan EKER
    @date 02.01.2021

    All the important work is done here.
    The main idea of this project is that executing different processes concurrently and handling the communication between them.
    Main method creates the necessarry number of processes using fork() and exec() and starts waiting.
    When a process is terminated watchdog gets the information with wait(NULL) and recreate it.
    At the end this file is terminated by receiving SIGTERM signal.
    
    Interprocess communication is held by different methods.
    Using named pipes is an effective way to dealing with issue.
    I learned using exec() system call, named pipes and signal handling with this assignment.
*/
#include <iostream>
#include <string>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <sys/wait.h>

using namespace std;

/// Output file of the watchdog program
string wdoutfile;

struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec


/// Termination for watchdog 
///
/// Signal handler for SIGTERM signal. After all instructions are executed, executor sends SIGTERM to watchdog
/// @param signal the received signal number
void signal_sigterm(int signal){
    
    // Creating a fstream
    fstream fstreamm;
    fstreamm.open(wdoutfile, ios::app);

    fstreamm<< "Watchdog is terminating gracefully" << "\n";
    
    fstreamm.close();
    exit(0);
}

/// Gets the index of an element
///
/// Used when a process is died then wait(NULL) return pid of dead process and this function finds its process number P#
/// @warning provided the index can be found
/// @returns P#
/// @param arr pidList
/// @param n pid of the terminated process
int getindex(int *arr, int n){
    int index=0;
    while(1){
        if(arr[index] != n)
             index++;
        else
            break;
    }
    return index;
}





/*! \fn int main(int argc, char *argv[])
*/
int main(int argc, char *argv[]){

    //Installing the term signal handler
    signal(SIGTERM, signal_sigterm);  
    
    
    int n = stoi(argv[1]);          // Number of process (apart from watchdog)

    string pout = argv[2];          // Output file of processes

    string wdout = argv[3];         // Output file of watchdog
    wdoutfile = wdout;
  
    pid_t pidList[n+1];             // Holds pids of processes                                                                                                         

    // Creating a fstream
    fstream fstreamm;

    // Opening the ouput files
    // First open and close process output immediately so that we can have an empty and existing output file
    // Then clear and open output file of the watchdog
    fstreamm.open(pout, ios::out);
    fstreamm.close();
    fstreamm.open(wdout, ios::out);
    

    
    int writeend;
    
    // PID of watchdog
    int wdpid = (long)getpid();
    pidList[0] = wdpid;                 // Insert the pidlist of the pid of the watchdog
                                        // pidList[0] = PID_OF_WD            //it does not used
    


    // FIFO path for named pipe
    char *myfifo = "/tmp/myfifo";       

    // Creating the file, set permission
    mkfifo(myfifo, 0644);

    // Create a buffer to write into
    char buff[30];

    // Open FIFO for only write
    writeend = open(myfifo, O_WRONLY);

    // String manipulations to first message and write it to pipe
    string temp = "P" + to_string(0) + " " + to_string((long)getpid());
    strcpy(buff, temp.c_str());
    write(writeend, buff, 30);




    // CREATING N PROCESS AND THEN WRITING THEIR INFO TO PIPE 
    for(int i=1; i<=n; i++){
        nanosleep(&delta, &delta);

        int childpid = fork();

        if(childpid == 0){      

        string temp = to_string(i);
        char myarg[30];
        strcpy(myarg, temp.c_str());

        char myarg2[30];
        strcpy(myarg2, pout.c_str());
        
        execlp("./process", "./process", myarg, myarg2, NULL);      // exec() process object and pass process number and output file as arguments
        
        }else if(childpid > 0){ 

        pidList[i] = childpid;
        
        // Write info about child process to pipe
        string temp = "P" + to_string(i) + " " + to_string(childpid);
        strcpy(buff, temp.c_str());
        write(writeend, buff, 30);
        
        // Write to output file about the child process
        string tempwrite = "P" + to_string(i) + " is started and it has a pid of " + to_string(childpid);
        fstreamm<< tempwrite<<"\n";

        }

    }// CREATION OF N PROCESSES IS ENDED
    close(writeend);
    fstreamm.close();


    while(1){

        int killedpid = wait(NULL);                     // watchdog sleeps until a process terminates
        int killedPnum = getindex(pidList, killedpid);

        fstreamm.open(wdout, ios::app);
        writeend = open(myfifo, O_WRONLY);

        if(killedPnum == 1){                        // Killed process is head process now wd kill all other processes and recreate all of them
        
            fstreamm<< "P1 is killed, all processes must be killed"<<"\n";
            fstreamm<< "Restarting all processes"<<"\n";

            for(int i=2; i<=n; i++){                
                kill(pidList[i],SIGTERM);
                wait(NULL);
            }

            // Create P1 to PN
            // CREATING N PROCESS AND THEN WRITING THEIR INFO TO PIPE     
            for(int i=1; i<=n; i++){
                nanosleep(&delta, &delta);

                int childpid = fork();
                if(childpid == 0){      

                string temp = to_string(i);
                char myarg[30];
                strcpy(myarg, temp.c_str());

                char myarg2[30];
                strcpy(myarg2, pout.c_str());
                
                execlp("./process","./process", myarg, myarg2, NULL);
                
                }else if(childpid > 0){ 

                pidList[i] = childpid;
                
                // Write info about child process to pipe
                string temp = "P" + to_string(i) + " " + to_string(childpid);
                strcpy(buff, temp.c_str());
                write(writeend, buff, 30);

                // Write to output file about the child process
                string tempwrite = "P" + to_string(i) + " is started and it has a pid of " + to_string(childpid);
                fstreamm<< tempwrite<<"\n";

                }

            }// CREATION OF N PROCESSES IS ENDED
           
            

        }else{                                      // Killed process is not head so just recreate this one 

            string tempwrite = "P" + to_string(killedPnum) + " is killed";
            fstreamm<< tempwrite<<"\n";

            tempwrite = "Restarting P" + to_string(killedPnum);
            fstreamm<< tempwrite<<"\n";

            int childpid = fork();
            if(childpid == 0){
                
                string temp = to_string(killedPnum);
                char myarg[30];
                strcpy(myarg, temp.c_str());

                char myarg2[30];
                strcpy(myarg2, pout.c_str());

                execlp("./process","./process", myarg, myarg2, NULL);

            }else if(childpid > 0){

            pidList[killedPnum] = childpid;
            
            // Write to output file about the child process
            string temp = "P" + to_string(killedPnum) + " " + to_string(childpid);
            strcpy(buff, temp.c_str());
            write(writeend, buff, 30);

            // Write to output file about the child process
            string tempwrite = "P" + to_string(killedPnum) + " is started and it has a pid of " + to_string(childpid);
            fstreamm<< tempwrite<<"\n";

            }


        }
        close(writeend);
        fstreamm.close();
        
    }
    

    return 0;
}
