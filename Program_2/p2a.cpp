/***********************************************************************
****************************** CS4328.004 ******************************
****************************** Program #2 ******************************
*********************** Carlos Jobe and Tyler Fain *********************
*************************** 12 November 2021 ***************************
***********************************************************************/

#include <iostream>
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
float mainTime;       
float arrivalTime;      // increments to track arrival time for event queue
float arrivalInterval;  // value based on avgArrivalTime and exponential calc. used for incrementing arrival time
float newProcessClock;

int num_processes = 10000;
double totalTurnaround = 0;
double totalWaiting = 0;
double totalResponse = 0;
double totalIdle = 0;
double totalTime = 0;
double avgTurnaround;
double avgWaiting;
double avgResponse;
double utilization;

int pIDCounter;
int eventPIDCounter;

double calculatedTurnaround;
double calculatedWaiting;
double calculateUtilization;
double throughput;


struct Event {
    float time;
    int type;     
    int epID;
    int procID;
    bool operator < (const Event rhs) const
    {
        if (schedAlg == 2)
            if(rhs.time == time)
                return rhs.epID < epID;
            else
                return rhs.time < time; 
        else
            return rhs.epID < epID;
    }
};

struct Process {
    int pID;
    float arrivalTime;
    float burstTime;
    float remainingTime;
    float initialTime;   
    float finalTime;
    float turnaroundTime;
    float waitingTime;
    float responseTime;
    float idleTime;
    float CPUidleTime;
    float totalTime;

