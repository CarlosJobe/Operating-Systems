/***********************************************************************
****************************** CS4328.004 ******************************
****************************** Program #3 ******************************
*********************** Carlos Jobe and Tyler Fain *********************
**************************** 2 December 2021 ***************************
***********************************************************************/

/**************************Thoughts and Ideas***************************

No command line input, the program is fully self contained.

Generate a page-reference string // maybe an array, no reordering, fixed size.

loop 1 to 30 for the number of memory frames available
    within each loop run each of the 3 algorithms

need an output container. must log: # of frames, algorithm, # of page faults
    there will be 90 instances of this container // queue of structs should
    work since we are familliar with them.

*/

#include <iostream>
#include <array>
#include <vector>
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

const int pageReferenceStringLength = 20;                   // length of page string
const int pageFrameMax = 3;                                // max # of page frames
array<int, pageReferenceStringLength> pageReferenceString;

struct PageLog {
    int pageNumberCount;
    int pageFaultCount;
};

queue<PageLog> resultsFIFO;
queue<PageLog> resultsLRU;
queue<PageLog> resultsOPT;


/***********************************************************************
****************************** Prototypes *****************************
***********************************************************************/

void populatePageReferenceString();
int urand();
void processFIFO(int p);
void processLRU(int p);
void processOPT(int p);
void printLog();

/***********************************************************************
********************************* Main *********************************
***********************************************************************/
int main(int argc, char* argv[])
{
    populatePageReferenceString();

    for (int i = 1; i <= pageFrameMax; i++) {  // i represents the number of physical memory frames
        //processFIFO(i);
        processLRU(i);
        //processOPT(i);
    }
    cout << endl;
    for (int i = 0; i < pageReferenceStringLength; i++)
    {
        cout << pageReferenceString[i] << ", " ;
    }
    cout << "\nLog:" << endl;
    printLog();

    cout << "\n\t*** exiting program normally ***" << endl;
    return 0;
}

/***********************************************************************
******************************* Functions ******************************
***********************************************************************/

/***************************************************
******************* process FIFO *******************
***************************************************/
/*
Process page reference string using FIFO
*/
void processFIFO(int p) { // p represents the number of physical memory frames
    int loopLimit;
    queue<int> oldest;
    bool match = false;
    int matchValue;     // for testing
    vector<int> frameList;
    PageLog log{};
    log.pageNumberCount = p;
    log.pageFaultCount = 0;
    cout << "start with frame count = " << p << endl;
    for (int i = 0; i < pageReferenceStringLength; i++) {  // this loop iterates through the 100 elements in the pageReferenceString
        if (frameList.size() < p)
            loopLimit = frameList.size();
        else
            loopLimit = p;
        for (int ii = 0; ii < loopLimit; ii++) // this loop iterates through the frameList to check for matches
        {
            if (pageReferenceString[i] == frameList[ii])
            {
                match = true;
                matchValue = frameList[ii];
            }
        }
        if (match)
            cout << "match found: " << matchValue << endl;
        if (!match && frameList.size() < p) {   // this fills the frameList/oldest and increments the fault counter
            frameList.push_back(pageReferenceString[i]);
            log.pageFaultCount += 1;
            oldest.push(pageReferenceString[i]);
            match = false;
            for (int j = 0; j < frameList.size(); j++) {
                cout << frameList[j] << "\t";
            }
            cout << endl;
        }
        else if (!match && frameList.size() == p)  // identify oldest page and replace it
        {
            int toReplace = oldest.front();
            oldest.pop();
            for (int iii = 0; iii < loopLimit; iii++)
            {
                if (toReplace == frameList[iii])
                {
                    frameList[iii] = pageReferenceString[i];
                    log.pageFaultCount += 1;
                    oldest.push(pageReferenceString[i]);
                }
            }
            match = false;
            for (int j = 0; j < frameList.size(); j++) {  // this loop displays the frame list in process // for testing
                cout << frameList[j] << "\t";
            }
            cout << endl;
        }
        else
        {
            match = false;
        }
    }
    resultsFIFO.push(log);
}

