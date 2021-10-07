#include <iostream>  
#include <algorithm>  
#include <deque>   
#include <random>      
#include <fstream>      

using namespace std;

struct Player
{
   int pNo;
   deque<int> playerHand;
   bool won;
};

struct Deck
{
   deque<int> deck;
};

Deck* dealerDeck = new Deck;
Player* d1 = new Player;
Player* p1 = new Player;
Player* p2 = new Player;
Player* p3 = new Player;
Player* players[4] = {d1,p1,p2,p3};

bool winner = false;
bool deckLocked = false;
long sequence = 0;
int seed = 0;
int roundNo = 0;
long roundsWon = 0;
int handCount = 0;
FILE *gFile;

const int numPlayers = 3;

/***************************************************
******************** Prototypes ********************
***************************************************/
void populateDeck();
void shuffleDeck();
void printDeck();
void logDeck();
void initialDeal();
void* dealer(void*);
void* turn(void*);
void drawCard(Player*& p);
void discard(Player*& p);
void checkWin(Player*& p);
void displayHand(Player*& p);
void playerWon();

pthread_mutex_t mutexdealerDeck;
pthread_cond_t threadWait;

int main(int argc, char *argv[]){

    gFile = fopen("gameLog.txt", "a");
   seed = atoi(argv[0]);
   pthread_t threads[4];
   pthread_attr_t attr;
   pthread_mutex_init(&mutexdealerDeck, NULL);
   pthread_cond_init (&threadWait, NULL);
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   
   for(int i = 0; i <= numPlayers; i++)      // add player numbers to player structs
   {
      players[i]->pNo = i;
   }
   roundNo = 0;
   while(roundsWon < 3){
      winner = false;
      roundNo++;

      pthread_create(&threads[0], &attr, &dealer, (void *)roundsWon);
      
      while(!sequence);
      handCount = 0;
      while(!winner && sequence > 0 && handCount <= 10){  // loop for hand rotation until winner is declared
         for(int iii = 1; iii <= numPlayers; iii++){  // creating player threads
           pthread_create(&threads[iii], &attr, &turn, (void *)players[iii]);
         }
         for(int iv = 1; iv <= numPlayers; iv++){   // joining all threads
           pthread_join(threads[iv], NULL);
        }
        pthread_join(threads[0], NULL);
        handCount++;
      }
      playerWon();
   }
  pthread_mutex_destroy(&mutexdealerDeck);
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
  for(int i = 1; i <= 4; i++){
    for(int ii = 1; ii <= 13; ii++){
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
      fprintf(gFile, "\nDealer: Shuffles and Deals\n");
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
          //fprintf(gFile, "%d ", dealerDeck->deck.at(i));
         if((i +1) % 13 == 0 )
         {
            cout << endl;
             //fprintf(gFile, "\n");
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
      //cout << "\nDECK: " << endl;
      for(int i = 0; i < dealerDeck->deck.size(); i++){
         //cout << dealerDeck->deck.at(i)  << " " ;
          fprintf(gFile, "%d ", dealerDeck->deck.at(i));
         if((i +1) % 13 == 0 )
         {
            //cout << endl;
             fprintf(gFile, "\n");
         }
      }
   }
   fprintf(gFile, "\n");
  //cout << endl;
}

/***************************************************
**************** Deal Initial Cards ****************
***************************************************/
  void initialDeal(){
   for(int i = 1; i <= numPlayers; i++){
      int cardToDeal = dealerDeck->deck.front();
      dealerDeck->deck.pop_front();
      players[i]->playerHand.push_back(cardToDeal);
      players[i]->won = false;
      // fprintf(gFile, "Player: %d hand %d \n", i, cardToDeal);
   }
}

/***************************************************
******************** Player Turn *******************
***************************************************/
void* turn(void* player){                                // *************************** TURN
   Player* p = (Player*)player;
   pthread_mutex_lock(&mutexdealerDeck);
   while(p->pNo != sequence){
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
   pthread_cond_broadcast(&threadWait);
   pthread_mutex_unlock(&mutexdealerDeck);
   pthread_exit(NULL);
}

/***************************************************
******************* Dealer Duties ******************
***************************************************/
void* dealer(void* i){
   long seqMod = (long)i;
   pthread_mutex_lock(&mutexdealerDeck);
   if(sequence == 0)
   {
      populateDeck();
   }
   shuffleDeck();
   initialDeal();
   sequence = seqMod+1;
   pthread_cond_broadcast(&threadWait);
   pthread_mutex_unlock(&mutexdealerDeck);
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
      srand (seed + roundNo + handCount);
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
************** Display Player's Hand ***************
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
       //fprintf(gFile, "\n");
   }
}

/***************************************************
************** Display Player's Hand ***************
***************************************************/
void displayHand(Player*& p){
   string win;
   cout << "PLAYER " << p->pNo << ":" << endl;
   cout << "HAND";
    //fprintf(gFile, "Player %d: hand ", p->pNo);
   for (int i = 0; i < p->playerHand.size(); i++){
      cout << " " << p->playerHand.at(i);
       //fprintf(gFile, "%d ", p->playerHand.at(i));
   }
    //fprintf(gFile, "\n");
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
      for( int i = 1; i <= numPlayers; i++)
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
      fprintf(gFile, "Round # %d was a draw", roundNo);
   }
   for ( int ii = 1; ii <= numPlayers; ii++)
      {
         fprintf(gFile, "Player %d: exits round\n", ii);
         while (!players[ii]->playerHand.empty())
            {
              discard(players[ii]);
              
            }
      }
}
