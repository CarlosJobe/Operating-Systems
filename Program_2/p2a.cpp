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
#include <iomanip>

using namespace std;

/***********************************************************************
*************************** Global Variables ***************************
***********************************************************************/

constexpr auto EVENT1 = 1;
constexpr auto EVENT2 = 2;
constexpr auto EVENT3 = 3;
constexpr auto EVENT4 = 4;
constexpr auto EVENT5 = 5;
constexpr auto EVENT6 = 6;
constexpr auto EVENT7 = 7;

constexpr auto FCFS_START = 1;
constexpr auto SRTF_START = 2;
constexpr auto RR_START = 3;
constexpr auto CREATION_EVENT = 4;
constexpr auto FCFS_END = 5;
constexpr auto SRTF_END = 6;
constexpr auto RR_END = 7;


int schedAlg;
int avgArrivalRate;
float avgArrivalTime;
float avgServiceTime;
float quantumInterval;
bool end_condition;
float mainTime;       // this should be driven by the events list handler??
float arrivalTime;
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
        return rhs.time < time;
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
int addProcess(struct Process new_process);
int handleEventType1(struct Event e);
int handleEventType2(struct Event e);
int handleEventType3(struct Event e);
int handleEventType4(struct Event e);
int handleEventType5(struct Event e);
int handleEventType6(struct Event e);
int handleEventType7(struct Event e);

//int creationProcess(struct Event eve);
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
void generateEventToCreateProcess(float time);

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
    mainTime = 0.0;
    arrivalTime = 0.0;
    newProcessClock = 0.0;
    pIDCounter = 1;
    avgArrivalTime = 1 / static_cast<float>(avgArrivalRate);
    //cout << "avgArrivalTime = " << avgArrivalTime << endl;

    generateEventToCreateProcess(mainTime);
    //Process p1 = generateProcess();
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
    /*
    so here we read the first item in the eventQueue and determine the event type
    then use that to send the event to the appropriate event processor.

    the individual processors do much of the work
    */

    int loop = 0;
    while (loop < 10)
    {
        Event toRun = eventQueue.top();
        //eventQueue.pop();
        cout << "time=" << fixed << setprecision(7) << toRun.time << " \t\t: type=" << toRun.type << " \t\t: epID=" << toRun.epID << endl;
        switch (toRun.type)
        {
        case EVENT1:
            handleEventType1(toRun);
            break;
        case EVENT2:
            handleEventType2(toRun);
            break;
        case EVENT3:
            handleEventType3(toRun);
            break;
        case EVENT4:
            handleEventType4(toRun);
            break;
        case EVENT5:
            handleEventType5(toRun);
            break;
        case EVENT6:
            handleEventType6(toRun);
            break;
        case EVENT7:
            handleEventType7(toRun);
            break;
            // May need an event 8 that checks remaining time 
            // for SRTF
        default:
            break;
        }
        loop++;
    }

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
    Process p{};
    if (pIDCounter == 1)
    {
        p.arrivalTime = 0.0;
    }
    else
    {
        newProcessClock += (genexp(avgArrivalTime)/100.0f);
        p.arrivalTime = newProcessClock;
        cout << "newProcessClock = " << newProcessClock << endl;
    }
    p.pID = pIDCounter;
    //p.arrivalTime = mainTime;
    p.burstTime = (genexp(avgServiceTime)/100.0f);
    //cout << "avgServiceTime = " << avgServiceTime << endl;
    //cout << "burstTime returned = " << p.burstTime << endl;
    p.remainingTime = p.burstTime;
    p.initialTime = 0.0;
    p.finalTime = 0.0;
    p.turnaroundTime = 0.0;
    p.waitingTime = 0.0;
    p.responseTime = 0.0;

    generateProcessEvents(p);
    addProcess(p);

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
    
    e1.type = schedAlg;
    e1.epID = p.pID;   // will have to change as processes will have to recycle into the event list
    int x1 = schedule_event(e1);
    if (x1)
        cout << "error scheduling event" << endl;

    if (schedAlg == 1)
    {
        // genearate process service complete event
        Event e2{};
        e2.time = p.arrivalTime + p.burstTime;
        e2.type = FCFS_END;        // indicates an end event
        e2.epID = p.pID;
        int x2 = schedule_event(e2);
        if (x2)
            cout << "error scheduling event" << endl;
    }

    if (schedAlg == 3)
    {
        // genearate process service complete event
        Event e2{};
        e2.time = p.arrivalTime + quantumInterval;
        e2.type = RR_END;     
        e2.epID = p.pID;
        int x2 = schedule_event(e2);
        if (x2)
            cout << "error scheduling event" << endl;
    }
}

