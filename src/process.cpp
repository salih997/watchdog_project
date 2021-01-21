/*!
    \file process.cpp
    @author Salih Bedirhan EKER
    @date 02.01.2021

*/
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <fstream>

using namespace std;

/// Output file of the process all processes have access to this file
string poutfile;

///The number of this process
int myPnum;


/// Signal handler
///
/// Signal handler just writes the message that it received the signal excep for SIGTERM signal
/// @see signal_SIGTERM()
void signal_handler(int signal){
    fstream fstreamm;

    fstreamm.open(poutfile, ios::app);
    string temp = "P" + to_string(myPnum) + " received signal " + to_string(signal);
    fstreamm<<temp<<"\n";
    fstreamm.close();
}

/// Signal handler for SIGTERM signal
///
/// When process receive SIGTERM signal prints the message to the output file and terminate the process
void signal_SIGTERM(int signal){
    fstream fstreamm;

    fstreamm.open(poutfile, ios::app);
    string temp = "P" + to_string(myPnum) + " received signal 15, terminating gracefully";
    fstreamm<<temp<<"\n";
    fstreamm.close();
    exit(0);
}

/*! \fn int main(int argc, char *argv[])
    \brief Method just install signal handlers, print the message that it wait for a signal and then waits a signal

*/
int main(int argc, char *argv[]){

    int pnum = stoi(argv[1]);              
    myPnum = pnum;

    string pout = argv[2];        
    poutfile = pout;
    

    fstream fstreamm;
    fstreamm.open(pout, ios::app);

    string temp = "P" + to_string(pnum) + " is waiting for a signal";
    fstreamm<<temp<<"\n";
    
    fstreamm.close();


    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGTRAP, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGXCPU, signal_handler);

    signal(SIGTERM, signal_SIGTERM);
    

    while(1);

    return 0;
}