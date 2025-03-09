#include <iostream>
#include <windows.h> 
#include <cstdlib> 
#include <conio.h>
#include <iomanip>
#include <fstream>

using namespace std;

const int BLACK = 0;
const int BLUE = 1;
const int GREEN = 2;
const int CYAN = 3;
const int RED = 4;
const int MAGENTA = 5;
const int YELLOW = 6;
const int WHITE = 7;

bool returntomenu = false;
bool skipNextTurn = false; // Determines if the next player's turn should be skipped
int nextCardDraw = 0;      // Number of cards to draw on the next turn

void clearScreen() 
{
    system("CLS");
}

void setColor(int textColor, int bgColor) 
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

void initializeDeck(int deck[][15]) 
{
    int currentCard = 0; 
    int deckSize = 108;  

    // Initialzing number cards 0 - 9 for each color
    for(int clr = 0; clr < 4; clr++) 
	{
        // Add the single "0" card for each color
        deck[currentCard][0] = clr;    
        deck[currentCard][1] = 0;      
        deck[currentCard][2] = 1;      
        currentCard++;

        // Add two copies of each number card (1 to 9) for the color
        for(int num = 1; num <= 9; num++) 
		{
            for(int duplicate = 0; duplicate < 2; duplicate++) 
			{
                deck[currentCard][0] = clr;
                deck[currentCard][1] = num;
                deck[currentCard][2] = 1;
                currentCard++;
            }
        }

        // Add two copies of each special card (Skip, Reverse, Draw Two)
        for(int special = 10; special <= 12; special++) 
        {
            for(int duplicate = 0; duplicate < 2; duplicate++) 
			{
                deck[currentCard][0] = clr;
                deck[currentCard][1] = special;
                deck[currentCard][2] = 1;
                currentCard++;
            }
        }
    }

    // Add Wild and Wild Draw Four cards to the deck
    for(int wild = 0; wild < 4; wild++) 
	{
        // Wild card
        deck[currentCard][0] = 4;  
        deck[currentCard][1] = 13;  // Value for "Wild"
        deck[currentCard][2] = 1;   // tells that the card exists
        currentCard++;

        // Wild Draw Four card
        deck[currentCard][0] = 4;
        deck[currentCard][1] = 14;  // Value for "Wild Draw Four"
        deck[currentCard][2] = 1;
        currentCard++;
    }
}

void shuffleDeck(int deck[][15]) 
{
    srand(time(0)); 
    const int deckSize = 108; 
    for(int i = deckSize - 1; i > 0; i--) 
	{
        int j = rand() % (i + 1);

        // Swap the properties of two cards
        for(int prop = 0; prop < 15; prop++) 
		{
            int temp = deck[i][prop];
            deck[i][prop] = deck[j][prop];
            deck[j][prop] = temp;
        }
    }
}

void cardHorizontal(int color, int value, int row) 
{
    
    int displayColor;
    switch(color) 
	{
        case 0: displayColor = RED; 
		break;  
        case 1: displayColor = BLUE; 
		break;
        case 2: displayColor = GREEN; 
		break; 
        case 3: displayColor = YELLOW; 
		break; 
        case 4: displayColor = BLACK; 
		break; 
        default: displayColor = BLACK; 
		break;
    }

    setColor(WHITE, displayColor);
    switch(row) 
	{
        case 0: cout<<" ------- "; 
		break; 
        case 1: cout<<" |     | "; 
		break; 
        case 2: { // for Middle row with the card's value or symbol
            cout<<" |  ";
            if(value <= 9) 
			{
                cout<<value<<"  "; // one digit numbers
            } 
			else 
			{
                switch(value) 
				{
					// represnting special cards
                    case 10: cout<<"S  "; 
					break; 
                    case 11: cout<<"R  "; 
					break; 
                    case 12: cout<<"D2 "; 
					break; 
                    case 13: cout<<"W  "; 
					break; 
                    case 14: cout<<"W4 "; 
					break; 
                }
            }
            cout<<"| ";
            break;
        }
        case 3: cout<<" |     | "; 
		break; 
        case 4: cout<<" ------- "; 
		break; 
    }
    setColor(WHITE, BLACK);
}

