
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include <deque>        // std::deque
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

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

const int numPlayers = 3;

/***************************************************
******************** Prototypes ********************
***************************************************/
void populateDeck();
void shuffleDeck();
void printDeck();
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

   seed = atoi(argv[1]);
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
   
   for(long ii = 0; ii < 3; ii++)   // loop for 3 rounds of play
   { 
      winner = false;
      roundNo = ii+1;

            // creating dealer thread
      pthread_create(&threads[0], &attr, &dealer, (void *)ii);

      while(!sequence);

      while(!winner && sequence > 0){  // loop for hand rotation until winner is declared
         for(int iii = 1; iii <= numPlayers; iii++){  // creating player threads
           pthread_create(&threads[iii], &attr, &turn, (void *)players[iii]);
         }   
               
        pthread_join(threads[0], NULL);

        for(int iv = 1; iv <= numPlayers; iv++){   // joining all threads
           pthread_join(threads[iv], NULL);
        }
      }
      playerWon();
   }
  pthread_mutex_destroy(&mutexdealerDeck);
  pthread_cond_destroy(&threadWait);
  pthread_attr_destroy(&attr);
   
  pthread_exit (NULL);
}

/***************************************************
******************** Functions *********************
***************************************************/

/***************************************************
**************** Populate the Deck *****************
***************************************************/
 void populateDeck(){
  //cout << "populateDeck Function" << endl;
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
**************** Deal Initial Cards ****************
***************************************************/
  void initialDeal(){
   for(int i = 1; i <= numPlayers; i++){
      int cardToDeal = dealerDeck->deck.front();
      dealerDeck->deck.pop_front();
      players[i]->playerHand.push_back(cardToDeal);
      players[i]->won = false;
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
      discard(p);    // maybe add won condition to prevent remaining players from discarding
   }
   if(sequence == 3)
   {
      //printDeck();
      sequence = 1;
   }
   else
      sequence++;
   pthread_mutex_unlock(&mutexdealerDeck);
   pthread_cond_broadcast(&threadWait);
   pthread_exit(NULL);
}

/***************************************************
******************* Dealer Duties ******************
***************************************************/
void* dealer(void* i){                                   // *************************** DEALER
   long seqMod = (long)i;
   pthread_mutex_lock(&mutexdealerDeck);
   if(sequence == 0)
   {
      populateDeck();      // *** this needs to be only on first round ***
   }
   shuffleDeck();
   initialDeal();
   sequence = seqMod+1;
   pthread_mutex_unlock(&mutexdealerDeck);
   pthread_cond_broadcast(&threadWait);
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
      //displayHand(p);
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
//  individual player discards a card
   srand (seed);
   int toDiscard = rand() % 100 + 1;
   if(toDiscard%2 == 0){
      int cardToDiscard = p->playerHand.front();
      p->playerHand.pop_front();
      dealerDeck->deck.push_back(cardToDiscard);
   }
   else {
      int cardToDiscard = p->playerHand.back();
      p->playerHand.pop_back();
      dealerDeck->deck.push_back(cardToDiscard);
   }
   //displayHand(p);
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
   for( int i = 1; i <= numPlayers; i++)
   {
      displayHand(players[i]);
   }
   printDeck();
   for ( int ii = 1; ii <= numPlayers; ii++)
   {
      while (!players[ii]->playerHand.empty())
         {
           discard(players[ii]);
         }
   }
   
}
