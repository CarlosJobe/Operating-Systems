/***********************************************************************
****************************** CS4328.004 ******************************
****************************** Program #2 ******************************
*********************** Carlos Jobe and Tyler Fain *********************
**************************** 8 November 2021 ***************************
***********************************************************************/

#include <iostream>
//#include <chrono>
#include <array>
#include <math.h>
#include <fstream>
#include <queue>
#include <random>
#include <ctime>
#include <stdlib.h>

using namespace std;

/***********************************************************************
*************************** Global Variables ***************************
***********************************************************************/

constexpr auto EVENT1 = 1;
constexpr auto EVENT2 = 2;

int schedAlg;
int avgArrivalRate;
float avgArrivalTime;
float avgServiceTime;
float quantumInterval;
bool end_condition;
//struct event* head;
float time_clock;       // this should be driven by the events list handler??
//float arrivalTimeClock;
float newProcessClock;

int num_processes = 10;
int totalTurnaround = 0;
int totalWaiting = 0;
int totalResponse = 0;
int totalIdle = 0;
float throughput;
float avgTurnaround;
float avgWaiting;
float avgResponse;
float utilization;

int pIDCounter;

struct Event {
    float time;
    int type;       // maybe start of process, end of process, 
    // maybe type 1 = FCFS start, 2= RR (re)start, 3 = SRTF (re)start
    int epID;
    bool operator < (const Event rhs) const
    {
        return time < rhs.time;
    }
    // add more fields
    //struct event* next;  // dont think we need this since we are using queues....
};

struct Process {
    int pID;
    float arrivalTime;
    float burstTime;
    float remainingTime;   // will need to set this equal to burstTime at process creation
    float initialTime;    // is this the time that it first starts being processed by the CPU?
    float finalTime;
    float turnaroundTime;
    float waitingTime;
    float responseTime;
    //int rrCounter;

    bool operator<(const Process& a) const
    {
        if (remainingTime == a.remainingTime)
        {
            return pID < a.pID;
        }
        else
        {
            return remainingTime < a.remainingTime;
        }
    }
};

priority_queue<Event> eventQueue;
queue<Process> processReadyQueue;       // for FCFS and RR
priority_queue<Process> priorityPRQ;    // Process ready queue for SRTF


/***********************************************************************
****************************** Prototypes *****************************
***********************************************************************/

void init();
int run_sim();
void generate_report();
int schedule_event(struct Event new_event);
//int process_event1(struct Event* eve);
//int process_event2(struct Event* eve);
bool commandLineInput(int argc, char* argv[]);
void commandLineInstructions(int i);
//void fcfs(struct process, int num_processes);
//void srtf();
//void rr();
float urand();
float genexp(float lambda);
//struct Process p[10];
struct Process generateProcess();
void generateProcessEvents(struct Process p);

/***********************************************************************
********************************* Main *********************************
***********************************************************************/
int main(int argc, char* argv[])
{

    // parse arguments
    bool exiting = commandLineInput(argc, argv);
    if (exiting)
    {
        cout << "\n\t*** exiting program because of bad command line input ***" << endl;
        return 0;
    }

    init();
    run_sim();
    generate_report();


    while (!eventQueue.empty())
    {
        Event toPrint = eventQueue.top();
        eventQueue.pop();
        if (toPrint.type % 2)
        {
            cout << "time=" << toPrint.time << " \t: type=" << toPrint.type << " \t: epID=" << toPrint.epID << endl;
        }
    }
    cout << "\n\t*** exiting program normally ***" << endl;
    return 0;
}

/***********************************************************************
******************************* Functions ******************************
***********************************************************************/