/***************************************************
********** create 'process creation' event *********
***************************************************/
/*
create events that will in turn create processes
*/
void generateEventToCreateProcess(float t)
{
    Event e1{};
    e1.time = t;
    e1.type = CREATION_EVENT;
    e1.epID = pIDCounter;   
    int x1 = schedule_event(e1);
    if (x1)
        cout << "error scheduling event" << endl;

    // need to accomodate arrival interval
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
**************** add process to queue **************
***************************************************/
/*
add process to appropriate queue
*/
int addProcess(struct Process new_process)
{
    if (schedAlg == 1 || schedAlg == 3)
    {
        processReadyQueue.push(new_process);
    }
    else
        priorityPRQ.emplace(new_process);

    return 0;  // temp to make empty program run, function probably needs to be void

}

/***************************************************
*************** Handle Event Type 1 ****************
***************************************************/
/*
process the first event type
these will be FCFS Start events
*/
int handleEventType1(struct Event e)
{
    //cout << "Handling FCFS Start Event - Type 1" << endl;
    // collecct relevent data for analysis
    eventQueue.pop();
    return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 2 ****************
***************************************************/
/*
process the second event type
these will be SRTF Start events
*/
int handleEventType2(struct Event e)
{
    cout << "Handling SRTF Start Event - Type 2" << endl;
    // collecct relevent data for analysis
    // compare remaining time to burst time of next event?
    // 1 may need to front/pop the process and store it independent
    // of the queue. this allows comparison to the first process
    // in the queue to see if it has less remaining time 
    // 2 may want to scan the event list to see if the next event
    // is to create a process so we can schedule a check at that time
    eventQueue.pop();
    return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 3 ****************
***************************************************/
/*
process the third event type
these will be RR Start events
*/
int handleEventType3(struct Event e)
{
    cout << "Handling RR Start Event - Type 3" << endl;
    // collecct relevent data for analysis
    eventQueue.pop();
    return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 4 ****************
***************************************************/
/*
process the fourth event type
these will be events that trigger process creation
this facilitates event scheduling based on average
arrival time
*/
int handleEventType4(struct Event e)
{
    //create a new process
    Process p1 = generateProcess();
        //create start event
        // if type = 1 create end event <- built into generateProcess();
    cout << "new process ID = " << p1.pID << endl;
    //increment arrivalTime        
    arrivalTime += (genexp(avgArrivalTime) / 100.0f);
    cout << "arrival time = " << arrivalTime << endl;
    //create next creation event
    generateEventToCreateProcess(arrivalTime);
    //delete original process
    eventQueue.pop();
    return 0;
}

/***************************************************
*************** Handle Event Type 5 ****************
***************************************************/
/*
process the fifth event type
these will be FCFS End events
*/
int handleEventType5(struct Event e)
{
    cout << "Handling FCFS End Event - Type 5" << endl;
    // collecct relevent data for analysis
    Process p = processReadyQueue.front();
    mainTime += p.burstTime;
    cout << "mainTime = " << mainTime << endl; 
    eventQueue.pop();
    processReadyQueue.pop();
    return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 6 ****************
***************************************************/
/*
process the sixth event type
these will be SRTF End events
*/
int handleEventType6(struct Event e)
{
    cout << "Handling SRTF End Event - Type 6" << endl;
    return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 7 ****************
***************************************************/
/*
process the seventh event type
these will be RR End events
*/
int handleEventType7(struct Event e)
{
    cout << "Handling RR End Event - Type 7" << endl;
    return 0;  // temp to make empty program run
}

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
        x = (-1.0f / lambda) * log10(u);
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
    cout << "Event Queue" << endl;
    while (!eventQueue.empty())
    {
        Event toPrint = eventQueue.top();
        eventQueue.pop();
        //if (toprint.type % 2)
        //{
        cout << "time=" << toPrint.time << " \t\t: type=" << toPrint.type << " \t\t: epid=" << toPrint.epID << endl;
        //}
    }
    cout << "Process Ready Queue" << endl;
    while (!processReadyQueue.empty())
    {
        Process toPrint = processReadyQueue.front();
        processReadyQueue.pop();
        cout << "Process pID = " << toPrint.pID << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << endl;

    }
    cout << "Priority Process Ready Queue" << endl;
    while (!priorityPRQ.empty())
    {
        Process toPrint = priorityPRQ.top();
        priorityPRQ.pop();
        cout << "SRTF Process pID = " << toPrint.pID << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << endl;
    }
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
        avgServiceTime = static_cast<float>(atof(argv[3]));
        if (argc == 5)
            quantumInterval = static_cast<float>(atof(argv[4]));
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
