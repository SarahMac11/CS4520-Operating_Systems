#include <fcntl.h>
#include <stdio.h>
#include <gtest/gtest.h>
#include <pthread.h>
#include <processing_scheduling.h>
// Using a C library requires extern "C" to prevent function managling
extern "C" 
{
#include <dyn_array.h>
}


#define NUM_PCB 30
#define QUANTUM1 4 // Used for Robin Round for process as the run time limit
#define QUANTUM2 5 // Used for Robin Round for process as the run time limit
extern unsigned int score;
extern unsigned int total;

/*
 * LOAD PROCESS CONTROL BLOCKS TEST CASES
 *
 * dyn_array_t *load_process_control_blocks(const char *input_file); 
 *
 */

// my original test cases without score included
TEST (load_process_control_blocks, NullFilePath) {
    dyn_array_t *da = load_process_control_blocks(NULL);
    ASSERT_EQ(da, (dyn_array_t*) NULL);
}

TEST (load_process_control_blocks, TrickyBadFileName) {
    const char *filename = "";
    dyn_array_t *da = load_process_control_blocks(filename);
    ASSERT_EQ(da, (dyn_array_t*) NULL);
}

TEST (load_process_control_blocks, TrickyBadFileNameNewLine) {
    const char *filename = "\n";
    dyn_array_t *da = load_process_control_blocks(filename);
    ASSERT_EQ(da, (dyn_array_t*) NULL);
}

TEST (load_process_control_blocks, EmptyFile) {
    const char *filename = "JIMRTESTANSWERS.JK";    // wish this was a real file
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    close(fd);
    dyn_array_t *da = load_process_control_blocks(filename);
    ASSERT_EQ(da, (dyn_array_t*) NULL);
}

// begin jimr's tests
TEST (load_process_control_blocks, nullFilePath) 
{
    dyn_array_t* da = load_process_control_blocks (NULL);
    ASSERT_EQ(da,(dyn_array_t*) NULL);

    score+=5;
}

TEST (load_process_control_blocks, notFoundFile) 
{

    dyn_array_t* da = load_process_control_blocks ("NotARealFile.Awesome");
    ASSERT_EQ(da,(dyn_array_t*)NULL);

    score+=5;
}

TEST (load_process_control_blocks, emptyFoundFile) 
{
    const char* fname = "EMPTYFILE.DARN";
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int fd = open(fname, flags, mode);
    close(fd);
    dyn_array_t* da = load_process_control_blocks (fname);
    ASSERT_EQ(da,(dyn_array_t*)NULL);

    score+=5;
}

TEST (load_process_control_blocks, incorrectPCBFoundFile) 
{
    const char* fname = "CANYOUHANDLETHE.TRUTH";
    uint32_t pcb_num = 10;
    uint32_t pcbs[5][3] = {{1,1,1},{2,2,2},{3,3,3},{4,4,4},{5,5,5}};
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int fd = open(fname, flags, mode);
    write(fd,&pcb_num,sizeof(uint32_t));
    write(fd,pcbs,5 * sizeof(uint32_t)*3);
    close(fd);
    dyn_array_t* da = load_process_control_blocks (fname);
    ASSERT_EQ(da,(dyn_array_t*)NULL);

    score+=5;
}

TEST (load_process_control_blocks, fullFoundFile) 
{
    const char* fname = "PCBs.bin";
    uint32_t pcb_num = NUM_PCB;
    uint32_t pcbs[NUM_PCB][3];
    for (uint32_t p = 0; p < pcb_num; ++p) 
    {
        pcbs[p][0] = rand() % 35 + 1;
        //        printf("%d, ", pcbs[p][0]);
        pcbs[p][1] = p;
        pcbs[p][2] = p;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int fd = open(fname, flags, mode);
    write(fd,&pcb_num,sizeof(uint32_t));
    write(fd,pcbs,pcb_num * sizeof(uint32_t)*3);
    close(fd);
    dyn_array_t* da = load_process_control_blocks (fname);
    ASSERT_NE(da, (dyn_array_t*) NULL);
    for (size_t i = 0; i < dyn_array_size(da); ++i) 
    {
        ProcessControlBlock_t * pPCB = (ProcessControlBlock_t *)dyn_array_at(da, i);
        ASSERT_EQ(pPCB->remaining_burst_time, pcbs[i][0]);
        ASSERT_EQ(pPCB->priority, pcbs[i][1]);
        ASSERT_EQ(pPCB->arrival, pcbs[i][2]);
    }
    dyn_array_destroy(da);

    score+=10;
}


/*
 * * First Come First Served  TEST CASES
 *
 * bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result);
 *
 */

// my original test cases without score included
TEST (first_come_first_serve, NullReadyQueue) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t *da = NULL;
    bool res = first_come_first_serve(da, sr);
    ASSERT_EQ(false, res);
    delete sr;
}

TEST (first_come_first_serve, NullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t* da = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool res = first_come_first_serve(da, sr);
    ASSERT_EQ(false, res);
    dyn_array_destroy(da);
}

// begin jimr's tests
TEST (first_come_first_serve, nullInputProcessControlBlockDynArray) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = first_come_first_serve (pcbs,sr);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;
}

TEST (first_come_first_serve, nullScheduleResult) 
{
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = first_come_first_serve (pcbs,sr);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;
}

