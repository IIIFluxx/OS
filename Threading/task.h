#ifndef TASK_H
#define TASK_H


    /* Defined macros*/
    #define TRUE 1
    #define FALSE 0

    typedef struct
    {
        int pid;
        int arrival_time, burst_time, priority; // Read in from file.
        int waiting_time, turnaround_time; // What we need to calculate.
        int time_left, complete_time; // What we maintain throughout the program.
        int status;
    }Task;

    // Forward declarations
    int readFile(char* argv, Task* taskArray);
    void* pp();
    void* srtf();

#endif