/***************************************************
************************ FCFS **********************
***************************************************/
/*
First Come First Served scheduling algorithm
*/
/*
void fcfs(struct Process, int num_processes)
{
    /*
    for(int i = 0; i < num_processes; i++)
    {
        cout << "Entere arrival time " << i+1 << ": ";
        cin >> p[i].arrivalTime;
        cout << "Enter burst time of process " << i+1 << ": ";
        cin >> p[i].burstTime;
        p[i].pID = i+1;
        cout << endl;
    }
     // removed end block comment
    for (int i = 0; i < num_processes; i++)
    {
        p[i].initialTime = (i == 0) ? p[i].arrivalTime : max(p[i - 1].finalTime, p[i].arrivalTime);
        p[i].finalTime = p[i].initialTime + p[i].burstTime;
        p[i].turnaroundTime = p[i].finalTime - p[i].arrivalTime;
        p[i].waitingTime = p[i].turnaroundTime - p[i].burstTime;
        p[i].responseTime = p[i].initialTime - p[i].arrivalTime;

        totalTurnaround += p[i].turnaroundTime;
        totalWaiting += p[i].waitingTime;
        totalResponse += p[i].responseTime;
        totalIdle += (i == 0) ? (p[i].arrivalTime) : (p[i].initialTime - p[i - 1].finalTime);
    }

    avgTurnaround = (double)totalTurnaround / num_processes;
    avgWaiting = (double)totalWaiting / num_processes;
    avgResponse = (double)totalResponse / num_processes;
    utilization = ((p[num_processes - 1].finalTime - totalIdle) / (double)p[num_processes - 1].finalTime) * 100;
    throughput = double(num_processes) / (p[num_processes - 1].finalTime - p[0].arrivalTime);

}*/


/***************************************************
************************ Init **********************
***************************************************/
/*
initialize all variable, states, and end conditions
schedule first events
*/
void init()
{
    end_condition = false;
    time_clock = 0.0;
    newProcessClock = 0.0;
    pIDCounter = 1;
    avgArrivalTime = 1 / static_cast<float>(avgArrivalRate);
    cout << "avgArrivalTime = " << avgArrivalTime << endl;


    Process p1 = generateProcess();
    //if/else to appropriate queue
    //schedule_event(p1);
    

}

/***************************************************
*********************** run_sim ********************
***************************************************/
/*
run the actual simulation
*/
int run_sim()
{
    for (int i = 0; i < 10; i++)
    {
        Process p1 = generateProcess();
        //if/else to appropriate queue
    }


   /*{
        eve = head;
        time_clock = eve->time;
        switch (eve->type)
        {
        case EVENT1:
            process_event1(eve);
            break;
        case EVENT2:
            process_event2(eve);
            break;

            // add more events

        //default:
            // error
        }

        head = eve->next;
        free(eve);
        eve = NULL;
    }*/
    return 0;
}

/***************************************************
****************** generate process ****************
***************************************************/
/*
generate a process for the process ready queue
*/
struct Process generateProcess()
{

    //**************************************************************************************************************************
    Process p{};
    if (pIDCounter == 1)
    {
        p.arrivalTime = 0.0;
    }
    else
    {
        newProcessClock += (genexp(avgArrivalTime)/100.0);
        p.arrivalTime = newProcessClock;
        cout << "newProcessClock = " << newProcessClock << endl;
    }
    p.pID = pIDCounter;
    //p.arrivalTime = time_clock;
    p.burstTime = (genexp(avgServiceTime)/100.0);
    //cout << "avgServiceTime = " << avgServiceTime << endl;
    //cout << "burstTime returned = " << p.burstTime << endl;
    p.remainingTime = p.burstTime;
    p.initialTime = 0.0;
    p.finalTime = 0.0;
    p.turnaroundTime = 0.0;
    p.waitingTime = 0.0;
    p.responseTime = 0.0;

    generateProcessEvents(p);

    pIDCounter++;
    return p;
}

/***************************************************
************** generate process arrival events *************
***************************************************/
/*
generate the process events for a process
*/
void generateProcessEvents(struct Process p)
{
    Event e1{};
    e1.time = p.arrivalTime;
    e1.type = 1;
    e1.epID = p.pID;   // will have to change as processes will have to recycle into the event list
    int x1 = schedule_event(e1);
    if (x1)
        cout << "error scheduling event" << endl;

    
    // genearate process service complete event
    Event e2{};
    e2.time = p.arrivalTime + p.burstTime;
    e2.type = 2;
    e2.epID = p.pID;
    int x2 = schedule_event(e2);
    if (x2)
        cout << "error scheduling event" << endl;
    
}

/***************************************************
******************* schedule event *****************
***************************************************/
/*
insert event into the event queue in its order of time
*/
int schedule_event(struct Event new_event)
{
    //Event* e = new_event;

    eventQueue.emplace(new_event);
    return 0;  // temp to make empty program run
}


