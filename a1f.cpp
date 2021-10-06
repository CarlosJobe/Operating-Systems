
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include <deque>        // std::deque
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace std;

struct Player
{
   //int playerNo;
   deque<int> playerHand;
   //playerNo -- card1 -- card2
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

// *** Prototypes ***
void populateDeck();
void shuffleDeck();
void printDeck();
//void initialDeal(Player*& players[]);
void initialDeal();
void* dealer(void*);
void* turn(void*);
void drawCard(Player*& p);
void discard(Player*& p);

pthread_mutex_t mutexdealerDeck;
pthread_cond_t threadCounter;
pthread_cond_t threadWait;

int main(){
   
   pthread_t threads[4];
   //pthread_attr_t attr;
   void *status;

   //pthread_attr_init(&attr);   
   pthread_mutex_init(&mutexdealerDeck, NULL);
   pthread_cond_init (&threadCounter, NULL);
   pthread_cond_init (&threadWait, NULL);
   
   for(int i = 0; i <= 3; i++){
      
      players[i]->playerHand.push_front(i);
      cout << "player # " << players[i]->playerHand.front() << endl;
   }

     cout << "entering thread creation loop" << endl;

   for(long i = 0; i < 3; i++){ // 3 rounds of play
      winner = false;
      /*    
      *** need to clear player hands before 2nd and 3rd rounds    

      *** need to create a dealer thread and send it to a deal function
      *** maybe this can include reinitializing the dealer and player decks
      
      */
      //sequence = 0;
      cout << "sequence in main = " << sequence << endl;
      rc = pthread_create(&threads[0], NULL, &dealer, (void *)i); // pass in int to modify seq#
           if (rc){
              cout << "(82)error creating pthread. Error : " << rc << endl;
              exit(-1);
           }
      
      while(!sequence);
      //initialDeal(player);
      //printDeck(dealerDeck);
      cout << "*****************" << endl;
      cout << "***** ROUND *****" << endl;
      cout << "*****  # " << (i+1) << "  *****" << endl;
      cout << "*****************" << endl;

      while(!winner && sequence > 0){
         cout << "in no winner while loop" << endl;
         for(int i = 1; i <= 3; i++){
           //if(sequence > 3) { sequence = 1;}
           rc = pthread_create(&threads[i], NULL, &turn, (void *)players[i]);
           if (rc){
              cout << "error creating pthread. Error : " << rc << endl;
              exit(-1);
           }
         }   
        

        for(int i = 0; i <= 3; i++){
           int t = pthread_join(threads[i], NULL);
           if(t){
              cout << "error creating thread" << endl;
              exit(-1);
           }
           //cout << "after error if in thread join loop" << endl;
        }
      }
      
   }

  pthread_mutex_destroy(&mutexdealerDeck);
  pthread_exit (NULL);
   
}

// *** Functions ***
// populate the deck
 void populateDeck(){
  cout << "populateDeck Function" << endl;
  dealerDeck->deck.clear();
  for(int i = 1; i <= 4; i++){
    for(int ii = 1; ii <= 13; ii++){
      int addCard = (i*100 + ii);
      dealerDeck->deck.push_back(addCard);
    }
  }
 }

// shuffle the deck
  void shuffleDeck(){
  cout << "shuffleDeck Function" << endl;
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  shuffle(dealerDeck->deck.begin(), dealerDeck->deck.end(), default_random_engine(seed));
}

// print the deck contents
void printDeck(){
   cout << "printDeck Function" << endl;
   if(dealerDeck->deck.size() == 0){
      cout << "\nThe deck is empty" << endl;
   }
   else{
      cout << "\nThe deck contains: " << endl;
      for(int i = 0; i < dealerDeck->deck.size(); i++){
         cout << dealerDeck->deck.at(i)  << " " ;
      }
   }
  cout << "\n * * * * * * * * * * * * * * * * * * " << endl;
  return;
}

//  deal initial cards
  void initialDeal(){
   cout << "initialDeal Function" << endl;
   for(int i = 1; i <= 3; i++){
      cout << " about to copy p[0] front to placeholder " << endl;
      int cardToDeal = dealerDeck->deck.front();
      cout << "card to deal = " << cardToDeal << endl;
      cout << " about the pop p0 front " << endl;
      dealerDeck->deck.pop_front();
      cout << " about to copy place holder to " << players[i]->playerHand.at(0) << " players back " << endl;
      players[i]->playerHand.push_back(cardToDeal);
      cout << " done copy place holder to " << i << " players back " << endl;
      cout << "Player " << players[i]->playerHand.front() << "'s card is " << players[i]->playerHand[1] << endl;
      printDeck();
   }
      cout << "leaving initial deal function" << endl;
}

