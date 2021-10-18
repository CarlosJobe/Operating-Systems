/***********************************************************************
****************************** CS4328.004 ******************************
****************************** Program #2 ******************************
*********************** Carlos Jobe and Tyler Fain *********************
**************************** 8 November 2021 ***************************
***********************************************************************/

#include <iostream>
#include <chrono>
#include <array>

using namespace std;

/***********************************************************************
*************************** Global Variables ***************************
***********************************************************************/

int schedAlg;
int avgArrivalRate;
float avgServiceTime;
float quantumInterval;
//bool improperInput;

/***********************************************************************
****************************** Prototypes *****************************
***********************************************************************/

bool commandLineInput(int argc, char* argv[]);
void commandLineInstructions(int i);


/***********************************************************************
********************************* Main *********************************
***********************************************************************/
int main(int argc, char* argv[])
{
    bool exiting = commandLineInput(argc, argv);
    if (exiting)
    {
        cout << "\n\t*** exiting program because of bad command line input ***" << endl;
        return 0;
    }

    cout << "\n\t*** exiting program normally ***" << endl;
    return 0;
}

/***********************************************************************
******************************* Functions ******************************
***********************************************************************/



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
        cout << "\tin hundreths of a second (0.01 sec resolution)" << endl;
        cout << "<Quantum> indicates the quantum interval for Round Robin scheduling" << endl;
        cout << "\tFor RR scheduling this value is in milliseconds (0.001 sec resolution)" << endl;
        cout << "\tYou can leave this blank for Algorithm #1 or #2 as it wil be ignored" << endl;
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
