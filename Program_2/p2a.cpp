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
float arrivalTime;      // increments to track arrival time for event queue
float arrivalInterval;  // value based on avgArrivalTime and exponential calc. used for incrementing arrival time
float newProcessClock;
float calculatedTurnaround;

int num_processes = 50;
float totalTurnaround = 0;
int totalWaiting = 0;
int totalResponse = 0;
int totalIdle = 0;
int totalTime = 0;
float throughput;
float avgTurnaround;
float avgWaiting;
float avgResponse;
float utilization;

int pIDCounter;
int eventPIDCounter;

//bool srtfInProcess;
//float srtfPremptTime;

struct Event {
    float time;
    int type;       // maybe start of process, end of process, 
    // maybe type 1 = FCFS start, 2= RR (re)start, 3 = SRTF (re)start
    int epID;
    int procID;
    bool operator < (const Event rhs) const
    {
        //return rhs.time < time;
        return rhs.epID < epID;
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
    float idleTime;
    //int rrCounter;

    bool operator<(const Process& a) const
    {
        if (remainingTime == a.remainingTime)
        {
            return a.pID < pID;
        }
        else
        {
            return a.remainingTime < remainingTime;
        }
    }
};

priority_queue<Event> eventQueue;
deque<Process> processReadyQueue;       // for FCFS and RR
priority_queue<Process> priorityPRQ;    // Process ready queue for SRTF
queue<Process> valuesQueue;
queue<Event> completedEventQueue;
queue<Process> newProcessQueue;
queue<Event> newEventQueue;


/***********************************************************************
****************************** Prototypes *****************************
***********************************************************************/

void init();
int run_sim();
void generate_report();
int schedule_event(struct Event new_event);
int addProcess(struct Process new_process);
void handleEventType1(struct Event e);
void handleEventType2(struct Event e);
void handleEventType3(struct Event e);
void handleEventType4(struct Event e);
void handleEventType5(struct Event e);
void handleEventType6(struct Event e);
void handleEventType7(struct Event e);

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
void runStatistics();

void printTopProcess();

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
    //runStatistics();
    generate_report();

    cout << "\n\t*** exiting program normally ***" << endl;
    return 0;
}

/***********************************************************************
******************************* Functions ******************************
***********************************************************************/

