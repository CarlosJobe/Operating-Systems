/***************************************************
******************** CS4328.004 ********************
******************** Program #1 ********************
************* Carlos Jobe and Tyler Fain ***********
******************* 8 October 2021 *****************
***************************************************/

#include <iostream>  
#include <algorithm>  
#include <deque>   
#include <random>      
#include <fstream>      

using namespace std;

struct Player                                   // structure to store player data
{
   int pNo;
   deque<int> playerHand;
   bool won;
};

struct Deck                                     // structure to store deck information
{
   deque<int> deck;
};

Deck* dealerDeck = new Deck;
//Player* d1 = new Player;            
Player* p1 = new Player;
Player* p2 = new Player;
Player* p3 = new Player;
Player* players[3] = {p1,p2,p3};                // array of player structs

bool winner = false;                            // Used to end round and declare winner
long sequence = 0;                              // used to end thread creation/join loop and various decisions
int seed = 0;                                   // user supplied seed for rand() to shuffle deck and discard
int roundNo = 0;                                // used to output round numbers
long roundsWon = 0;                             // used to end game after 3 players have won
int handCount = 0;                              // used to limit turns and identify a draw
FILE *gFile;                                    // used to create log file

const int numPlayers = 3;                       // number of player in game
const int winsToEnd = 3;                        // number of hands won to terminate game

/***************************************************
******************** Prototypes ********************
***************************************************/
void populateDeck();                
void shuffleDeck();
void printDeck();
void logDeck();
void initialDeal();
void* dealer(void*);                            // function that manages dealer thread
void* turn(void*);                              // function that manages the player threads
void drawCard(Player*& p);
void discard(Player*& p);
void checkWin(Player*& p);
void displayHand(Player*& p);
void playerWon();

pthread_mutex_t mutexdealerDeck;                // mutual exclusion lock for the main deck of cards
pthread_cond_t threadWait;                      // mutual exclusion lock for player/dealer waiting queue

int main(int argc, char *argv[]){

   gFile = fopen("gameLog.txt", "a");
   seed = atoi(argv[1]);                        // collect seed for random from user runtime
   pthread_t threads[numPlayers+1];                        // create 4 threads for dealer and players
   pthread_attr_t attr;                         // create thread attribute
   pthread_mutex_init(&mutexdealerDeck, NULL);  // initialize mutex lock for main deck of case  
   pthread_cond_init (&threadWait, NULL);       // initialize mutex lock for waiting queue
   pthread_attr_init(&attr);                    // initialize mutex thread attribute
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);   // set mutex joinable attribute
   
   for(int i = 0; i < numPlayers; i++)          // add player numbers to player structs
   {
      players[i]->pNo = i+1;
   }
   roundNo = 0;
   while(roundsWon < winsToEnd){                         // main gameplay loop. ends with 3 wins
      winner = false;
      roundNo++;

      pthread_create(&threads[0], &attr, &dealer, (void *)roundsWon);   // create dealer thread
      
      while(!sequence);
      handCount = 0;
      while(!winner && sequence > 0 && handCount <= 10){    // loop for hand rotation until winner is declared
         for(int iii = 0; iii < numPlayers; iii++){         // creating player threads
           pthread_create(&threads[(iii+1)], &attr, &turn, (void *)players[iii]);
         }
         for(int iv = 1; iv <= numPlayers; iv++){           // joining all player threads
           pthread_join(threads[iv], NULL);
        }
        pthread_join(threads[0], NULL);                     // join dealer thread
        handCount++; 
      }
      playerWon();
   }
  pthread_mutex_destroy(&mutexdealerDeck);                  // destroying all pthread tools
  pthread_cond_destroy(&threadWait);
  pthread_attr_destroy(&attr);
  pthread_exit (NULL);
  fclose(gFile);
}

/***************************************************
******************** Functions *********************
***************************************************/

/***************************************************
**************** Populate the Deck *****************
***************************************************/
 void populateDeck(){
  dealerDeck->deck.clear();
  for(int i = 1; i <= 4; i++){                  // loop for number of suits
    for(int ii = 1; ii <= 13; ii++){            // loop for one suit of cards
      int addCard = (i*100 + ii);
      dealerDeck->deck.push_back(addCard);
    }
  }
 }

/***************************************************
***************** Shuffle the Deck *****************
***************************************************/
  void shuffleDeck(){
  shuffle(dealerDeck->deck.begin(), dealerDeck->deck.end(), default_random_engine(seed));
  fprintf(gFile, "\nDealer: Shuffles\n");
}

/***************************************************
************* Print the Deck Contents **************
***************************************************/
void printDeck(){
   if(dealerDeck->deck.size() == 0){
      cout << "\nThe deck is empty" << endl;
   }
   else{
      cout << "\nDECK: " << endl;
      for(int i = 0; i < dealerDeck->deck.size(); i++){
         cout << dealerDeck->deck.at(i)  << " " ;
         if((i +1) % 13 == 0 )
         {
            cout << endl;
         }
      }
   }
  cout << endl;
}

/***************************************************
************** Log the Deck Contents ***************
***************************************************/
void logDeck(){
   if(dealerDeck->deck.size() == 0){
      cout << "\nThe deck is empty" << endl;
   }
   else{
      fprintf(gFile, "Deck:\n");
      for(int i = 0; i < dealerDeck->deck.size(); i++){
          fprintf(gFile, "%d ", dealerDeck->deck.at(i));
         if((i +1) % 13 == 0 )
         {
             fprintf(gFile, "\n");
         }
      }
   }
   fprintf(gFile, "\n");
}

