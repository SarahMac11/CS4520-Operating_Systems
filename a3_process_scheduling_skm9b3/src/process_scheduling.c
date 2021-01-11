#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) 
{
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    //UNUSED(ready_queue);
    //UNUSED(result);
    //return false;

    // error check params
    if(!ready_queue || !result)
        return false;

    // initialize variables
    uint32_t size = dyn_array_size(ready_queue);
    uint32_t waiting_time = 0;
    uint32_t turnaround_time = 0;
    uint32_t run_time = 0;

    ProcessControlBlock_t pcb;

    // while size has not been capped
    for(uint32_t i = 0; i < size; i++) {
        // update variables
        waiting_time += run_time;
        dyn_array_extract_back(ready_queue, (void *)&pcb);
        run_time += pcb.remaining_burst_time;
        turnaround_time += run_time;
        turnaround_time -= pcb.arrival;
        waiting_time -= pcb.arrival;

        while(pcb.remaining_burst_time > 0)
            virtual_cpu(&pcb);
    }

    // calculate and set results
    result->average_waiting_time = (float)waiting_time / (float)size;
    result->average_turnaround_time = (float)turnaround_time / (float)size;
    result->total_run_time = run_time;

    return true;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    //UNUSED(ready_queue);
    //UNUSED(result);
    //return false;   

    // error check params
    if(!ready_queue || !result)
        return false;

    // initialize result variables
    result->average_waiting_time = 0;
    result->average_turnaround_time = 0;
    result->total_run_time = 0;

    bool extract = false;

    // initialize variables
    uint32_t size = dyn_array_size(ready_queue);
    uint32_t clock_time = 0;
    uint32_t total_latency = 0;
    uint32_t turnaround_time = 0;
    int arrival_sum = 0;

    // malloc space for pcbs
    ProcessControlBlock_t *pcb_ptr = malloc(sizeof(ProcessControlBlock_t));
    pcb_ptr = (ProcessControlBlock_t *)dyn_array_export(ready_queue);
    dyn_array_t *dyn_arr_queue = dyn_array_create(0, size, NULL);

    // iterate through size for each process
    for(uint32_t i = 0; i < size; i++) {
        for(uint32_t j = 0; j < size; j++) {
            // get pcbs that have arrived and have not started
            if(pcb_ptr[j].arrival <= clock_time && pcb_ptr[j].started == false) {
                // add to dyn array queue
                extract = dyn_array_push_back(dyn_arr_queue, &pcb_ptr[j]);
                if(extract == false)
                    return false;

                // update arrival sum
                arrival_sum += pcb_ptr[j].arrival;
                pcb_ptr[j].started = true;
            }
        }

        if(dyn_array_size(dyn_arr_queue) > 0) {
            // use shortest remaining burst time helper to sort by shortest burst time
            dyn_array_sort(dyn_arr_queue, shortest_burst_time_helper);

            while(dyn_array_size(dyn_arr_queue) > 0) {
                // while dyn array queue is not complete
                ProcessControlBlock_t pcb;
                extract = dyn_array_extract_back(dyn_arr_queue, &pcb);
                if(extract == false)
                    return false;

                // add to total latency
                total_latency += clock_time;
                pcb.started = true;

                // until virtual cpu is completed
                while(pcb.remaining_burst_time != 0) {
                    virtual_cpu(&pcb);
                    clock_time++;       // inc clock time
                }

                pcb.started = false;
                turnaround_time += clock_time;
            }
        }
    }

    // set results
    result->average_waiting_time = (float)(total_latency - arrival_sum) / size;
    result->average_turnaround_time = (float)(turnaround_time - arrival_sum) / size;
    result->total_run_time = clock_time;
    return true;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;   
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
    //UNUSED(ready_queue);
    //UNUSED(result);
    //UNUSED(quantum);
    //return false;

    // error check params
    if(!ready_queue || !result || quantum <= 0)
        return false;

    // initialize variables
    uint32_t size = dyn_array_size(ready_queue);
    dyn_array_t *dyn_arr_queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    // uint32_t types to easily calculate results
    uint32_t waiting_time = 0;
    uint32_t turnaround_time = 0;
    uint32_t run_time = 0;

    uint32_t current_run_time;

    // get pcb initial arrival
    for (uint32_t i = 0; i < size; i++) {
        ProcessControlBlock_t *pcb_ptr = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
        // store priority
        pcb_ptr->priority = pcb_ptr->arrival;
    }

    // set up initial pcb queue helper
    pcb_queue_helper(ready_queue, dyn_arr_queue, run_time);

    // while dyn array size has not been capped
    while(dyn_array_size(dyn_arr_queue) != 0) {
        ProcessControlBlock_t pcb;
        dyn_array_extract_front(dyn_arr_queue, &pcb);
        waiting_time += run_time - pcb.arrival;

        // get current run time
        if(pcb.remaining_burst_time > quantum)
            current_run_time = quantum;
        else
            current_run_time = pcb.remaining_burst_time;

        run_time += current_run_time;

        // while more run time exists
        while(current_run_time > 0) {
            virtual_cpu(&pcb);
            current_run_time--;
        }

        pcb.arrival = run_time;

        // get available pcb queue
        pcb_queue_helper(ready_queue, dyn_arr_queue, run_time);

        if(pcb.remaining_burst_time == 0)
            // set turn around time if no more remaining burst time
            turnaround_time += run_time - pcb.priority;
        else
            dyn_array_push_back(dyn_arr_queue, &pcb);
    }

    // set results
    result->average_waiting_time = (float)waiting_time / size;
    result->average_turnaround_time = (float)turnaround_time / size;
    result->total_run_time = run_time;

    // destroy dyn array queue
    dyn_array_destroy(dyn_arr_queue);
    return true;
}