void printTopProcess()
{
    cout << "pID \t\t burst \t\t remaining" << endl;
    if (schedAlg == 2 && !priorityPRQ.empty())
    {
        Process p2 = priorityPRQ.top();
        cout << p2.pID << " \t\t " << p2.burstTime << " \t " << p2.remainingTime << endl;
    }
    if ((schedAlg == 1 && !processReadyQueue.empty()) || (schedAlg == 3 && !processReadyQueue.empty()))
    {
        Process p1 = processReadyQueue.front();
        cout << p1.pID << " \t\t " << p1.burstTime << " \t " << p1.remainingTime << endl;
    }
}


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
    mainTime = 0.0f;
    arrivalTime = 0.0f;
    arrivalInterval = 0.0f;
    newProcessClock = 0.0f;
    pIDCounter = 1;
    eventPIDCounter = 1;
    avgArrivalTime = 1.0f / static_cast<float>(avgArrivalRate);
    //cout << "avgArrivalTime = " << avgArrivalTime << endl;
    //srtfInProcess = false;
    //srtfPremptTime = 0.0;

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

    //int loop = 0;
    while (valuesQueue.size() < static_cast<float>(num_processes))
    {
        //cout << "switch loop # " << loop << endl;
        //generate_report();
        Event toRun = eventQueue.top();
        completedEventQueue.push(toRun);
        eventQueue.pop();

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
        //loop++;
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
        //cout << "newProcessClock = " << newProcessClock << endl;
    }
    p.pID = pIDCounter;
    p.burstTime = (genexp(avgServiceTime)/100.0f);
    p.remainingTime = p.burstTime;
    p.initialTime = 0.0f;
    p.finalTime = 0.0f;
    p.turnaroundTime = 0.0f;
    p.waitingTime = 0.0f;
    p.responseTime = 0.0f;

    generateProcessEvents(p);
    addProcess(p);
    newProcessQueue.push(p);
    //cout << " -- added new process # " << p.pID << " to the queue" << " \t: arrival=" << p.arrivalTime << " \t: burst=" << p.burstTime << endl;



    pIDCounter++;
    //generate_report();
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
    e1.epID = eventPIDCounter; 
    e1.procID = p.pID;
    eventPIDCounter++;
    newEventQueue.push(e1);
    int x1 = schedule_event(e1);
    if (x1)
        cout << "error scheduling event" << endl;
    

    if (schedAlg == 1)
    {
        // genearate process service complete event
        Event e2{};
        e2.time = p.arrivalTime + p.burstTime;
        e2.type = FCFS_END;        // indicates an end event
        e2.epID = eventPIDCounter;
        e2.procID = p.pID;
        eventPIDCounter++;
        newEventQueue.push(e2);
        int x2 = schedule_event(e2);
        if (x2)
            cout << "error scheduling event" << endl;
        
    }

    /*if (schedAlg == 2)
    {
        // genearate process service complete event
        Event e2{};
        e2.time = p.arrivalTime + p.burstTime;
        e2.type = SRTF_END;
        e2.epID = eventPIDCounter;
        e2.procID = p.pID;
        eventPIDCounter++;
        int x2 = schedule_event(e2);
        if (x2)
            cout << "error scheduling event" << endl;
    }*/

    if (schedAlg == 3)
    {
        // genearate process service complete event
        Event e2{};
        e2.time = p.arrivalTime + quantumInterval;
        e2.type = RR_END;     
        e2.epID = eventPIDCounter;
        e2.procID = p.pID;
        eventPIDCounter++;
        newEventQueue.push(e2);
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
    e1.epID = eventPIDCounter;
    e1.procID = 0;
    eventPIDCounter++;
    newEventQueue.push(e1);
    int x1 = schedule_event(e1);
    if (x1)
        cout << "error scheduling event" << endl;
    //cout << "new event # " << e1.epID << " type " << e1.type << " at time " << t << endl;

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
    //cout << " - - new event # " << new_event.epID << " type " << new_event.type << " at time " << new_event.time << endl;
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
        processReadyQueue.push_back(new_process);
        //processReadyQueue.push(new_process);
        //cout << " added process # " << new_process.pID << " to queue" << endl;

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
void handleEventType1(struct Event e)
{
    Process p = processReadyQueue.front();
    processReadyQueue.pop_front();
    //processReadyQueue.pop();
    // timekeeping 
    //cout << "event#" << e.epID << " : "
    //cout << "process #" << p.pID << " : mainTime=" << mainTime << " : initialTime=" << p.initialTime << endl;
    if (mainTime < e.time)
    {
        //cout << "inside idle time loop" << endl;
        float idleTime = e.time - mainTime;
        p.idleTime = idleTime;
        mainTime += idleTime;
    }
    if (p.initialTime == 0.0f)
    {
        //cout << "inside initial time correction loop" << endl;
        p.initialTime = mainTime;
    }
    //if (mainTime > e.time)
    //{

    //}

    // statistics stuff here


    processReadyQueue.push_front(p);
    //queue<Process> tempSwap;
    //tempSwap.push(p);
    //while (!processReadyQueue.empty())
    //{
    //    Process t = processReadyQueue.front();
    //    processReadyQueue.pop();
    //    tempSwap.push(t);
    //}

    //processReadyQueue = tempSwap;


    //cout << "  Process Ready Queue" << endl;

    //while (!tempSwap.empty())
    //{
    //    Process toPrint = tempSwap.front();
    //    //processReadyQueue.pop_front();
    //    tempSwap.pop();
    //    cout << "  pID = " << toPrint.pID << "\t: initial=" << toPrint.initialTime << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << " \t: remaining=" << toPrint.remainingTime << " \t: idle=" << toPrint.idleTime << endl;
    //}


    //return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 2 ****************
***************************************************/
/*
process the second event type
these will be SRTF Start events

need to compare remaining process time to arrivalTime, next event start, and next process remaining time

if current process is shorter, update process values, push process back into queue, then...
if next process was shorter, schedule another T2
if next event/arrivalTime was sooner, the process event/arrivalTime

need to check event list when process completes too. maybe schedule another T2?

move push to queue from T6 to here, really move everything
.
*/
void handleEventType2(struct Event e)
{
    //Need to check if priorityPRQ is empty and if so move to the next event
    if (!priorityPRQ.empty())
    {

        /*
        starting new concept.... **********************************/
        Event tempE = eventQueue.top();
        //cout << "pulled reference event ID = " << tempE.epID << " for process # " << tempE.procID << "from event queue" << endl;
        int tempType;
        Process currentP = priorityPRQ.top();
        priorityPRQ.pop();
        if (currentP.initialTime == 0.0)
            currentP.initialTime = mainTime;
        if (!priorityPRQ.empty())
            Process tempP = priorityPRQ.top();
        //cout << "  #  compare e.time(" << e.time << ") + currentP.remainingTime(" << currentP.remainingTime << ") with tempE.time(" << tempE.time << ")" << endl;
        //cout << "  #  if " << (e.time + currentP.remainingTime) << " > " << tempE.time << " then T2A;  otherwise T2B" << endl;
        if (!priorityPRQ.empty() && (((e.time + currentP.remainingTime) > tempE.time) ||
            ((e.time + currentP.remainingTime) > arrivalTime)))  // **T2A** if next event is before end of current event 
        {
            float tempTime;
            //cout << " ~~ T2A-process # " << currentP.pID << " - tempE.time:" << tempE.time << " - e.time:" << e.time << " = " << (tempE.time - e.time) << endl;
            if (tempE.time <= arrivalTime)      // next scheduled event is before next T4 creation
            {
                tempTime = (tempE.time - e.time);
                currentP.remainingTime -= tempTime;      // decrement remaining time
                priorityPRQ.emplace(currentP);                         // add statistics here
                mainTime += tempTime;
                //cout << "return process # " << tempP.pID << " to the queue" << endl;
                //if (currentP.remainingTime > 0)
                tempType = SRTF_START;
                //else
                    //tempType = SRTF_END;
            }
            else    // if (arrivalTime < tempE.time)
            {
                tempTime = (arrivalTime - e.time);
                currentP.remainingTime -= tempTime;
                currentP.arrivalTime = arrivalTime;      // double check this too
                priorityPRQ.emplace(currentP);             // add statistics here
                mainTime += tempTime;
                //cout << "return process # " << tempP.pID << " to the queue" << endl;
                //if (currentP.remainingTime > 0)
                tempType = SRTF_START;
                //else
                    //tempType = SRTF_END;
            }
            // create new start event
            Event tempE2{};
            tempE2.time = currentP.arrivalTime + tempTime; // different tempTime...
            tempE2.type = tempType;
            tempE2.epID = eventPIDCounter;
            tempE2.procID = currentP.pID;
            eventPIDCounter++;
            newEventQueue.push(tempE2);
            int E2 = schedule_event(tempE2);


        }
        else
        {
           // cout << " ~~ T2B-process # " << currentP.pID << " - tempE.time:" << tempE.time << " - e.time:" << e.time << " = " << (tempE.time - e.time) << endl;
            float tempTime = currentP.remainingTime;
            currentP.finalTime = tempTime;
            currentP.remainingTime = 0.0;      // decrement remaining time

            //priorityPRQ.emplace(currentP);                         // add statistics here
            mainTime += tempTime;
            //cout << "return process # " << tempP.pID << " to the queue" << endl;
            tempType = SRTF_START; //SRTF_END;
            // ******* need to "generateProcessEvents" here ****

            // genearate process service complete event
            Event end{};
            end.time = e.time + tempTime;
            end.type = tempType;
            end.epID = eventPIDCounter;
            end.procID = e.epID;
            eventPIDCounter++;
            newEventQueue.push(end);
            int x2 = schedule_event(end);
            if (x2)
                cout << "error scheduling event" << endl;

        }
        valuesQueue.push(currentP);  // why is this here?
    }
    //return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 3 ****************
***************************************************/
/*
process the third event type
these will be RR Start events
*/
void handleEventType3(struct Event e)
{
    Process p = processReadyQueue.front();
    processReadyQueue.pop_front();
    if(p.initialTime == 0.0)
        p.initialTime = mainTime;

    // statistics stuff here


    processReadyQueue.push_front(p);
    //return 0;  // temp to make empty program run
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
void handleEventType4(struct Event e)
{

    //create a new process
    Process p1 = generateProcess();
        //create start event
        // if type = 1 create end event <- built into generateProcess();
    //cout << "new process ID = " << p1.pID << endl;
    //increment arrivalTime        
    
    //cout << "arrival time = " << arrivalTime << endl;
    //create next creation event
    generateEventToCreateProcess(arrivalTime);
    //delete original process
    //eventQueue.pop();
    arrivalInterval = (genexp(avgArrivalTime) / 100.0f);
    arrivalTime += arrivalInterval;
    //return 0;
}

/***************************************************
*************** Handle Event Type 5 ****************
***************************************************/
/*
process the fifth event type
these will be FCFS End events
*/
void handleEventType5(struct Event e)
{
    //cout << "Handling FCFS End Event - Type 5" << endl;
    // collecct relevent data for analysis
    Process p = processReadyQueue.front();
    processReadyQueue.pop_front();
    //processReadyQueue.pop();
    mainTime += p.burstTime;
    //cout << "mainTime = " << mainTime << endl; 
    //Event tempE = eventQueue.top();
    //if (mainTime < )
    //eventQueue.pop();

    //stats
    p.finalTime = p.initialTime + p.arrivalTime;
    p.turnaroundTime = p.finalTime - p.burstTime;
    p.waitingTime = p.turnaroundTime - p.burstTime;
    valuesQueue.push(p);

    //return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 6 ****************
***************************************************/
/*
process the sixth event type
these will be SRTF End events
*/
void handleEventType6(struct Event e)
{
    Process p = priorityPRQ.top();
    valuesQueue.push(p);
    priorityPRQ.pop();
    //cout << "~  ~removed process id # " << p.pID << " from the queue" << endl;
    mainTime = e.time;
    //generate_report();
    //eventQueue.pop();
    //srtfInProcess = false;
    //cout << "still need to process SRTF end" << endl;

    //return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 7 ****************
***************************************************/
/*
process the seventh event type
these will be RR End events
*/
void handleEventType7(struct Event e)
{
    //cout << "Handling RR End Event - Type 7" << endl;

    Process p = processReadyQueue.front();
    processReadyQueue.pop_front();
    // stats variables
    //float tempTurnaround;

    //eventQueue.pop();
    if (p.remainingTime >= quantumInterval)
    {
        mainTime += quantumInterval; 
        p.remainingTime -= quantumInterval;
        p.turnaroundTime += quantumInterval;
        //cout << "mainTime = " << mainTime << endl;
        //cout << "reduced process # " << p.pID << " remaining time by " << quantumInterval << " to " << p.remainingTime << endl;
        //cout << "returned process # " << p.pID << " to the queue" << endl;
        processReadyQueue.push_back(p);
    }
    else
    {
        mainTime += p.remainingTime;
        p.turnaroundTime += p.remainingTime;
        p.remainingTime = 0.0f;
        valuesQueue.push(p);
       // cout << "mainTime = " << mainTime << endl;
        //cout << " *** process # " << p.pID << " has completed ***" <<  endl;
    }
;

    
    //return 0;  // temp to make empty program run
}

/***************************************************
****************** run statistics ******************
***************************************************/
/*
perform statical analysis on completed processes
*/
void runStatistics()
{
    
    while (!valuesQueue.empty())
    {
        Process p = valuesQueue.front();
        valuesQueue.pop();
        
        
        totalTurnaround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
        totalTime += p.finalTime - p.initialTime;
    }
    
    float calculatedTurnaround = totalTurnaround / static_cast<float>(num_processes);
    float calculatedWaiting = totalWaiting / static_cast<float>(num_processes);
    //float CPU_Utilization will need to calculate idle time
    float throughput = static_cast<float>(num_processes) / totalTime;
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
    cout << "  QUEUE REPORT" << endl;
    cout << "  Event Queue" << endl;
    while (!eventQueue.empty())
    {
        Event toPrint = eventQueue.top();
        eventQueue.pop();
        //if (toprint.type % 2)
        //{
        cout << "  time=" << toPrint.time << " \t\t: type=" << toPrint.type << " \t\t: epid=" << toPrint.epID << "\t\tassociated Process=" << toPrint.procID << endl;
        //}
    }
    cout << "  Process Ready Queue" << endl;
    while (!processReadyQueue.empty())
    {
        Process toPrint = processReadyQueue.front();
        processReadyQueue.pop_front();
        //processReadyQueue.pop();
        cout << "  pID = " << toPrint.pID << "\t: initial=" << toPrint.initialTime << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << " \t: remaining=" << toPrint.remainingTime << " \t: idle=" << toPrint.idleTime << endl;
    }

    cout << "  New Process Queue" << endl;
    while (!newProcessQueue.empty())
    {
        Process toPrint = newProcessQueue.front();
        newProcessQueue.pop();
        cout << "  pID = " << toPrint.pID << " \t: initial=" << toPrint.initialTime << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << " \t: remaining=" << toPrint.remainingTime << " \t: idle=" << toPrint.idleTime << endl; // << " \t: final=" << toPrint.finalTime << endl;
    }

    cout << "  Priority Process Ready Queue" << endl;
    while (!priorityPRQ.empty())
    {
        Process toPrint = priorityPRQ.top();
        priorityPRQ.pop();
        cout << "  pID = " << toPrint.pID << " \t: initial=" << toPrint.initialTime << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << " \t: remaining=" << toPrint.remainingTime << " \t: idle=" << toPrint.idleTime << endl;
    }

    cout << "  Completed Process Queue" << endl;
    while (!valuesQueue.empty())
    {
        Process toPrint = valuesQueue.front();
        valuesQueue.pop();
        cout << "  pID = " << toPrint.pID << " \t: initial=" << toPrint.initialTime << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime <<  " \t: idle=" << toPrint.idleTime << endl;
        //cout << "  pID = " << toPrint.pID << " \t: initial=" << toPrint.initialTime << " \t: arrival=" << toPrint.arrivalTime << " \t: burst=" << toPrint.burstTime << " \t: remaining=" << toPrint.remainingTime << " \t: idle=" << toPrint.idleTime << endl; // << " \t: final=" << toPrint.finalTime << endl;
    }

    //cout << "  Completed Event Queue" << endl;
    //while (!completedEventQueue.empty())
    //{
    //    Event toPrint = completedEventQueue.front();
    //    completedEventQueue.pop();
    //    cout << "  time=" << toPrint.time << " \t\t: type=" << toPrint.type << " \t\t: epid=" << toPrint.epID << "\t\tassociated Process=" << toPrint.procID << endl;
    //}

    //cout << "  New Event Queue" << endl;
    //while (!newEventQueue.empty())
    //{
    //    Event toPrint = newEventQueue.front();
    //    newEventQueue.pop();
    //    cout << "  time=" << toPrint.time << " \t\t: type=" << toPrint.type << " \t\t: epid=" << toPrint.epID << "\t\tassociated Process=" << toPrint.procID << endl;
    //}
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
        cout << "\ten hundredths of a second (0.01 sec resolution)" << endl;
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