/***************************************************
**************** Deal Initial Cards ****************
***************************************************/
  void initialDeal(){
   int tempPlayerNo = sequence%3;
   for(int i = 0; i < numPlayers; i++){    
      int cardToDeal = dealerDeck->deck.front();
      dealerDeck->deck.pop_front();
      players[tempPlayerNo]->playerHand.push_back(cardToDeal);
      players[tempPlayerNo]->won = false;
      fprintf(gFile, "Dealer: Deals %d to Player %d\n", cardToDeal, (tempPlayerNo+1));
      if(tempPlayerNo == 2)
      {
         tempPlayerNo = 0;
      }
      else
      {
      tempPlayerNo++;
      }
   }
   fprintf(gFile, "\n");
}

/***************************************************
******************** Player Turn *******************
***************************************************/
void* turn(void* player){   
   Player* p = (Player*)player;
   pthread_mutex_lock(&mutexdealerDeck);                  // lock deck mutex for individual player
   while(p->pNo != sequence){                             // waiting queue loop for mutex unlock
      pthread_cond_wait(&threadWait, &mutexdealerDeck);
   }
   if(!winner)
   {
      drawCard(p);
   }
   checkWin(p);
   if(!winner)
   {
      discard(p);
   }
   if(sequence == 3)
   {
      sequence = 1;
   }
   else
   {
      sequence++;
   }
   if(!winner)
   {
      logDeck();
   }
   pthread_cond_broadcast(&threadWait);                  // broadcast signal that the mutex is unlocked
   pthread_mutex_unlock(&mutexdealerDeck);               // unlock mutex for main deck of cards
   pthread_exit(NULL);
}

/***************************************************
******************* Dealer Duties ******************
***************************************************/
void* dealer(void* i){
   long seqMod = (long)i;
   pthread_mutex_lock(&mutexdealerDeck);                 // lock deck mutex for dealer
   if(sequence == 0)
   {
      populateDeck();
   }
   shuffleDeck();
   initialDeal();
   sequence = seqMod+1;
   pthread_cond_broadcast(&threadWait);                  // broadcast signal that the mutex is unlocked
   pthread_mutex_unlock(&mutexdealerDeck);               // unlock mutex for main deck of cards
   pthread_exit(NULL);
}

/***************************************************
**************** Player Draws Card *****************
***************************************************/
void drawCard(Player* &p){
   if(!winner){
      int cardToDraw = dealerDeck->deck.front();
      dealerDeck->deck.pop_front();
      p->playerHand.push_back(cardToDraw);
      fprintf(gFile, "Player %d: hand %d \n", p->pNo, p->playerHand.front());
      fprintf(gFile, "Player %d: draws %d \n",p->pNo, cardToDraw);
   }
}

/***************************************************
************** Player Discards a Card **************
***************************************************/
void discard(Player* &p){
   if(winner)
   {
      int cardToDiscard = p->playerHand.back();
      p->playerHand.pop_back();
      dealerDeck->deck.push_back(cardToDiscard);
   }
   else{
      srand (seed + roundNo + handCount);          // added non-static values to improve randomness
      int toDiscard = rand();// % 100 + 1;
      if(toDiscard%2 == 0){
         int cardToDiscard = p->playerHand.front();
         p->playerHand.pop_front();
         dealerDeck->deck.push_back(cardToDiscard);
          fprintf(gFile, "Player %d: discards %d \n", p->pNo, cardToDiscard);
          fprintf(gFile, "Player %d: hand %d \n", p->pNo, p->playerHand.front());
      }
      else {
         int cardToDiscard = p->playerHand.back();
         p->playerHand.pop_back();
         dealerDeck->deck.push_back(cardToDiscard);
          fprintf(gFile, "Player %d: discards %d \n", p->pNo, cardToDiscard);
          fprintf(gFile, "Player %d: hand %d \n", p->pNo, p->playerHand.back());
      }
   }
}

/***************************************************
********** Check if player won the hand ************
***************************************************/
void checkWin(Player* &p){
   if((p->playerHand.size() == 2) && ((p->playerHand.front()%100) == (p->playerHand.back()%100))){
    winner = true;
    p->won = true;
      cout << endl;
      cout << "Player # " << p->pNo << " wins round " << roundNo << endl;
      cout << endl;
       fprintf(gFile, "Player %d: hand %d %d \n", p->pNo, p->playerHand.front(), p->playerHand.back());
       fprintf(gFile, "Player %d: wins round %d \n", p->pNo, roundNo);
   }
}

/***************************************************
************** Display Player's Hand ***************
***************************************************/
void displayHand(Player*& p){
   string win;
   cout << "PLAYER " << p->pNo << ":" << endl;
   cout << "HAND";
   for (int i = 0; i < p->playerHand.size(); i++){
      cout << " " << p->playerHand.at(i);
   }
   if(p->won) { win = "yes";}
   else { win = "no"; }
   cout << "\nWIN " << win << endl;
}

/***************************************************
******************* Player Won *********************
***************************************************/
void playerWon(){
   if (winner)
   {
      for( int i = 0; i < numPlayers; i++)
      {
         displayHand(players[i]);
      }
      printDeck();
      roundsWon++;
   }
   else
   {
      cout << endl;
      cout << "Round # " << roundNo << " is a draw" << endl;
      fprintf(gFile, "Round # %d was a draw\n", roundNo);
   }
   for ( int ii = 0; ii < numPlayers; ii++)
      {
         fprintf(gFile, "Player %d: exits round\n", players[ii]->pNo);
         while (!players[ii]->playerHand.empty())
            {
              discard(players[ii]);             
            }
      }
}
