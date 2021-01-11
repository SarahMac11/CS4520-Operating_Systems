# Assignment 3

Milestone 0: CMake and Unit Testing filled out 

Milestone 1: PCB file loading and First Come First Served 

Milestone 2: Shortest Job First, Shortest Remaining Time First, Round Robin, and analysis of algorithms 

Note: 
You can manually copy the time analysis from console and paste it to this readme file, but directly output from your program is strongly recommended.     
---------------------------------------------------------------------------
Add your scheduling algorithm analysis below this line in a readable format. 
---------------------------------------------------------------------------

NOTE:
I was unsure of the P Scheduling Algorithm since we did not implement Priority and P for Print did not seem to make sense since each scheduled algorithm is printed regardless so I made P = PSRTF (Preemtive Shortest Remaining Time First)...I will take whatever extra credit you think is fitting XD (just kidding...but also not)



| Scheduling Algorithm | Average Turnaround Time | Average Waiting Time | Total Clock Time |
|----------------------|-------------------------|----------------------|------------------|
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 5      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 6      | 18.750000               | 11.250000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| RR: Quantum = 2      | 21.000000               | 13.500000            | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 9      | 16.500000               | 9.000000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 5      | 20.500000               | 13.000000            | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 5      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 12     | 16.500000               | 9.000000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| RR: Quantum = 12     | 16.500000               | 9.000000             | 30               |
| RR: Quantum = 12     | 16.500000               | 9.000000             | 30               |
| RR: Quantum = 2      | 21.000000               | 13.500000            | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| SJF                  | 15.750000               | 8.250000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
| RR: Quantum = 4      | 20.500000               | 13.000000            | 30               |
| RR: Quantum = 22     | 16.500000               | 9.000000             | 30               |
| RR: Quantum = 1      | 21.250000               | 13.750000            | 30               |
| FCFS                 | 15.000000               | 7.500000             | 30               |
| PSRJF                | 14.500000               | 7.000000             | 30               |
