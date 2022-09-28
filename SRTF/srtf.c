#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Task Struct
#include "task.h"

// Global variables
int current_time = 0; 
int numProc = 0;
int tot_burst = 0;


// ============= Main =================
int main(int argc, char* argv[])
{
    int currentIdx = 0;
    char menuStr[] = "SRTF Simulation\n"
                     "Enter Filename: \n";
    // Start operation:
    char filename[20];
    Task taskArray[50];
    while(1)
    {
        printf("\n%s", menuStr);
        scanf("%s", filename);
        if(strcmp(filename, "QUIT") == 0)
        {
            break;
        }
        else
        {
            readFile(filename, taskArray);
            for(int ii=0;ii<numProc;ii++)
            {
                printf("%d %d %d \n", taskArray[ii].arrival_time,taskArray[ii].burst_time, taskArray[ii].priority);
            }
            
            // Perform scheduling
            int maxIdx = -5;
            while(current_time != tot_burst)
            {
                maxIdx = -5; // Reset max, find new max.
				taskArray[maxIdx].time_left = 9999;
                for(int ii=0;ii<numProc;ii++)
                { // Check if time_left is lower.
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

            printf("avgWT = %f\n", avgWT);
            printf("avgTT = %f\n", avgTT);
            current_time = 0; 
            numProc = 0;
            tot_burst = 0;
        }
    }
}

// =================== File IO ===================
int readFile(char* argv, Task* taskArray)
{
    FILE* file; /* File pointer to the settings file stream */
    
    int counter = 0;
    /* Opens file with a name specified by user in the command line */
    file = fopen(argv, "r");

    if(file == NULL) /* If file cannot open */
    {
        printf("Error - could not open file. ");
    }
    else // Read line by line.
    {
        while(!feof(file)) /* && (error == FALSE)) */
        {
            // Format: {Arrival} <space> {Burst} <space> {Priority}           

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
}