   //**** may need to import int for player number and access player globally
void* turn(void* player){        // *************************** TURN
   Player* p = (Player*)player;
   pthread_mutex_lock(&mutexdealerDeck);
   //long turnCount = (sequence % 4);
   //cout << "turnCount = " << turnCount << endl;
   //while(p->playerNo != turnCount){
   while(p->playerHand.front() != sequence){
      cout << "(turn:185)waiting in player conditional wait loop" << endl;
      cout << "sequence = " << sequence << endl;
      cout << "playerNo = " << p->playerHand.front() << endl;
      pthread_cond_wait(&threadWait, &mutexdealerDeck);
   }
   cout << "out of player conditional wait loop" << endl;
   //if(turn == 0){
      //dealer stuff
   //}
   //else{
   for (int i = 0; i < p->playerHand.size() ; i++){
      cout << "position " << i << " contains " << p->playerHand.at(i) << endl;
   }
   drawCard(p);
   discard(p);
   //}
   cout << "increment seq" << endl;
   if(sequence == 3)
      sequence = 1;
   else
      sequence++;
   cout << "unlock mutex" << endl;
   pthread_mutex_unlock(&mutexdealerDeck);
   cout << "broadcast pthread unlock" << endl;
   pthread_cond_broadcast(&threadWait);
   cout << "exit turn" << endl;
   pthread_exit(NULL);
   
}

void* dealer(void* i){ // *************************** DEALER
   long seqMod = (long)i;
   pthread_mutex_lock(&mutexdealerDeck);
   cout << "in dealer" << endl;
/*    int numNum = players[0]->playerHand.front();
   while(numNum != sequence){
      cout << "waiting in dealer while" << endl;
      pthread_cond_wait(&threadWait, &mutexdealerDeck);
   } */
   cout << "in dealer" << endl;
   printDeck();
   cout << "back from print deck" << endl;
   populateDeck();
   printDeck();
   cout << "back from print deck" << endl;
   shuffleDeck();
   printDeck();
   cout << "back from print deck" << endl;
   initialDeal();
   cout << "back from initial deal" << endl;
   cout << "increment sequence in dealer" << endl;
   sequence = seqMod+1;
   cout << "New sequence = " << sequence << endl;
   pthread_cond_broadcast(&threadWait);
   cout << "unlock mutex" << endl;
   pthread_mutex_unlock(&mutexdealerDeck);
   for(int i = 0; i <= 3; i++){
      //cout << "player # " << p[i].playerNo << endl;
      cout << "player # " << players[i]->playerHand.front() << endl;
   }
   
   cout << "broadcast pthread unlock" << endl;
   //for(int i = 0; i <= 3; i++){
   //   cout << "player # " <<p[i].playerNo << endl;
   //}
   cout << "end of dealer" << endl;
   pthread_exit(NULL); 
}


//  individual player draws a card
void drawCard(Player* &p){
   if(!winner){
      cout << "mutex locked" << endl;
      cout << "entering drawCard function" << endl;

      //Player* p = (Player*)player;
      cout << "Player : " << p->playerHand.front() << " is drawing a card" << endl;
     // winner = false;
      
      int cardToDraw = dealerDeck->deck.front();
      cout << "Card to deal is " << cardToDraw << endl;
      dealerDeck->deck.pop_front();
      p->playerHand.push_back(cardToDraw);
      //cout << p->playerHand[1] << p->playerHand[2] << endl;

      //cout << p->playerHand[1] << p->playerHand.back() << endl;
      for (int i = 0; i < p->playerHand.size() ; i++){
         cout << "Pposition " << i << " contains " << p->playerHand.at(i) << endl;
      }
   }
}

void discard(Player* &p){
   //if((p->playerHand.at(1)%100) == (p->playerHand.at(2)%100)){
   if((p->playerHand.at(1)%100) == (p->playerHand.back()%100)){
    winner = true;
      cout << "winner" << endl;
   }

   else{
//  individual player discards a card
   cout << "entering discard function" << endl;
   unsigned seed = chrono::system_clock::now().time_since_epoch().count();
   srand (seed);
   int toDiscard = rand() % 100 + 1;
   if(toDiscard%2 == 0){
      int cardToDiscard = p->playerHand.at(1);
      p->playerHand.erase (p->playerHand.begin()+1);
      dealerDeck->deck.push_back(cardToDiscard);
      cout << "Player : " << p->playerHand.front() << " is discarding card # :" << cardToDiscard << endl;
      printDeck();
   }
   else {
      //int cardToDiscard = p->playerHand[2];
      int cardToDiscard = p->playerHand.back();
      //p->playerHand.erase (p->playerHand.begin()+2);
      p->playerHand.pop_back();
      dealerDeck->deck.push_back(cardToDiscard);
      cout << "Player : " << p->playerHand.front() << " is discarding card # :" << cardToDiscard << endl;
      printDeck();
   }
 //winner = false;
   cout << "leaving discard function" << endl;
   }
}


