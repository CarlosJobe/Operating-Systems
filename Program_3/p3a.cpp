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
#include <deque>
#include <random>
#include <ctime>
#include <stdlib.h>
#include <iomanip>

using namespace std;

/***********************************************************************
*************************** Global Variables ***************************
***********************************************************************/

const int pageReferenceStringLength = 100;                   // length of page string
const int pageFrameMax = 30;                                // max # of page frames
array<int, pageReferenceStringLength> pageReferenceString;
int faultsFIFO = 0;
int faultsLRU = 0;
int faultsOPT = 0;
int frame = 0;

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
void calculations();
void printLog();
void generate_report();
int nextOPT(vector<int> &fl, int i);

/***********************************************************************
********************************* Main *********************************
***********************************************************************/
int main(int argc, char* argv[])
{
    populatePageReferenceString();

    for (int i = 1; i <= pageFrameMax; i++) {  // i represents the number of physical memory frames
        processFIFO(i);
        processLRU(i);
        processOPT(i);
    }
    calculations();
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
    queue<int> toReplace;
    bool match = false;
    int matchValue;     // for testing
    vector<int> frameList;
    PageLog log{};
    log.pageNumberCount = p;
    log.pageFaultCount = 0;
    //cout << "start with frame count = " << p << endl;        // @@ uncomment to trace algorithm @@
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
        //if (match)
            //cout << "match found: " << matchValue << endl;        // @@ uncomment to trace algorithm @@
        if (!match && frameList.size() < p) {   // this fills the frameList/toReplace and increments the fault counter
            frameList.push_back(pageReferenceString[i]);
            log.pageFaultCount += 1;
            toReplace.push(pageReferenceString[i]);
            match = false;
            //for (int j = 0; j < frameList.size(); j++) {        // @@ uncomment to trace algorithm @@
            //    cout << frameList[j] << "\t";
            //}
            //cout << endl;
        }
        else if (!match && frameList.size() == p)  // identify toReplace page and replace it
        {
            int tempReplace = toReplace.front();
            toReplace.pop();
            for (int iii = 0; iii < loopLimit; iii++)
            {
                if (tempReplace == frameList[iii])
                {
                    frameList[iii] = pageReferenceString[i];
                    log.pageFaultCount += 1;
                    toReplace.push(pageReferenceString[i]);
                }
            }
            match = false;
            //for (int j = 0; j < frameList.size(); j++) {        // @@ uncomment to trace algorithm @@
            //    cout << frameList[j] << "\t";
            //}
            //cout << endl;
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
    queue<int> toReplace;
    queue<int> tempQ;
    bool match = false;
    int matchValue;     // for testing
    vector<int> frameList;
    PageLog log{};
    log.pageNumberCount = p;
    log.pageFaultCount = 0;
    //cout << "start with frame count = " << p << endl;        // @@ uncomment to trace algorithm @@
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
                // when we have a match, reorder the toReplace list
                swap(toReplace, tempQ);
                while (!tempQ.empty())
                {
                    int move = tempQ.front();
                    tempQ.pop();
                    if (move != matchValue)
                    {
                        toReplace.push(move);
                    }
                }
                toReplace.push(matchValue);
            }
        }
        //if (match)
            //cout << "match found: " << matchValue << endl;        // @@ uncomment to trace algorithm @@
        if (!match && frameList.size() < p) {   // this fills the frameList/toReplace and increments the fault counter
            frameList.push_back(pageReferenceString[i]);
            log.pageFaultCount += 1;
            toReplace.push(pageReferenceString[i]);
            match = false;
            //for (int j = 0; j < frameList.size(); j++) {        // @@ uncomment to trace algorithm @@
            //    cout << frameList[j] << "\t";
            //}
            //cout << endl;
        }
        else if (!match && frameList.size() == p)  // identify toReplace page and replace it
        {
            int tempReplace = toReplace.front(); // maybe a LRU finder function here?
            toReplace.pop();
            for (int iii = 0; iii < loopLimit; iii++)
            {
                if (tempReplace == frameList[iii])
                {
                    frameList[iii] = pageReferenceString[i];
                    log.pageFaultCount += 1;
                    toReplace.push(pageReferenceString[i]);
                }
            }
            match = false;
            //for (int j = 0; j < frameList.size(); j++) {        // @@ uncomment to trace algorithm @@
            //    cout << frameList[j] << "\t";
            //}
            //cout << endl;
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
void processOPT(int p) { // p represents the number of physical memory frames
    int loopLimit;
    int optValue;
    bool match = false;
    int matchValue;     // for testing
    vector<int> frameList;
    PageLog log{};
    log.pageNumberCount = p;
    log.pageFaultCount = 0;
    //cout << "start with frame count = " << p << endl;        // @@ uncomment to trace algorithm @@
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
        //if (match)
        //    cout << "match found: " << matchValue << endl;        // @@ uncomment to trace algorithm @@
        if (!match && frameList.size() < p) {   // this fills the frameList/toReplace and increments the fault counter
            frameList.push_back(pageReferenceString[i]);
            log.pageFaultCount += 1;
            match = false;
            //for (int j = 0; j < frameList.size(); j++) {        // @@ uncomment to trace algorithm @@
            //    cout << frameList[j] << "\t";
            //}
            //cout << endl;
        }
        else if (!match && frameList.size() == p)  // identify optimal page and replace it
        {
            optValue = nextOPT(frameList, i);
            for (int iii = 0; iii < loopLimit; iii++) // cycle through page list to find page to replace
            {
                if (optValue == frameList[iii])
                {
                    frameList[iii] = pageReferenceString[i];
                    log.pageFaultCount += 1;
                }
            }
            match = false;
            //for (int j = 0; j < frameList.size(); j++) {        // @@ uncomment to trace algorithm @@
            //    cout << frameList[j] << "\t";
            //}
            //cout << endl;
        }
        else
        {
            match = false;
        }
    }
    resultsOPT.push(log);
}

