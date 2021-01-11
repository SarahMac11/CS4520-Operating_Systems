#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
// I think P is Priority but that's not used so making it PSRJF = Preemtive Shortest Remaining Job First 
// #OVER ACHIEVER WHERES MY EXTRA CREDIT
#define P "PSRJF"
#define RR "RR"
#define SJF "SJF"

// FUNCTION DEFINITIONS

// Source Cited: https://www.geeksforgeeks.org/write-your-own-atoi/
// Implements atoi function
// \param str is a pointer to input string set to be sorted through
// \return the result from input string
int myAtoi_helper(char *str);

// Prints visual statistics to read me file and print to screen
// \param fp if pointer to readme.md file
// \param st_alg is a pointer to the specified scheduling algorithim input (FCFS, P, RR, or SJF)
// \param sr is a pointer to the schedule result struct
void print_stats_helper(FILE *fp, const char *sr_alg, ScheduleResult_t *sr);

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
    // check args
    if (argc < 3 || argc > 4) 
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        return EXIT_FAILURE;
    }

    //abort();  // replace me with implementation.

    // load process control blocks with 1st arg
    dyn_array_t *ready_queue = load_process_control_blocks(argv[1]);
    ScheduleResult_t *result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));

    //quantum and round robin set up
    size_t quantum = 4;
    char rr[19] = "";

    if(argv[3]) {
        quantum = myAtoi_helper(argv[3]);
        // if quantum is provided
        snprintf(rr, 19, "RR: Quantum = %d", (int)quantum);
    } else
        // set quantum to 4 if not provided
        snprintf(rr, 19, "RR: Quantum = %d", 4);

    // set up statistics record to be added to readme file
    struct stat stats_record;
    FILE *fp = fopen("../readme.md", "a");
    stat("../readme.md", &stats_record);

    // set up statistics table header in readme file
    // exsisting characters in readme.md file with my additional note = 957
    if(stats_record.st_size == 958) {
        fprintf(fp, "\n\n| Scheduling Algorithm | Average Turnaround Time | Average Waiting Time | Total Clock Time |\n");
        fprintf(fp, "|----------------------|-------------------------|----------------------|------------------|\n");
    }

    // swtich cases for argv 2 input (first letter only)
    switch(*argv[2]) {
        // FCFS - First Come First Serve
        case 'F':
            if(first_come_first_serve(ready_queue, result))
                print_stats_helper(fp, FCFS, result);
            break;
            // Priority isn't used so just going with it!
            // PSRTF - Preemtive Shortest Remaining Time First?
            // AKA function Shortest Remaining Time First
        case 'P':
            if(shortest_remaining_time_first(ready_queue, result))
                print_stats_helper(fp, P, result);
            break;
            // RR - Round Robin
        case 'R':
            if(argc == 4 && sscanf(argv[3], "%zu", &quantum)) {
                if(round_robin(ready_queue, result, quantum))
                    print_stats_helper(fp, rr, result);
            } else
                fprintf(stderr, "ERROR: Invalid Quantum Input\n");
            break;
            // SJF - Shortest Job First
        case 'S':
            if(shortest_job_first(ready_queue, result))
                print_stats_helper(fp, SJF, result);
            break;
            // default case
        default:
            printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
            break;
    }

    // clean up
    fclose(fp);
    dyn_array_destroy(ready_queue);
    free(result);

    return EXIT_SUCCESS;
}

/*
 * My helper functions
 */

int myAtoi_helper(char *str) {

    // Initialize result
    int res = 0;

    // Iterate through all characters 
    // of input string and update result
    for (int i = 0; str[i] != '\0'; ++i) 
        res = res * 10 + str[i] - '0'; 

    // return result
    return res;
}

void print_stats_helper(FILE *fp, const char *sr_alg, ScheduleResult_t *sr) {
    // print result to read me file
    fprintf(fp, "| %-20s | %-23f | %-20f | %-16lu |\n", sr_alg, sr->average_turnaround_time, sr->average_waiting_time, sr->total_run_time);
    // print results for reference
    printf("\n| Scheduling Algorithm | Average Turnaround Time | Average Waiting Time | Total Clock Time |\n");
    printf("|----------------------|-------------------------|----------------------|------------------|\n");
    printf("| %-20s | %-23f | %-20f | %-16lu |\n", sr_alg, sr->average_turnaround_time, sr->average_waiting_time, sr->total_run_time);
}