/***************************************************
******************** process LRU *******************
***************************************************/
/*
Process page reference string using LRU
*/
void processLRU(int p) {
    int loopLimit;
    queue<int> oldest;
    queue<int> tempQ;
    bool match = false;
    int matchValue;     // for testing
    vector<int> frameList;
    PageLog log{};
    log.pageNumberCount = p;
    log.pageFaultCount = 0;
    cout << "start with frame count = " << p << endl;
    for (int i = 0; i < pageReferenceStringLength; i++) {  // this loop iterates through the 100 elements in the pageReferenceString
        if (frameList.size() < p)
            loopLimit = frameList.size();
        else
            loopLimit = p;
        for (int ii = 0; ii < loopLimit; ii++) // this loop iterates through the frameList to check for matches
        {
            if (pageReferenceString[i] == frameList[ii])
            {
                match = true;
                matchValue = frameList[ii];
                // when we have a match, reorder the oldest list
                swap(oldest, tempQ);
                while (!tempQ.empty()) 
                {
                    int move = tempQ.front();
                    tempQ.pop();
                    if (move != matchValue)
                    {
                        oldest.push(move);
                    }
                }
                oldest.push(matchValue);
            }
        }
        if (match)
            cout << "match found: " << matchValue << endl;
        if (!match && frameList.size() < p) {   // this fills the frameList/oldest and increments the fault counter
            frameList.push_back(pageReferenceString[i]);
            log.pageFaultCount += 1;
            oldest.push(pageReferenceString[i]);
            match = false;
            for (int j = 0; j < frameList.size(); j++) {
                cout << frameList[j] << "\t";
            }
            cout << endl;
        }
        // @@@@@@ HERE IS WHERE WE NEED TO IMPLEMENT THE LRU ALGORITHM @@@@@@
        else if (!match && frameList.size() == p)  // identify oldest page and replace it
        {
            int toReplace = oldest.front(); // maybe a LRU finder function here?
            oldest.pop();
            for (int iii = 0; iii < loopLimit; iii++)
            {
                if (toReplace == frameList[iii])
                {
                    frameList[iii] = pageReferenceString[i];
                    log.pageFaultCount += 1;
                    oldest.push(pageReferenceString[i]);
                }
            }
            match = false;
            for (int j = 0; j < frameList.size(); j++) {  // this loop displays the frame list in process // for testing
                cout << frameList[j] << "\t";
            }
            cout << endl;
        }
        else
        {
            match = false;
        }
    }
    resultsLRU.push(log);

}

/***************************************************
******************* process OPT *******************
***************************************************/
/*
Process page reference string using OPT
*/
void processOPT(int p) {
    

}

void printLog()
{
    while (!resultsFIFO.empty())
    {
        PageLog p = resultsFIFO.front();
        resultsFIFO.pop();
        cout << "pageNumberCount=" << p.pageNumberCount << ": pageFaultCount=" << p.pageFaultCount << endl;
    }

    while (!resultsLRU.empty())
    {
        PageLog p = resultsLRU.front();
        resultsLRU.pop();
        cout << "pageNumberCount=" << p.pageNumberCount << ": pageFaultCount=" << p.pageFaultCount << endl;
    }

    while (!resultsOPT.empty())
    {
        PageLog p = resultsOPT.front();
        resultsOPT.pop();
        cout << "pageNumberCount=" << p.pageNumberCount << ": pageFaultCount=" << p.pageFaultCount << endl;
    }
}

/***************************************************
*********** populate page reference string *********
***************************************************/
/*
populate the page-refernce string with 100 ints with
random values from 0 to 49
*/
void populatePageReferenceString()
{
    for (int i = 0; i < pageReferenceStringLength; i++)
    {
        int tempTest = urand();
        pageReferenceString[i] = tempTest;
    }
}

/***************************************************
*********************** urand **********************
***************************************************/
/*
returns a random number between 0 and 1
*/
int urand()
{
    mt19937 rng;
    uniform_int_distribution<int> dist(0, 49);  //(min, max)
    rng.seed(random_device{}()); //non-deterministic seed
    return dist(rng);
}