    bool operator<(const Process& a) const
    {
        if (remainingTime == a.remainingTime)
        {
            return arrivalTime < a.arrivalTime;
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

bool commandLineInput(int argc, char* argv[]);
void commandLineInstructions(int i);
float urand();
float genexp(float lambda);
struct Process generateProcess();
void generateProcessEvents(struct Process p);
void generateEventToCreateProcess(float time);
void runStatistics();


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
    runStatistics();
    generate_report();

    cout << "\n\t*** exiting program normally ***" << endl;
    return 0;
}

/***********************************************************************
******************************* Functions ******************************
***********************************************************************/



/***************************************************
************************ Init **********************
***************************************************/
/*
initialize all variable, states, and end conditions
schedule first events
*/
void init()
{
    mainTime = 0.0f;
    arrivalTime = 0.0f;
    arrivalInterval = 0.0f;
    pIDCounter = 1;
    eventPIDCounter = 1;
    avgArrivalTime = 1.0f / static_cast<float>(avgArrivalRate);

    generateEventToCreateProcess(mainTime);

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

    while (valuesQueue.size() < static_cast<float>(num_processes))
    {
        Event toRun = eventQueue.top();
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
        default:
            break;
        }
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
        p.arrivalTime = arrivalTime;

    p.pID = pIDCounter;
    p.burstTime = (genexp(1/avgServiceTime));
    p.remainingTime = p.burstTime;
    p.initialTime = 0.0f;
    p.finalTime = 0.0f;
    p.turnaroundTime = 0.0f;
    p.waitingTime = 0.0f;
    p.responseTime = 0.0f;

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
    e1.epID = eventPIDCounter;
    e1.procID = p.pID;
    eventPIDCounter++;
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
        e2.epID = eventPIDCounter;
        e2.procID = p.pID;
        eventPIDCounter++;
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
    //newEventQueue.push(e1);
    int x1 = schedule_event(e1);
    if (x1)
        cout << "error scheduling event" << endl;
    //if (t != 0)
    //    arrivalInterval = (genexp(1 / avgArrivalTime));
}

/***************************************************
******************* schedule event *****************
***************************************************/
/*
insert event into the event queue in its order of time
*/
int schedule_event(struct Event new_event)
{
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
    if (mainTime < e.time)
    {
        float tempTime = e.time - mainTime;
        p.CPUidleTime = tempTime;
        mainTime += tempTime;
    }
    if (p.initialTime == 0.0f)
    {
        p.initialTime = mainTime;
    }
    p.waitingTime = p.initialTime - p.arrivalTime;

    processReadyQueue.push_front(p);

    //return 0;  // temp to make empty program run
}

/***************************************************
*************** Handle Event Type 2 ****************
***************************************************/
/*
process the second event type
*/
void handleEventType2(struct Event e)
{
    //Need to check if priorityPRQ is empty and if so move to the next event
    if (!priorityPRQ.empty())
    {
        Event tempE = eventQueue.top();
        int tempType;
        Process currentP = priorityPRQ.top();
        priorityPRQ.pop();
        if (mainTime < currentP.arrivalTime)  // if current time is before process arrival calculate idle time and advance maintime to match
        {
            float CPUidleTime = currentP.arrivalTime - mainTime;
            currentP.CPUidleTime += CPUidleTime;
            mainTime += CPUidleTime;
        }
        if (mainTime > currentP.arrivalTime)    // if current time is after process arrival, calculate wait time and update event time
        {
            float waitTime = mainTime - currentP.arrivalTime;
            currentP.waitingTime += waitTime;
        }
        if (currentP.initialTime == 0.0)    // set process inital time
        {
            if (mainTime < currentP.arrivalTime)    // if current time is before process arrival time then set initial time to arriv
            {
                currentP.initialTime = currentP.arrivalTime;
            }
            else            // if the current time is after process arrival then set initial time to current time
            {
                currentP.initialTime = mainTime;
            }
        }
        //  scheduling algorithm
        if (!priorityPRQ.empty() && (((mainTime + currentP.remainingTime) > tempE.time) ||
            (((mainTime + currentP.remainingTime) > arrivalTime))))  // **T2A** if next event is before end of current event 
        {
            float tempTime = 0.0f;

            if ((tempE.time < arrivalTime) && (tempE.time > mainTime))
            {
                tempTime = (tempE.time - mainTime);
                currentP.remainingTime -= tempTime;      // decrement remaining time
                priorityPRQ.emplace(currentP);                         
                mainTime += tempTime;
                tempType = SRTF_START;
            }
            else //if (arrivalTime < tempE.time)
            {
                tempTime = (arrivalTime - mainTime);
                if (currentP.remainingTime < tempTime)
                {
                    tempTime = currentP.remainingTime;
                    mainTime += tempTime;
                    currentP.remainingTime = 0.0f;
                    currentP.finalTime = mainTime;                                                      // stats
                    tempType = SRTF_END;
                    currentP.turnaroundTime = currentP.finalTime - currentP.arrivalTime;                // stats
                    currentP.idleTime = currentP.finalTime - currentP.arrivalTime - currentP.burstTime; // stats
                    currentP.totalTime = currentP.finalTime;
                    valuesQueue.push(currentP);
                }
                else
                {
                    currentP.remainingTime -= tempTime;
                    mainTime += tempTime;
                    tempType = SRTF_START;
                    priorityPRQ.emplace(currentP);
                }
            }
            // create new start event
            Event tempE2{};
            tempE2.time = mainTime + tempTime;
            tempE2.type = tempType;
            tempE2.epID = eventPIDCounter;
            tempE2.procID = currentP.pID;
            eventPIDCounter++;
            int E2 = schedule_event(tempE2);
        }
        else
        {
            float tempTime = 0.0f;
            int tempType = 0;
            if ((!priorityPRQ.empty()) && (mainTime >= tempE.time)) // || mainTime >= arrivalTime)
            {
                Process tempP = priorityPRQ.top();
                if (tempP.remainingTime < currentP.remainingTime)
                {
                    tempTime = currentP.remainingTime - tempP.remainingTime;
                    tempType = SRTF_START;
                    currentP.remainingTime -= tempTime;      // decrement remaining time
                    priorityPRQ.emplace(currentP);
                    mainTime += tempTime;
                }
            }
            else
            {
                tempTime = currentP.remainingTime;
                mainTime += tempTime;
                currentP.finalTime = mainTime;                                                      // stats
                tempType = SRTF_END;
                currentP.remainingTime = 0.0f;      // decrement remaining time
                currentP.turnaroundTime = currentP.finalTime - currentP.arrivalTime;                // stats
                currentP.idleTime = currentP.finalTime - currentP.arrivalTime - currentP.burstTime; // stats
                currentP.totalTime = currentP.finalTime;
                valuesQueue.push(currentP);
            }
            Event tempE2{};
            tempE2.time = currentP.arrivalTime + tempTime; // different tempTime...
            tempE2.type = tempType;
            tempE2.epID = eventPIDCounter;
            tempE2.procID = currentP.pID;
            eventPIDCounter++;
            int E2 = schedule_event(tempE2);
        }
    }
    else
        cout << "priorityPRQ empty" << endl;

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
    float tempType; 
    if (mainTime < e.time)
    {
        float CPUidleTime = e.time - mainTime;
        p.waitingTime += CPUidleTime;
        mainTime += CPUidleTime;
    }
    if (p.initialTime == 0.0f)
    {
        p.initialTime = mainTime;
    }

    if (p.remainingTime > quantumInterval)
    {
        mainTime += quantumInterval;
        p.remainingTime -= quantumInterval;
        processReadyQueue.push_back(p);
        tempType = RR_START;
    }
    else
    {
        mainTime += p.remainingTime;
        p.finalTime = mainTime;
        p.remainingTime = 0.0f;
        p.turnaroundTime = p.finalTime - p.arrivalTime;  // collect
        p.idleTime = p.finalTime - p.initialTime - p.burstTime;  // collect
        p.totalTime = p.finalTime; // -p.initialTime;
        valuesQueue.push(p);
        tempType = RR_END;
    }

    // create new  event
    Event tempE1{};
    tempE1.time = mainTime;
    tempE1.type = tempType;
    tempE1.epID = eventPIDCounter;
    tempE1.procID = p.pID;
    eventPIDCounter++;
    int E1 = schedule_event(tempE1);


    processReadyQueue.push_front(p);
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
      

    //create next creation event
    generateEventToCreateProcess(arrivalTime);
    arrivalInterval = (genexp(1/avgArrivalTime));
    arrivalTime += arrivalInterval;

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
    Process p = processReadyQueue.front();
    processReadyQueue.pop_front();
    p.finalTime = p.initialTime + p.burstTime;
    mainTime = p.finalTime;
;

    //stats
    p.turnaroundTime = p.finalTime - p.arrivalTime;
    p.idleTime = p.finalTime - p.initialTime - p.burstTime;
    p.totalTime = p.finalTime; // -p.arrivalTime;
    valuesQueue.push(p);
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

        totalIdle += p.CPUidleTime;
        totalTurnaround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
        totalTime = p.totalTime;
    }


    calculatedTurnaround = totalTurnaround / static_cast<double>(num_processes);
    //cout << "calculatedTurnaround(" << calculatedTurnaround << ")=totalTurnaround(" << totalTurnaround << ")/num_processes(" << num_processes << endl;
    calculatedWaiting = totalWaiting / static_cast<double>(num_processes);
    calculateUtilization = ((totalTime - totalIdle) / totalTime) * 100;
    //cout << "calculateUtilization(" << calculateUtilization << ")=(totalTime(" << totalTime << ")-totalIdle(" << totalWaiting << ")/num_processes(" << num_processes << endl;

    cout << "Total idle: " << totalIdle << " and total time: " << totalTime << endl;
    throughput = (static_cast<double>(num_processes) / totalTime);

    cout << "number of processes=" << num_processes << " for schedAlg alg #" << schedAlg << endl;
    cout << "calculatedTurnaround=" << calculatedTurnaround << endl;
    cout << "calculatedWaiting=" << calculatedWaiting << endl;
    cout << "calculateUtilization=" << calculateUtilization << endl;
    cout << "throughput=" << throughput << endl;
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
        x = (-1.0f / lambda) * log2(u);
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
    ofstream data("sim.data");
    ofstream xcel("sim.csv");
    if (data.is_open() && xcel.is_open())
    {

        int w = 15;
        if (avgArrivalRate == 1)
        {
            data << setfill(' ') << setw(w) << "Scheduler" << setw(w) << "Lambda" << setw(w) << "Turnaround" << setw(w) << "Throughput" << setw(w) << "CPU Utilization" << setw(w) << "throughput" << endl;
            cout << setfill(' ') << setw(w) << "Scheduler" << setw(w) << "Lambda" << setw(w) << "Turnaround" << setw(w) << "Throughput" << setw(w) << "CPU Utilization" << setw(w) << "throughput" << endl;

            xcel << "Scheduler, Lambda, Turnaround, Waiting, CPU Utilization, throughput\n";
        }
        data << setfill(' ') << setw(w) << schedAlg << setw(w) << avgArrivalRate << setw(w) << calculatedTurnaround << setw(w) << calculatedWaiting << setw(w) << calculateUtilization << setw(w) << throughput << endl;
        xcel << schedAlg << "," << avgArrivalRate << "," << calculatedTurnaround << "," << calculatedWaiting << "," << calculateUtilization << "," << throughput << endl;

        data.close();
    }
    else cout << "error" << endl;
    
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