TEST (first_come_first_serve, goodInputA) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[3] = 
    {
        [0] = {24,2,0,0},
        [1] = {3,3,0,0},
        [2] = {3,1,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = first_come_first_serve (pcbs,sr);	
    ASSERT_EQ(true,res);
    float answers[3] = {27,17,30};
    ASSERT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}

TEST (first_come_first_serve, goodInputB) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[4] = 
    {
        [0] = {6,3,0,0},
        [1] = {8,2,0,0},
        [2] = {7,4,0,0},
        [3] = {3,1,0,0},
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);		
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = first_come_first_serve (pcbs,sr);	
    ASSERT_EQ(true,res);
    float answers[3] = {16.25,10.25,24};
    ASSERT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}



/*
 * * Shortest Job First  TEST CASES
 *
 * bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result);
 * 
 */

// my orignal test cases without score included

TEST (shortest_job_first, NullReadyQueue) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t *da = NULL;
    bool res = shortest_job_first(da, sr);
    ASSERT_EQ(false, res);
    delete sr;
}

TEST (shortest_job_first, NullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t *da = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool res = shortest_job_first(da, sr);
    ASSERT_EQ(false, res);
    dyn_array_destroy(da);
}

// begin jimr's tests
TEST (shortest_job_first, nullInputProcessControlBlockDynArray) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = shortest_job_first (pcbs,sr);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;
}

TEST (shortest_job_first, nullScheduleResult) 
{
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = shortest_job_first (pcbs,sr);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;
}

TEST (shortest_job_first, goodInput) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[4] = 
    {
        [0] = {25,2,0,0},
        [1] = {2,3,1,0},
        [2] = {4,1,2,0},
        [3] = {1,4,3,0},
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = shortest_job_first (pcbs,sr);	
    ASSERT_EQ(true,res);
    float answers[3] = {26.25,18.25,32};
    ASSERT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}


/*
 * * Shortest Remaining Time First  TEST CASES
 *
 * bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result);
 *
 */

// my tests cases without score included

TEST (shortest_remaining_time_first, NullReadyQueue) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t *da = NULL;
    bool res = shortest_remaining_time_first(da, sr);
    ASSERT_EQ(false, res);
    delete sr;
}

TEST (shortest_remaining_time_first, NullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t *da = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool res = shortest_remaining_time_first(da, sr);
    ASSERT_EQ(false, res);
    dyn_array_destroy(da);
}

// begin jimr's tests
TEST (shortest_remaining_time_first, nullInputProcessControlBlockDynArray) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = shortest_remaining_time_first (pcbs,sr);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;
}

TEST (shortest_remaining_time_first, nullScheduleResult) 
{
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = shortest_remaining_time_first (pcbs,sr);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;
}

TEST (shortest_remaining_time_first, goodInput) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[4] = 
    {
        [0] = {25,2,0,0},
        [1] = {2,3,1,0},
        [2] = {4,1,2,0},
        [3] = {1,4,3,0},
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = shortest_remaining_time_first (pcbs,sr);	
    ASSERT_EQ(true,res);
    float answers[3] = {10.25,2.25,32};
    ASSERT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}



/*
 * ROUND ROBIN TEST CASES
 *
 * bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum);
 *
 */

// my test cases without score included

TEST (round_robin, NullReadyQueue) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t *da = NULL;
    bool res = round_robin(da, sr, QUANTUM1);
    ASSERT_EQ(false, res);
    dyn_array_destroy(da);
}

TEST (round_robin, NullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t *da = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool res = round_robin(da, sr, QUANTUM1);
    ASSERT_EQ(false, res);
    dyn_array_destroy(da);
}

// begin jimr's tests
TEST (round_robin, nullInputProcessControlBlockDynArray) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = round_robin (pcbs,sr,QUANTUM1);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;
}

TEST (round_robin, nullScheduleResult) 
{
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = round_robin (pcbs,sr,QUANTUM1);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;

}

TEST (round_robin, goodInputA) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[3] = 
    {
        [0] = {24,4,0,0},
        [1] = {3,2,0,0},
        [2] = {3,1,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = round_robin (pcbs,sr,QUANTUM1);	
    ASSERT_EQ(true,res);
    float answers[3] = {15.666667,5.666667,30};
    ASSERT_FLOAT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_FLOAT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}

TEST (round_robin, goodInputB) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[4] = 
    {
        [0] = {20,1,0,0},
        [1] = {5,2,0,0},
        [2] = {6,3,0,0},
        [3] = {11,4,14,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);     
    dyn_array_push_back(pcbs,&data[0]); 
    bool res = round_robin (pcbs,sr,QUANTUM2);  
    ASSERT_EQ(true,res);
    float answers[3] = {26.25,15.75,42};
    ASSERT_FLOAT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
} 


class GradeEnvironment : public testing::Environment 
{
    public:
        virtual void SetUp() 
        {
            score = 0;
            total = 190;
        }

        virtual void TearDown() 
        {
            ::testing::Test::RecordProperty("points_given", score);
            ::testing::Test::RecordProperty("points_total", total);
            std::cout << "SCORE: " << score << '/' << total << std::endl;
        }
};

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
    return RUN_ALL_TESTS();
}