dyn_array_t *load_process_control_blocks(const char *input_file) 
{

    //UNUSED(input_file);

    // error check params and input file
    if(!input_file || !strcmp(input_file, "") || !strcmp(input_file, "\n"))
        return NULL;

    // file extension check .bin
    char *ext = strrchr(input_file, '.');
    if(!ext || strcmp(ext, ".bin"))
        return NULL;

    // open file
    int fd = open(input_file, O_RDONLY);
    if(fd < 1) {
        close(fd);
        return NULL;
    }

    // buffer and bytes read
    uint32_t buff;
    int br = read(fd, &buff, sizeof(uint32_t));

    // error check buffer of fd
    if(br == 0 || br == -1) {
        close(fd);
        return NULL;
    }

    // pass values from input file to each pcb
    ProcessControlBlock_t *pcb = malloc(sizeof(ProcessControlBlock_t) * buff);
    for(uint32_t i = 0; i < buff; i++) {
        br = read(fd, &pcb[i].remaining_burst_time, sizeof(uint32_t));
        br = read(fd, &pcb[i].priority, sizeof(uint32_t));
        br = read(fd, &pcb[i].arrival, sizeof(uint32_t));
    }

    // array of pcbs
    dyn_array_t *dyn_arr = dyn_array_import((void *)pcb, sizeof(buff), sizeof(ProcessControlBlock_t), NULL);

    // close and free
    close(fd);
    free(pcb);
    return dyn_arr;
    //return NULL;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    //UNUSED(ready_queue);
    //UNUSED(result);
    //return false;

    // error check params
    if(!ready_queue || !result)
        return false;

    uint32_t size = dyn_array_size(ready_queue);

    // store inital arrival time in priority
    for(uint32_t i = 0; i < size; i++) {
        ProcessControlBlock_t *pcb_ptr = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
        pcb_ptr->priority = pcb_ptr->arrival;
    }

    dyn_array_t *dyn_arr_queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);

    uint32_t waiting_time = 0;
    uint32_t turnaround_time = 0;
    uint32_t run_time = 0;

    // use arrival calc helper to determine arrival difference and sort
    dyn_array_sort(ready_queue, arrival_calc_helper);
    // initial dyn array queue time
    pcb_queue_sort_by_time_helper(ready_queue, dyn_arr_queue, run_time);

    // while dyn array size has not been capped
    while(dyn_array_size(dyn_arr_queue) != 0) {
        ProcessControlBlock_t pcb;
        dyn_array_extract_front(dyn_arr_queue, &pcb);

        // update variables
        waiting_time += run_time - pcb.arrival;
        // create ready queue size and run time variables
        uint32_t ready_queue_size = dyn_array_size(ready_queue);
        uint32_t current_run_time;

        // if there's remaining space in ready queue
        if(ready_queue_size != 0) {
            uint32_t arrival_n;
            arrival_n = ((ProcessControlBlock_t *)dyn_array_at(ready_queue, dyn_array_size(ready_queue) - 1))->arrival;
            current_run_time = arrival_n - run_time;
        } else
            current_run_time = pcb.remaining_burst_time;

        // increment run time with current run time
        run_time += current_run_time;

        // while pcb has started
        while(current_run_time > 0) {
            virtual_cpu(&pcb);
            current_run_time--;
        }

        // add the pcb arrival to the total run time
        pcb.arrival = run_time;

        if(pcb.remaining_burst_time == 0)
            // check if pcb has completed and calc turnaround time with run time and initial dyn array queue time
            turnaround_time += run_time - pcb.priority;
        else
            dyn_array_push_back(dyn_arr_queue, &pcb);

        pcb_queue_sort_by_time_helper(ready_queue, dyn_arr_queue, run_time);
    }

    // set results
    result->average_waiting_time = (float)waiting_time / size;
    result->average_turnaround_time = (float)turnaround_time / size;
    result->total_run_time = run_time;

    // destroy dyn array queue
    dyn_array_destroy(dyn_arr_queue);

    return true;
}