/***************************************************
***************** next OPT selector ****************
***************************************************/
/*
identify and return the page that will not be used for the
longest period of time
*/
int nextOPT(vector<int> &fl, int i)
{
    deque<int> tempFrameList;
    deque<int> tempPageRefString;

    for (int k = 0; k < fl.size(); k++)
    {
        tempFrameList.push_front(fl[k]);
    }
    for (int j = i; j < pageReferenceString.size(); j++)
    {
        for (int jj = 0; jj < tempFrameList.size(); jj++)
        {
            if (tempFrameList[jj] == pageReferenceString[j])
            {
                if (tempFrameList.size() == 1)
                {
                    return tempFrameList.front();
                }
                else
                {
                    tempFrameList.erase(tempFrameList.begin()+jj);
                }
            }
        }
    }
    return tempFrameList.front();
}
/***************************************************
******************** Calculations *******************
***************************************************/
void calculations()
{
    ofstream xcel("data.csv");
    if(xcel.is_open())
    {
        if(frame == 0)
        {
            xcel << "frame, faults FIFO, faults LRU, faults OPT\n";
        }
    while(!resultsFIFO.empty() && !resultsLRU.empty() && !resultsOPT.empty())
    {
        PageLog fifo = resultsFIFO.front();
        resultsFIFO.pop();
        
        faultsFIFO = fifo.pageFaultCount;
        frame = fifo.pageNumberCount;
        
        PageLog lru = resultsLRU.front();
        resultsLRU.pop();
        
        faultsLRU = lru.pageFaultCount;
        
        PageLog opt = resultsOPT.front();
        resultsOPT.pop();
        
        faultsOPT = opt.pageFaultCount;
        
        xcel << fifo.pageNumberCount << "," << fifo.pageFaultCount << "," << lru.pageFaultCount << "," << opt.pageFaultCount << endl;

    }
    cout << "Total frames: " << frame << endl;
    cout << "Total page faults for FIFO: " << faultsFIFO << endl;
    cout << "Total page faults for LRU: " << faultsLRU << endl;
    cout << "Total page faults for OPT: " << faultsOPT << endl;
    }
 
    else cout << "error" << endl;
}

/***************************************************
*********** TEMPORARY LOG PRINTER *********
***************************************************/
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
    //array<int,20> temp = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    //pageReferenceString.swap(temp);
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
