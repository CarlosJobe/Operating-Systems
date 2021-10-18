/***************************************************
******************** CS4328.004 ********************
******************** Program #2 ********************
************* Carlos Jobe and Tyler Fain ***********
****************** 8 November 2021 *****************
***************************************************/

#include <iostream>
#include <chrono>
#include <array>


/***************************************************
***************** Global Variables *****************
***************************************************/

int schedAlg;
float avgArrivalRate;
float avgServiceTime;
float quantumInterval;

/***************************************************
******************** Prototypes ********************
***************************************************/

void commandLine(int i);

/***************************************************
*********************** Main ***********************
***************************************************/
int main(int argc, char "argv[])
         {
           if (argv.size() < 3 || argv.size() > 4)
           {
             
           }
           
         }
         
/***************************************************
********************* Functions ********************
***************************************************/
         
void commandLine(int i)
         {
          if(i = 0)
          {
           cout << "Command Line Arguments Are Necessary" << endl;
            cout << "Use the following format:" << endl;
            cout << "./simulator.out <Algorithm#> <Arrival> <Service> <Quantum>" << endl;
            cout << "Where:" << endl;
            cout << "<Algorithm#> indicates the desired scheduling algorithm as follows:" << endl;
            cout << "\t1 - First-Come First-Served (FCFS) [non-preemptive]" << endl;
            cout << "\t2 - Shortest Remaining Time First (SRTF) [preemptive by comparison of run time left]" << endl;
            cout << "\t3 - Round Robin (RR) [preemptive by quantum]" << endl;
            cout << "<Arrival> indicates the average arrival rate in processes per second" << endl;
            cout << "from 1 process per second to 30 processes per second" << endl;
            cout << "<Service> indicates the average service time" << endl;
            cout << "\tin hundreths of a second (0.01 sec resolution)" << endl;
            cout << "<Quantum> indicates the quantum interval for Round Robin scheduling" << endl;
            cout << "\tFor RR scheduling this value is in milliseconds (0.001 sec resolution)" << endl;
          }
         }