int shortest_burst_time_helper(const void *pcb1, const void *pcb2)
{
    // sort decending

    // if pcb1 remaining burst time > pcb2 remaining burst time
    if (((ProcessControlBlock_t *)pcb1)->remaining_burst_time > ((ProcessControlBlock_t *)pcb2)->remaining_burst_time)
        return -1;
    // if pcb1 < pcb2 remaining burst time
    else if (((ProcessControlBlock_t *)pcb1)->remaining_burst_time < ((ProcessControlBlock_t *)pcb2)->remaining_burst_time)
        return 1;
    // if pcb1 = pcb2 remaining burst time
    return 0;
}

void pcb_queue_helper(dyn_array_t *ready_queue, dyn_array_t *dyn_arr_queue, uint32_t run_time) {
    size_t size = dyn_array_size(ready_queue);
    // run process on every queue until completed
    for(size_t i = 0; i < size; i++) {
        ProcessControlBlock_t pcb;
        dyn_array_extract_back(ready_queue, &pcb);
        if(pcb.arrival <= run_time)
            dyn_array_push_back(dyn_arr_queue, &pcb);
        else
            dyn_array_push_front(ready_queue, &pcb);
    }
}

void pcb_queue_sort_by_time_helper(dyn_array_t *ready_queue, dyn_array_t *dyn_arr_queue, uint32_t run_time) {
    // run pcb queue helper to evaluate available pcbs ready in queue
    pcb_queue_helper(ready_queue, dyn_arr_queue, run_time);
    // sort available pcb queues using time calc helper
    dyn_array_sort(dyn_arr_queue, burst_time_calc_helper);
}

int arrival_calc_helper(const void *pcb1, const void *pcb2) {
    // get arrival times for pcb1 and pcb2 to compare
    uint32_t a = ((ProcessControlBlock_t *)pcb1)->arrival;
    uint32_t b = ((ProcessControlBlock_t *)pcb2)->arrival;
    if(a == b)
        // if arrival times are equal
        return ((ProcessControlBlock_t *)pcb1)->remaining_burst_time - ((ProcessControlBlock_t *)pcb2)->remaining_burst_time;
    // else return difference of 2 arrival times
    return b - a;
}

int burst_time_calc_helper(const void *pcb1, const void *pcb2) {
    // return remaining burst time difference
    return ((ProcessControlBlock_t *)pcb1)->remaining_burst_time - ((ProcessControlBlock_t *)pcb2)->remaining_burst_time;
}
