#ifndef PROCESS_SCHEDULING_H
#define PROCESS_SCHEDULING_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "dyn_array.h"

    typedef struct 
    {
        uint32_t remaining_burst_time;  // the remaining burst of the pcb
        uint32_t priority;				// The priority of the task
        uint32_t arrival;				// Time the process arrived in the ready queue
        bool started;              	    // If it has been activated on virtual CPU
    } 
    ProcessControlBlock_t;		        // you may or may not need to add more elements

    typedef struct 
    {
        float average_waiting_time;     // the average waiting time in the ready queue until first schedue on the cpu
        float average_turnaround_time;  // the average completion time of the PCBs
        unsigned long total_run_time;   // the total time to process all the PCBs in the ready queue
    } 
    ScheduleResult_t;

    // Reads the PCB burst time values from the binary file into ProcessControlBlock_t remaining_burst_time field
    // for N number of PCB burst time stored in the file.
    // \param input_file the file containing the PCB burst times
    // \return a populated dyn_array of ProcessControlBlocks if function ran successful else NULL for an error
    dyn_array_t *load_process_control_blocks(const char *input_file);

    // Runs the First Come First Served Process Scheduling algorithm over the incoming ready_queue
    // \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
    // \param result used for first come first served stat tracking \ref ScheduleResult_t
    // \return true if function ran successful else false for an error
    bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result);

    // Runs the Shortest Job First Scheduling algorithm over the incoming ready_queue
    // \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
    // \param result used for shortest job first stat tracking \ref ScheduleResult_t
    // \return true if function ran successful else false for an error
    bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result);

    // Runs the Priority algorithm over the incoming ready_queue
    // \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
    // \param result used for shortest job first stat tracking \ref ScheduleResult_t
    // \return true if function ran successful else false for an error
    bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result);

    // Runs the Round Robin Process Scheduling algorithm over the incoming ready_queue
    // \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
    // \param result used for round robin stat tracking \ref ScheduleResult_t
    // \param the quantum
    // \return true if function ran successful else false for an error
    bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum);

    // Runs the Shortest Remaining Time First Process Scheduling algorithm over the incoming ready_queue
    // \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
    // \param result used for shortest job first stat tracking \ref ScheduleResult_t
    // \return true if function ran successful else false for an error
    bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result);

    //compare remaining burst time of two pcbs 
    // \param pcb1 void pointer to a pcb to find remaining burst time
    // \param pcb2 void pointer to a pcb to find remaining burst time
    // \return -1 if pcb1 remaining burst time is greater
    // \return 1 if pcb2 remaining burst time is greater
    // \return 0 if pcb1 = pcb2 remaining burst time
    int shortest_burst_time_helper(const void *pcb1, const void *pcb2);

    // evaluate available pcbs in ready queue 
    // \param ready queue is dyn_array of type ProcessControlBlock_t that contains up to N elements
    // \param dyn arr queue is the initial or current dyn_array queue
    // \param run time is the current total run time
    void pcb_queue_helper(dyn_array_t *ready_queue, dyn_array_t *dyn_arr_queue, uint32_t run_time);

    // sort available pcbs in ready queue by time
    // \param ready queue is dyn_array of type ProcessControlBlock_t that contains up to N elements
    // \param dyn array queue is initial or current dyn_array queue
    // \param run time is the current total run time of pcb queues
    void pcb_queue_sort_by_time_helper(dyn_array_t* ready_queue, dyn_array_t* dyn_arr_queue, uint32_t run_time);

    // arrival calculator comparator
    // \param pcb1 pointer to a pcb to get arrival time
    // \param pcb2 pointer to a pcb to get arrival time
    // \return remaining burst time difference if pcb1 and pcb2 arrival times are the same, else return difference between arrival times
    int arrival_calc_helper(const void *pcb1, const void *pcb2);

    // burst time calculator comparator
    // \param pcb1 pointer to a pcb queue to get remaining burst time
    // \param pcb2 pointer to a pcb queue to get remaining burst time
    // \return difference in pcb1 and pcb2 remaining burst times
    int burst_time_calc_helper(const void *pcb1, const void *pcb2); 


#ifdef __cplusplus
}
#endif
#endif