/***************************************************
****************** process event 1 *****************
***************************************************/
/*
process the first event
*** These will be used to handle the different types of events in the event queue ***
*/
/*int process_event1(struct Event* eve)
{
    return 0;  // temp to make empty program run
}*/

/***************************************************
****************** process event 2 *****************
***************************************************/
/*
process the second event
*/
/*int process_event2(struct Event* eve)
{
    return 0;  // temp to make empty program run
}*/

/***************************************************
*********************** urand **********************
***************************************************/
/*
returns a random number between 0 and 1
*/
float urand()
{
    mt19937 rng;
    uniform_real_distribution<float> dist(0.0, 1.0);  //(min, max)
    rng.seed(random_device{}()); //non-deterministic seed
    return dist(rng);
}

/***************************************************
********************** genexp **********************
***************************************************/
/*
returns a random number that follows an exp distribution
*/
float genexp(float lambda)
{
    float u, x;
    x = 0;
    
    while (x == 0)
    {
        u = urand();
        x = (-1.0 / lambda) * log10(u);
    }
    return(x);
}

/***************************************************
******************* generate report ****************
***************************************************/
/*
output statistics
*/
void generate_report()
{

}

/***************************************************
***************** Command Line Input ***************
***************************************************/
/*
Convert the command line input to the necessary
data types and report an error if presented with
insufficient data or data out of range when applicable
*/
bool commandLineInput(int argc, char* argv[])
{
    bool improperInput = false;
    if (argc < 4 || argc > 5)
    {
        commandLineInstructions(0);
        improperInput = true;
    }
    else
    {
        schedAlg = atoi(argv[1]);
        avgArrivalRate = atoi(argv[2]);
        avgServiceTime = atof(argv[3]);
        if (argc == 5)
            quantumInterval = atof(argv[4]);
        else
            quantumInterval = 0.0;
    }
    if (schedAlg < 1 || schedAlg > 3)
    {
        commandLineInstructions(1);
        improperInput = true;
    }
    if (avgArrivalRate < 1 || avgArrivalRate >30)
    {
        commandLineInstructions(2);
        improperInput = true;
    }
    if (argc == 4 && atoi(argv[1]) == 3)
    {
        commandLineInstructions(3);
        improperInput = true;
    }
    return improperInput;
}

/***************************************************
************* Command Line Instructions ************
***************************************************/
/*
Instructions clarifying the necessary command line
inputs and their value ranges if applicable
*/
void commandLineInstructions(int i)
{
    if (i == 0)
    {
        cout << "Command Line Arguments Are Necessary" << endl;
        cout << "Use the following format:" << endl;
        cout << "\n./simulator.out <Algorithm#> <Arrival> <Service> <Quantum>\n" << endl;
        cout << "Where:" << endl;
        cout << "<Algorithm#> indicates the desired scheduling algorithm as follows:" << endl;
        cout << "\t1 - First-Come First-Served (FCFS) [non-preemptive]" << endl;
        cout << "\t2 - Shortest Remaining Time First (SRTF) [preemptive by comparison of run time left]" << endl;
        cout << "\t3 - Round Robin (RR) [preemptive by quantum]" << endl;
        cout << "<Arrival> indicates the average arrival rate in processes per second" << endl;
        cout << "\tfrom 1 process per second to 30 processes per second" << endl;
        cout << "<Service> indicates the average service time" << endl;
        cout << "\tin hundredths of a second (0.01 sec resolution)" << endl;
        cout << "<Quantum> indicates the quantum interval for Round Robin scheduling" << endl;
        cout << "\tFor RR scheduling this value is in milliseconds (0.001 sec resolution)" << endl;
        cout << "\tYou can leave this blank for Algorithm #1 or #2 as it will be ignored" << endl;
    }
    if (i == 1)
    {
        cout << "\n\t*** <Algorithm#> must be a value from 1 to 3 ***\n" << endl;
        commandLineInstructions(0);
    }
    if (i == 2)
    {
        cout << "\n\t*** <Arrival> must be a value from  1 to 30 ***\n" << endl;
        commandLineInstructions(0);
    }
    if (i == 3)
    {
        cout << "\n\t*** <Quantum> is required for Algorithm #3 - Round Robbin (RR) ***\n" << endl;
        commandLineInstructions(0);
    }
}
