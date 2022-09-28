#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// ====================== Task Struct ============
#include "task.h"

// Global variables for Scheduling.
int current_time = 0; 
int numProc = 0;
int tot_burst = 0;

//Global variables for memory shared among threads
int bufferCount = 0;
int done = 0;
char buffer1[11];
char buffer2[11];
pthread_mutex_t lock;
pthread_mutex_t readingMutex;
pthread_mutex_t receivingMutex;
pthread_cond_t calculated = PTHREAD_COND_INITIALIZER;
pthread_cond_t read = PTHREAD_COND_INITIALIZER;

// ============= Main // Parent Thread =================
int main(int argc, char* argv[])
{
    char outputArr[50];
    pthread_t srtfT;
    pthread_t ppT;
    int tally = 0;
    int done = 0;
    //initialise lock
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&readingMutex, NULL);
    pthread_mutex_init(&receivingMutex, NULL);


    do
    {
        if(pthread_create(&srtfT, NULL, &srtf, NULL) != 0)
        {
            return NULL;
        }

        if(pthread_create(&ppT, NULL, &pp, NULL) != 0)
        {
            return NULL;
        }
        printf("Enter Filename: \n");
        scanf("%s", buffer1);

        if(strcmp(buffer1, "QUIT") == 0)
        {
            done = 1;
            printf("SRTF terminating.....\n");
            printf("PP terminating.....\n");
            exit(0);
        }
        else if(buffer1[0] != '0/')
        {
            pthread_cond_broadcast(&read); //--- Kickstarts reading process - signals A & B to read.
        }

        // ======================================================================
        while(bufferCount == 0 && done == 0)
        {
            pthread_cond_wait(&calculated, &readingMutex); 
        }

        sprintf(outputArr,"%s",buffer2);
        
        tally++;
        bufferCount = 0;
        buffer2[0] = '\0'; // Empties the buffer.
        while(bufferCount == 0 && done == 0)
        {
            pthread_cond_wait(&calculated, &readingMutex); 
        }
        strcat(outputArr,buffer2);
        tally++;
        if(tally == 2 && done == 0)
        {
            printf("\n%s\n", outputArr);
        }
        bufferCount = 0;
        buffer2[0] = '\0'; // Empties the buffer.
        buffer1[0] = '\0'; // Empties the buffer.
        tally = 0; // Reset tally.
    } while(done == 0);

    // =============
    if(pthread_join(srtfT, NULL) != 0)
    {
        return NULL;
    }

    if(pthread_join(ppT, NULL) != 0)
    {
        return NULL;
    }

    printf("SRTF terminating.....\n");
    printf("PP terminating.....\n");

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&readingMutex);
    pthread_mutex_destroy(&receivingMutex);

    return 0;
}

// =================== Thread A: PP  ===================

void* pp()
{
    char output[] = "PP Simulation\n";
    // Start operation:
    Task taskArray[50];
    pthread_mutex_lock(&lock);

    while(buffer1[0] == '\0')
    {
        pthread_cond_wait(&read, &lock); // This gets unlocked when filename received.
    }
    

    readFile(buffer1, taskArray);    
    printf("%s", output);

    // Perform scheduling
    int maxIdx = -5;
    while(current_time != tot_burst)
    {
        maxIdx = -5; // Reset max, find new max.
        taskArray[maxIdx].priority = 9999;
        for(int ii=0;ii<numProc;ii++)
        {
            if(taskArray[ii].arrival_time <= current_time && taskArray[ii].time_left != 0 && taskArray[ii].priority < taskArray[maxIdx].priority) // Check if priority is lower.
            {
                maxIdx = ii;
            }
        }
        
        // Max/Current should be initialised per timestep.
        printf("P%d|", taskArray[maxIdx].pid);
        // If a task was found i.e. if maxIdx != -5
        if(maxIdx != -5)
        {
            // Decrement remaining time.
            taskArray[maxIdx].time_left--;
            // Increment total time 
            current_time++;
            if(taskArray[maxIdx].time_left == 0)
            {
                //add CT
                taskArray[maxIdx].complete_time = current_time; 

                //calc and store TT/WT
                // TT = CT - AT 
                taskArray[maxIdx].turnaround_time = taskArray[maxIdx].complete_time - taskArray[maxIdx].arrival_time;
                // WT = TT - BT
                taskArray[maxIdx].waiting_time = taskArray[maxIdx].turnaround_time - taskArray[maxIdx].burst_time;
            }
        }
        else
        {
            current_time++;
        }
    }
    printf("\n\n");

    int maxWT = 0;
    int maxTT = 0;
    for(int ii=0;ii<numProc;ii++)
    {
        maxWT += taskArray[ii].waiting_time;
        maxTT += taskArray[ii].turnaround_time;
    }

    float avgWT=0,avgTT=0;
    avgTT = (float) maxTT / numProc;
    avgWT  = (float) maxWT / numProc;
    
    sprintf(buffer2, "PP: avgTT = %f | avgWT = %f\n", avgTT, avgWT);
    // Reset variables.
    current_time = 0;
    numProc = 0;
    tot_burst = 0;
    bufferCount++;
    // Signal that thread has written to buffer.
    pthread_cond_signal(&calculated);
    pthread_mutex_unlock(&lock);
    return NULL;
}