void PrintPlayerHand(int phand[][15], int pcount, int playernum) 
{
	cout <<endl;
    cout<<"Player "<<playernum<<"'s Hand ( "<<pcount<<" cards):"<<endl;

    // using a to Loop through each row of the card representation
    for(int row = 0; row < 5; row++) 
	{ // Each card has 5 rows
        for(int i = 0; i < pcount; i++) 
		{
            cardHorizontal(phand[i][0], phand[i][1], row);

            // Adding a space between cards but excludes after the last card
            if(i < pcount - 1) 
			{
                cout<<"  "; 
            }
        }
        cout<<endl; 
    }
}

// function to add a card to a player's hand 
bool addCards(int playerhand[][15], int &playercount, int card[15], const int maxCards) 
{
    if(playercount >= maxCards) 
	{
        return false; // Cannot add more cards if the hand is full with cards
    }
    for(int i = 0; i < 15; i++) 
	{
        playerhand[playercount][i] = card[i];
    }
    playercount++; // its Incrementing  the count of cards in the player's hand
    return true;
}

// Function to remove a card from a player's hand
bool removeCards(int playerhand[][15], int &playercount, int cardPos) 
{
    if(cardPos < 0 || cardPos >= playercount) 
	{
        return false; // for invalid card position
    }

    // Shift all cards after the removed card to the left
    for(int i = cardPos; i < playercount - 1; i++) 
	{
        for(int j = 0; j < 15; j++) 
		{
            playerhand[i][j] = playerhand[i + 1][j];
        }
    }
    playercount--; // Decrementing  the count of cards in the player's hand
    return true;
}
void handleSpecialCard(int specialCardType) 
{
    skipNextTurn = false;
    nextCardDraw = 0;

    switch(specialCardType) 
	{
        case 10: 
		// 10 represnets the Skip card, true then next player turn skipped
            skipNextTurn = true;
			cout <<endl; 
            cout<<"Skip Card Played! Next Player's Turn is Skipped"<<endl;
            cout<<"Press any key to continue..";
            getch();
			//show this untila a key is pressed
            break;
        case 11: 
		// Reverse card which reverses the order of turns used just like skip
            skipNextTurn = true;  
            cout<<"Reverse Card Played!"<<endl;
            getch();
            break;
        case 12: 
		// Draws two card and also skips the turn
            nextCardDraw = 2;
            skipNextTurn = true; 
            cout<<"Draw Two Played!"<<endl;
            getch();
            break;
        case 13: 
		// Wild card
            cout<<"Wild card played! Color has been changed"<<endl;
            getch();
            break;
        case 14: 
		// Wild draw four card skipping and drawing four
            nextCardDraw = 4; 
            skipNextTurn = true; 
            cout<<"Wild Draw Four!"<< endl;
            getch();
            break;
    }
}

bool isValidPlay(int cardToPlay[15], int topCard[15]) 
{
    // Wild card will always be played
    if (cardToPlay[0] == 4)
	{
    	return true;			
	}  

    // we can play wild cards on any cards
    if (topCard[0] == 4)
	{  
    	return true;
	}
	
    // Match by color or symbol
    if (cardToPlay[0] == topCard[0] || cardToPlay[1] == topCard[1])
    {
     	return true; // true For the Colors 
	}

 // if theres not any matche found 
    return false;
}

void drawMultipleCards(int phand[][15], int &pcount, int deck[][15], int &deckPosition, const int maxCards) 
{
	int deckSize = 108;
    for(int i = 0; i < nextCardDraw && deckPosition < deckSize; i++) 
	{
        int newCard[15];
        for (int j = 0; j < 15; j++) 
		{
            newCard[j] = deck[deckPosition][j]; //taking the next card from the deck
        }
        if(addCards(phand, pcount, newCard, maxCards)) // Add the card to the player's hand
		{ 
            deckPosition++; // MoVe to the next card in the deck
        }
    }
    nextCardDraw = 0; //reseting 
}

