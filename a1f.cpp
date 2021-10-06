#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include <deque>        // std::deque
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace std;

struct Player
{
   deque<int> playerHand;
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

long rc = 0;
bool winner = false;
bool deckLocked = false;
long sequence = 0;
int seed = 0;

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
void displayHand(Player*& p);

pthread_mutex_t mutexdealerDeck;
pthread_cond_t threadWait;

int main(int argc, char *argv[]){
   cout << " 80808080808080808080808080808080808080808080808080808080808080808080808080808080 " << endl;
   cout << " 80808080808080808080808080808080808080808080808080808080808080808080808080808080 " << endl;

   seed = atoi(argv[1]);
   pthread_t threads[4];
   pthread_mutex_init(&mutexdealerDeck, NULL);
   pthread_cond_init (&threadWait, NULL);
   
   for(int i = 0; i <= 3; i++)      // add player numbers to player structs
   {
      players[i]->playerHand.push_front(i);
      //cout << "player # " << players[i]->playerHand.front() << endl;
   }

     cout << "entering thread creation loop" << endl;

   for(long ii = 0; ii < 3; ii++)   // loop for 3 rounds of play
   { 
      winner = false;
      /*    
      *** need to clear player hands before 2nd and 3rd rounds    
      */
      cout << "sequence in main = " << sequence << endl;
            // creating dealer thread
      rc = pthread_create(&threads[0], NULL, &dealer, (void *)ii);
           if (rc){
              cout << "(82)error creating pthread. Error : " << rc << endl;
              cout << "attempting to create thread: 0" << endl;
              exit(-1);
           }
      //cout << "sequence after thread creation = " << sequence << endl;
      while(!sequence);
/*       cout << "*****************" << endl;
      cout << "***** ROUND *****" << endl;
      cout << "*****  # " << (ii+1) << "  *****" << endl;
      cout << "*****************" << endl; */

      while(!winner && sequence > 0){  // loop for hand rotation until winner is declared
         cout << "in no winner while loop" << endl;
         for(int iii = 1; iii <= 3; iii++){  // creating player threads
           rc = pthread_create(&threads[iii], NULL, &turn, (void *)players[iii]);
           if (rc){
              cout << "error creating pthread: " << iii << endl;
              exit(-1);
           }
         }   
        
        for(int iv = 0; iv <= 3; iv++){   // joining all threads
           int t = pthread_join(threads[iv], NULL);
/*            if(!t){
              cout << "error joining thread: " << iv << endl;
              exit(-1);
           } */
        }
      }
   }
  pthread_mutex_destroy(&mutexdealerDeck);
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
  for(int v = 1; v <= 4; v++){
    for(int vi = 1; vi <= 13; vi++){
      int addCard = (v*100 + vi);
      dealerDeck->deck.push_back(addCard);
    }
  }
 }

/***************************************************
***************** Shuffle the Deck *****************
***************************************************/
  void shuffleDeck(){
  cout << "shuffleDeck Function" << endl;
  shuffle(dealerDeck->deck.begin(), dealerDeck->deck.end(), default_random_engine(seed));
}

/***************************************************
************* Print the Deck Contents **************
***************************************************/
void printDeck(){
   //cout << "printDeck Function" << endl;
   if(dealerDeck->deck.size() == 0){
      cout << "\nThe deck is empty" << endl;
   }
   else{
      cout << "\nThe deck contains: " << endl;
      for(int vii = 0; vii < dealerDeck->deck.size(); vii++){
         cout << dealerDeck->deck.at(vii)  << " " ;
         if((vii +1) % 13 == 0 )
         {
            cout << endl;
         }
      }
   }
  cout << "\n * * * * * * * * * * * * * * * * * * * * * * * * * " << endl;
  return;
}

/***************************************************
**************** Deal Initial Cards ****************
***************************************************/
  void initialDeal(){
   //cout << "initialDeal Function" << endl;
   for(int viii = 1; viii <= 3; viii++){
      int cardToDeal = dealerDeck->deck.front();
      //cout << "card to deal = " << cardToDeal << endl;
      dealerDeck->deck.pop_front();
      //cout << " about to copy place holder to " << players[viii]->playerHand.at(0) << " players back " << endl;
      players[viii]->playerHand.push_back(cardToDeal);
      cout << "Player " << players[viii]->playerHand.front() << "'s card is " << players[viii]->playerHand[1] << endl;
      //printDeck();
   }
      //cout << "leaving initial deal function" << endl;
}

/***************************************************
******************** Player Turn *******************
***************************************************/
void* turn(void* player){        // *************************** TURN
   Player* p = (Player*)player;
   pthread_mutex_lock(&mutexdealerDeck);
   while(p->playerHand.front() != sequence){
      cout << "(turn:180)waiting in player conditional wait loop" << endl;
      //cout << "sequence = " << sequence << endl;
      //cout << "playerNo = " << p->playerHand.front() << endl;
      pthread_cond_wait(&threadWait, &mutexdealerDeck);
   }
   cout << "out of player conditional wait loop" << endl;

   drawCard(p);
   discard(p);
   if(sequence == 3)
   {
      printDeck();
      sequence = 1;
   }
   else
      sequence++;
   cout << "sequence in turn = " << sequence << endl;
   pthread_mutex_unlock(&mutexdealerDeck);
   pthread_cond_broadcast(&threadWait);
   pthread_exit(NULL);
}

/***************************************************
******************* Dealer Duties ******************
***************************************************/
void* dealer(void* i){ // *************************** DEALER
   long seqMod = (long)i;
   pthread_mutex_lock(&mutexdealerDeck);
   cout << "in dealer" << endl;
   populateDeck();      // *** this needs to be only on first round ***
   //printDeck();
   shuffleDeck();
   //printDeck();
   initialDeal();
   cout << "back from initial deal" << endl;
   sequence = seqMod+1;
   cout << "New sequence = " << sequence << endl;
   pthread_cond_broadcast(&threadWait);
   pthread_mutex_unlock(&mutexdealerDeck);
   pthread_exit(NULL); 
}

/***************************************************
**************** Player Draws Card *****************
***************************************************/
void drawCard(Player* &p){
   if(!winner){
      cout << "Player : " << p->playerHand.front() << " is drawing a card" << endl;
      int cardToDraw = dealerDeck->deck.front();
      cout << "Card to deal is " << cardToDraw << endl;
      dealerDeck->deck.pop_front();
      p->playerHand.push_back(cardToDraw);
      displayHand(p);
   }
}

/***************************************************
************** Player Discards a Card **************
***************************************************/
void discard(Player* &p){
   if((p->playerHand.at(1)%100) == (p->playerHand.back()%100)){
    winner = true;
      cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" << endl;
      cout << "   * * * * * * * * * * * * * WINNER! * * * * * * * * * * * *   " << endl;
      cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" << endl;
   }

   else{
//  individual player discards a card
   cout << "entering discard function" << endl;
   srand (seed);
   int toDiscard = rand() % 100 + 1;
   if(toDiscard%2 == 0){
      int cardToDiscard = p->playerHand.at(1);
      p->playerHand.erase (p->playerHand.begin()+1);
      dealerDeck->deck.push_back(cardToDiscard);
      cout << "Player : " << p->playerHand.front() << " is discarding card # :" << cardToDiscard << endl;
      //printDeck();
   }
   else {
      int cardToDiscard = p->playerHand.back();
      p->playerHand.pop_back();
      dealerDeck->deck.push_back(cardToDiscard);
      cout << "Player : " << p->playerHand.front() << " is discarding card # :" << cardToDiscard << endl;
      //printDeck();
   }
   displayHand(p);
   cout << "leaving discard function" << endl;
   }
}

/***************************************************
************** Display Player's Hand ***************
***************************************************/
void displayHand(Player*& p){
   string win;
   cout << "---------------------------------------------------" << endl;
   cout << "PLAYER " << p->playerHand.at(0) << ":" << endl;
   cout << "HAND";
   for (int xii = 1; xii < p->playerHand.size(); xii++){
      cout << " " << p->playerHand.at(xii);
   }
   if(winner) { win = "yes";}
   else { win = "no"; }
   cout << "\nWIN " << win << endl;
   cout << "---------------------------------------------------" << endl;
}

/***************************************************
******************* Player Won *********************
***************************************************/

/*
need to:
display results
write results to log file
players return their cards to the deck
reset sequence
*/