// =================== Thread B: SRTF ===================
void* srtf()
{
    char output[] = "SRTF Simulation: \n";
    // Start operation:
    Task taskArray[50];
    pthread_mutex_lock(&lock);
    
    while(buffer1[0] == '\0' && done == 0)
    {
        pthread_cond_wait(&read, &lock); // This gets unlocked when filename received.
    }

    readFile(buffer1, taskArray);
    printf("%s", output);

    // Perform scheduling
    int maxIdx = -5;
    while(current_time != tot_burst)
    {
        maxIdx = -5; // Reset max, find new max.
        taskArray[maxIdx].time_left = 9999;
        for(int ii=0;ii<numProc;ii++)
        {
            
            if(taskArray[ii].arrival_time <= current_time && taskArray[ii].time_left != 0 && taskArray[ii].time_left < taskArray[maxIdx].time_left)
            {
                maxIdx = ii;
            }
        }
        
        // Max/Current should be initialised per timestep.
        printf("P%d|", taskArray[maxIdx].pid);
        // If a task was found i.e. if maxIdx != -5
        if(maxIdx != -5)
        {
            // Decrement remaining time.
            taskArray[maxIdx].time_left--;
            // Increment total time 
            current_time++;
            if(taskArray[maxIdx].time_left == 0)
            {
                //add CT
                taskArray[maxIdx].complete_time = current_time; 

                //calc and store TT/WT
                // TT = CT - AT 
                taskArray[maxIdx].turnaround_time = taskArray[maxIdx].complete_time - taskArray[maxIdx].arrival_time;
                // WT = TT - BT
                taskArray[maxIdx].waiting_time = taskArray[maxIdx].turnaround_time - taskArray[maxIdx].burst_time;
            }
        }
        else
        {
            current_time++;
        }
    }
    printf("\n\n");

    int maxWT = 0;
    int maxTT = 0;
    for(int ii=0;ii<numProc;ii++)
    {
        maxWT += taskArray[ii].waiting_time;
        maxTT += taskArray[ii].turnaround_time;
    }

    float avgWT=0,avgTT=0;
    avgTT = (float) maxTT / numProc;
    avgWT  = (float) maxWT / numProc;

    sprintf(buffer2, "SRTF: avgTT = %f | avgWT = %f\n", avgTT, avgWT);
    // Reset variables.
    current_time = 0; 
    numProc = 0;
    tot_burst = 0;
    bufferCount++;
    // Signal that thread has written to buffer.
    pthread_cond_signal(&calculated);
    // Unlock mutex.
    pthread_mutex_unlock(&lock);
    return NULL;
}

// =================== File IO ===================
int readFile(char* argv, Task* taskArray)
{
    FILE* file; /* File pointer to the file stream */
    
    int counter = 0;
    // ==============
    //Opens file with a name specified by user in the command line
    file = fopen(argv, "r");

    if(file == NULL) /* If file cannot open */
    {
        printf("Error - could not open file. ");
    }
    else // Read line by line.
    {
        while(!feof(file)) /* && (error == FALSE)) */
        {
            // Reading Format: {Arrival} <space> {Burst} <space> {Priority}           
            fscanf(file, "%d %d %d", &(taskArray[counter].arrival_time), (&taskArray[counter].burst_time), (&taskArray[counter].priority));
            /* Read in the file and assort each part of the file to its according part in our taskArray - array of structs */
            taskArray[counter].pid = counter + 1;
            taskArray[counter].time_left = taskArray[counter].burst_time; // Burst = Orig burst time (constant), timeleft changes upon bursts.
            tot_burst += taskArray[counter].burst_time;
            counter++;
            numProc++;
        }
        if(ferror(file))
        {
            perror("Error reading from file. \n");
        }
        fclose(file);
    }
    return NULL;
}