bool callUno(int playerCount) 
{
    char call;
    do 
    {
        cout<<"You have "<<playerCount<<" cards left"<<endl;
        cout<<"Do you want to call uno or not yet!"<<endl;
        cin>>call;
        
        // Check for valid inputs
        if(call == 'U' || call == 'u') 
        { 
            if(playerCount == 2) 
            {
                cout<<"Uno Called"<<endl;
                getch();
                return true;
            }
            else 
            {
                cout<<"False Uno Call! Drawing Penalty!"<<endl; 
                getch();
                return false;
            }
        }
        else if(call == 'N' || call == 'n')
        {
            if(playerCount == 2) 
            {
                cout<<"Not Called Uno - Drawing Penalty!"<<endl; 
                getch();
                return false;
            }
            return true;
        }
        else 
        {
            // Error message for invalid input
            cout<<"Invalid input! Please enter 'U' or 'N'."<<endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
    } 
    while(true); // Continue loop until valid input is received
}


void printBoard(int p1hand[][15], int p2hand[][15], int p1count, int p2count, int currentPlayer, int discardPile[][15], int discardCount) 
{
    clearScreen(); // Clear the console screen
    cout<<"=====================UNO GAME====================="<<endl;
    cout<<"Current Player: Player "<<currentPlayer<<endl<<endl;
    
    // Display the current player's hand
    if(currentPlayer == 1) 
	{
        PrintPlayerHand(p1hand, p1count, 1);
    }
	else 
	{
        PrintPlayerHand(p2hand, p2count, 2);
    }
    
    // Display the top card in the discard pile
    cout<<"\nDiscard Pile Top Card:"<<endl;
    if(discardCount > 0) 
	{
        for(int row = 0; row < 5; row++) // Print each row of the card
		{ 
            cardHorizontal(discardPile[discardCount - 1][0], discardPile[discardCount - 1][1], row);
            cout<<endl;
        }
    }
    
    // Display the remaining cards for both players
    cout<<"\nCards Remaining:"<<endl;
    cout<<"Player 1: "<<p1count<<" Cards"<<endl;
    cout<<"Player 2: "<<p2count<<" Cards"<<endl;
}

void updateDiscardPile(int discardPile[][15], int &discardCount, int playedCard[][15]) 
{
    for(int i = 0; i < 15; i++) 
	{
        discardPile[discardCount][i] = playedCard[0][i]; // Add the played card to the discard pile
    }
    discardCount++; // Increment the count of cards in the discard pile
}

// Function to initialize the discard pile with the first playable card from the deck
void startDiscardPile(int deck[][15], int discardPile[][15], int &discardCount) 
{
    int start = 0;
    while(deck[start][0] == 4 && deck[start][1] == 14) // Skip wild cards during initialization
	{ 
        start++;
    }
    int tempCard[1][15];
    for(int i = 0; i < 15; i++) 
	{
        tempCard[0][i] = deck[start][i]; // Select the first non-wild card
    }
    updateDiscardPile(discardPile, discardCount, tempCard); // Add it to the discard pile
}

// Function to get the top card of the discard pile
void getTopCard(int discardPile[][15], int discardCount, int topCard[15]) 
{
    if(discardCount > 0) 
	{ 
        for (int i = 0; i < 15; i++) 
		{
            topCard[i] = discardPile[discardCount - 1][i]; // Get the last card from the pile
        }
    }
}

void dealCards(int deck[][15], int p1hand[][15], int p2hand[][15], int &p1count, int &p2count, const int FirstCards) 
{
    int deckPosition = 0; // Start at the top of the deck
    p1count = 0;
    p2count = 0;
    
    // Deal the specified number of cards to each player
    for(int i = 0; i < FirstCards; i++) 
	{
        for(int j = 0; j < 15; j++) 
		{
            p1hand[i][j] = deck[deckPosition][j]; // Deal to Player 1
            p2hand[i][j] = deck[deckPosition + 1][j]; // Deal to Player 2
        }
        p1count++; // Increase Player 1's card count
        p2count++; // Increase Player 2's card count
        deckPosition += 2; // Move to the next set of cards
    }
}

void playTurn(int playerHand[][15], int &playerCount, int &deckPosition, int deck[][15], int discardPile[][15], int &discardCount,
              const int deckSize, const int maxCards, int &currentPlayer) 
{  
    // Array to hold the top card of the discard pile
    int topCard[15]; 
    char pause, resume; // Variables to handle pause/resume functionality
    
    // Get the top card of the discard pile
    getTopCard(discardPile, discardCount, topCard);
    
    // Handle special card effects 
    if(nextCardDraw > 0) 
	{
        drawMultipleCards(playerHand, playerCount, deck, deckPosition, maxCards);
        if(skipNextTurn) 
		{
            skipNextTurn = false; // Reset the skip effect
            currentPlayer = (currentPlayer == 1) ? 2 : 1; // Switch player
            return; // End the turn
        }
    }
    
    // If the player's turn is skipped, show a message and switch players
    if(skipNextTurn) 
	{
        skipNextTurn = false;
        cout<<"Your Turn Skipped"<<endl;
        getch(); // Wait for input before proceeding
        currentPlayer = (currentPlayer == 1) ? 2 : 1; // Switch player
        return;
    }
    
    // Display menu for player's turn
    char choice;
    resume:
    cout<<"\nOptions: "<<endl;
    cout<<"1. Play a Card"<<endl;
    cout<<"2. Draw a Card"<<endl;

    // loop validation for the player choice
    do 
	{
        cout<<"Enter Your Choice (1-2) or E to exit: ";
        cin>>choice;
        if(choice == 'E' || choice == 'e') 
		{ 
            // 
            do
			{
                cout<<"Game Paused!"<<endl;
                cout<<"Are you sure you want to Exit the game! (Y/N): ";
                cin>>pause;
                
                if(pause != 'Y' && pause != 'y' && pause != 'N' && pause != 'n') 
				{
                    cout<<"Invalid Input! Try Again (Y or N)"<<endl;
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
            } 
			while(pause != 'Y' && pause != 'y' && pause != 'N' && pause != 'n');

            if(pause == 'Y' || pause == 'y') 
			{
                cout<<"Going to main menu"<<endl;
                returntomenu=true; 
                return;
            } 
			else 
			{
                cout<<"Resuming the Game!"<<endl;
                goto resume;
            }
        }
        if(choice != '1' && choice != '2') 
		{
            cout<<"Invalid Input! Enter Again (1 or 2): "<<endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
    } 
	while (choice != '1' && choice != '2');    
    
    // Handle card play (choice 1)
    if(choice == '1') 
	{
        int cardPosition;
        do 
		{
            // Prompt user to select a card from their hand
            cout<<"Enter the card position to play (0-"<< playerCount - 1<<"): ";
            cin>>cardPosition;
            if(cin.fail() || cardPosition < 0 || cardPosition >= playerCount) {
                cout<<"Invalid position. Please enter a valid card position."<<endl;
                cin.clear();
                cin.ignore(10000, '\n');
            }
        } 
		while (cin.fail() || cardPosition < 0 || cardPosition >= playerCount);
        
        // Get the card the player wants to play
        int cardToPlay[15];
        for(int i = 0; i < 15; i++) 
		{
            cardToPlay[i] = playerHand[cardPosition][i];
        }
        
        // Check if the play is valid
        if(isValidPlay(cardToPlay, topCard)) 
		{
            if(!callUno(playerCount)) 
{
    // If Uno was falsely called or not called when having two cards
    for(int i = 0; i < 2 && deckPosition < deckSize; i++) 
    {
        int newCard[15];
        for(int j = 0; j < 15; j++) 
        {
            newCard[j] = deck[deckPosition][j];
        }
        if(addCards(playerHand, playerCount, newCard, maxCards)) 
        {
            deckPosition++;
        }
    }
    currentPlayer = (currentPlayer == 1) ? 2 : 1; 
    return;
}
            
            // Handle Wild card color change
            if(cardToPlay[0] == 4) 
			{
                int newColor;
                bool valid = false;
                while(!valid) 
				{
                    cout<<"Choose a Color (0:Red, 1:Blue, 2:Green, 3:Yellow): ";
                    cin>>newColor;
                    
                    if(cin.fail()) 
					{
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout<<"Invalid Input! Enter (0-3): "<<endl;
                    } 
					else 
					{
                        if(newColor >= 0 && newColor <= 3) 
						{
                            cardToPlay[0] = newColor; // Set the new color for the Wild card
                            valid = true;
                        } 
						else 
						{
                            cout<<"Invalid Color! Choose from (0-3): "<<endl;
                        }
                    }
                }
            }
            
            updateDiscardPile(discardPile, discardCount, &cardToPlay);
            removeCards(playerHand, playerCount, cardPosition);
            handleSpecialCard(cardToPlay[1]); 
            currentPlayer = (currentPlayer == 1) ? 2 : 1; 
            return;
        }
        cout<<"\nInvalid Play!"<<endl;
        getch();
        return;
    }
    
    // Handle drawing a card (choice 2)
    if(choice == '2' && deckPosition < deckSize) 
	{
        int newCard[15];
        for(int i = 0; i < 15; i++) 
		{
            newCard[i] = deck[deckPosition][i];
        }
        deckPosition++; // Move to the next card in the deck
        if(addCards(playerHand, playerCount, newCard, maxCards)) 
		{
            cout<<"\nCard drawn!"<<endl;
            getch();
            currentPlayer = (currentPlayer == 1) ? 2 : 1; 
            return;
        }
    }
    
    cout<<"\nNo cards left in deck!"<<endl;
    getch();
    return;
}
void saveLoadGameResult(bool player1, bool player2, int p1count, int p2count, int discardCount, int deckPosition) 
{
    int p1winner = 0, p2winner = 0; 
    
    // Read existing wins for Player 1 from the file
    ifstream storewinner("results1.txt");
    if(storewinner.is_open()) 
	{
        storewinner >> p1winner; 
        storewinner.close();
    }
    
    // Read existing wins for Player 2 from the file
    storewinner.open("results2.txt");
    if(storewinner.is_open()) 
	{
        storewinner >> p2winner; 
        storewinner.close();
    }
    
    // Update win counts based on the game's winner
    if(player1) 
	{
        p1winner++; 
    } 
	else if(player2) 
	{
        p2winner++; 
    }
    
    // Save updated win counts back to the respective files
    ofstream updwinner1("results1.txt");
    if(updwinner1.is_open()) 
	{
        updwinner1 << p1winner; // Save Player 1's updated win count
        updwinner1.close();
    }
    
    ofstream updwinner2("results2.txt");
    if(updwinner2.is_open()) 
	{
        updwinner2 << p2winner; // Save Player 2's updated win count
        updwinner2.close();
    }
    
    // Log the game's statistics in a separate game log file
    ofstream historyFile("game_logs.txt", ios::app); // Open in append mode
    if(historyFile.is_open()) 
	{
        historyFile<<"=== Game Log ==="<<endl;
        
        // Record the winner in the log
        if(player1)
		{
            historyFile<<"Winner: Player 1"<<endl;
        }
		else if(player2) 
		{
            historyFile<<"Winner: Player 2"<<endl;
        } 
		else
		{ 
            historyFile<<"Result: Tie"<<endl; 
        }
        
        // Log of all the game results
        historyFile<<"Player 1 Remaining Cards: "<<p1count<<endl;
        historyFile<<"Player 2 Remaining Cards: "<<p2count<<endl;
        
        historyFile<<"Discard Pile Size: " <<discardCount<<endl;
        historyFile<<"Remaining Deck Cards: "<<(108 - deckPosition)<<endl; // subtracting total card form the current deck's position
        
      cout <<endl;
        historyFile<<"Total Wins:"<<endl;
        historyFile<<"Player 1: "<< p1winner<<" times"<<endl;
        historyFile<<"Player 2: "<<p2winner<<" times"<<endl;
        historyFile.close(); 
    }
    
 
    cout<<"Winner History"<<endl;
    cout<<"Player 1 has won "<< p1winner<<" Times"<<endl;
    cout<<"Player 2 has won "<< p2winner<<" Times"<<endl;
}
void storedHistory() 
{
    int p1winner = 0, p2winner = 0; 
    
    // previous result loading with ifstream
    ifstream loadwinner("results1.txt");
    if(loadwinner.is_open()) 
	{
		 
        loadwinner >> p1winner; 
        loadwinner.close();
    } 
	else 
	{
	//if no data is already there
        cout<<"No previous results on Player 1's score file!"<<endl; 
    }
    
// Loading  Player 2's win count from the file in which its stored
    ifstream loadwinner2("results2.txt");
    if(loadwinner2.is_open()) 
	{
	// Reading  Player 2's win count 	
        loadwinner2 >> p2winner; 
        loadwinner2.close();
    } 
	else 
	{
        cout<<"No previous results on Player 2's score file!"<<endl; // No data is already there
    }
    
  cout<<endl;
    cout<<"================= SCORE BOARD =================" << endl;
    cout<<"Player 1 has won: "<<p1winner<<" times"<<endl; 
    cout<<"Player 2 has won: "<<p2winner<<" times"<<endl; 
    cout<<"============================================="<<endl;
    cout<<endl;
    cout<<"Press a key for menu...";
    getch(); 
}
int main() 
{
    int option; // Variable to store the user's menu choice
    do 
	{
        clearScreen();
        cout<<"==============================================================="<<endl;
        cout<<"=====================Menu of Game=============================="<<endl;
        cout<<"==============================================================="<<endl;
        cout<<"Select From Below Options"<<endl;
        cout<<"1. Start the Game"<<endl; 
        cout<<"2. Check out Score"<<endl; 
        cout<<"3. Exit"<<endl; 
        cout<<"Enter your choice (1-3): ";
        
        cin>>option; 
        cin.clear();    // Clear any input errors
        cin.ignore(10000, '\n'); // Ignore leftover characters in the input buffer

        switch(option) 
		{
            case 1: 
			{
                cout<<"Starting the Game......"<<endl;

                const int deckSize = 108;          // Total cards in the deck
                const int Max_Card_PP = 50;       // Maximum cards a player can hold
                const int firstCards = 7;         // Initial number of cards dealt to each player
 
                int deck[deckSize][15];           // Array to represent the deck of cards
                int discardPile[deckSize][15];    // Array to represent the discard pile
                int discardCount = 0;             // Number of cards in the discard pile
                int deckPosition = firstCards * 2; // Position in the deck after dealing initial cards
                
                // Reset flag for returning to the menu
                returntomenu = false;

                initializeDeck(deck);
                shuffleDeck(deck);
               
                int player1hand[Max_Card_PP][15]; 
                int player2hand[Max_Card_PP][15]; 
                int player1count = 0;             
                int player2count = 0;             
                
                dealCards(deck, player1hand, player2hand, player1count, player2count, firstCards);
                startDiscardPile(deck, discardPile, discardCount);
                deckPosition++; // Move past the first card used in the discard pile
                
                int currentPlayer = 1;            // Tracks the current player's turn 1or2
                bool gameOver = false;            // bool to check if the game is over
                bool p1Winner = false;            // bool to tell if which  Players is the winner
                bool p2Winner = false;            
                
                while(!gameOver) 
				{
                    printBoard(player1hand, player2hand, player1count, player2count, currentPlayer, discardPile, discardCount);
                    
                    if(currentPlayer == 1) 
					{
                        playTurn(player1hand, player1count, deckPosition, deck, discardPile, discardCount, deckSize, Max_Card_PP, currentPlayer);
                        if (player1count == 0) { // Check if Player 1 has won
                            gameOver = true;
                            p1Winner = true;
                            cout<<"\nPlayer 1 Wins!"<<endl;
                            break;
                        }
                    } 
                    // Handle Player 2's turn
                    else 
					{
                        playTurn(player2hand, player2count, deckPosition, deck, discardPile, discardCount, deckSize, Max_Card_PP, currentPlayer);
                        if(player2count == 0) 
						{ 
                            gameOver = true;
                            p2Winner = true;
                            cout << "\nPlayer 2 Wins!" << endl;
                            break;
                        }
                    }
                    
                    // Break the loop if the game returns to the menu
                    if(returntomenu) 
					{
                        break;
                    }
                }
                
                // Save the game results if a winner was determined
                if(gameOver) 
				{
                    saveLoadGameResult(p1Winner, p2Winner, player1count, player2count, discardCount, deckPosition);
                }
                
                cout<<"\nPress any key to return to menu...";
                getch(); // Wait for user input before returning to the menu
                break;
            }
            case 2: 
			{
                storedHistory();
                break;
            }
            
            case 3: 
			{
                cout<<"Thanks for playing! Goodbye!"<<endl;
                return 0; 
            }
            default: 
			{
                cout<<"Invalid Input! Press any key to try again..."<<endl;
                getch(); // Wait for user input before redisplaying the menu
                break;
            }
        }
    } 
	while(true); // Loop until  exit
    return 0; 
}
