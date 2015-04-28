#include <iostream.h>
#include <windows.h>
#include <stdio.h>
#include <winbase.h>
#include <time.h>

#define SHIPTHERE 1
#define SEATHERE 0
#define HITTHERE 2

#define DEST 2
#define SUB 3
#define CRUS 3
#define BESP 4
#define AIRC 5

//Foreground Colors
#define BLUE		0x0010
#define GREEN		0x0020
#define RED			0x0040
#define INTENS		0x0080

#define fBLUE		0x0001
#define fGREEN		0x0002
#define fRED		0x0004
#define fINTENS		0x0008

//Structure that contains the coordinates for the entire game
typedef struct {
	int ID;
	int status;							//0= nothing there, 1= ship there, 2= hit ship there
	char type;
	WORD color;
} unit;

typedef struct {
	int x;
	int y;
} COOR;

typedef struct {
	int degr;							//The number of degrees, either 90, 180, or 270
	char orion;
} DIRECT;

typedef struct {
	COOR front;
	DIRECT dir;
	char name[16];							//THe name, Battleship, sub, etc.
	char type;								//The Type of ship
	int dim;
	bool active;
	WORD color;
} ship;

typedef struct {
	ship dest;
	ship sub;
	ship crus;
	ship battle;
	ship carr;
	int admiral;
} fleet;

//Function Prototypes
int menu();									//The Menu function that displays the main menu
void menuFunc(int option);					//"Menu Function" function
char changeOp();							//"Change Option" function
void about();								//The About function that displays the information
void makeBoard(unit board[10][10]);							//Function that creates the board
int setPiece(ship *ship1, fleet fleet1, unit board[10][10], COOR start, char dir);	//The function that sets the pieces on the board
void refreshBoard(unit Disp[10][10]);		//Refreshes the board
void setup();
void game();
void key();
void aisetup();
int target(fleet fleet1, COOR trget, unit trgetBoard[10][10], unit gboard[10][10]);
void showGuesses(unit gb[10][10]);
void autoSet();

//Global Variables
bool cont;									//The variable that is for the main while loop
bool end;									//Variable for main while loop
unit board1[10][10];						//The Player 1 Board for Game
unit board1g[10][10];
unit board2[10][10];						//The Player 2 Board for Game
unit board2g[10][10];
fleet play[2];								//The fleets of each player, 0=ai, 1=player
WORD color;
WORD wAttributesOld;
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hConsole;
int keyTemp;
bool keydone;
char shipnames[5][12] = {"Destroyer" , "Submarine" , "Cruiser" , "Battleship" , "Aircarrier"};
bool set;

main() {

	if( ( hConsole = CreateFile("CONOUT$", GENERIC_WRITE | GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,0L, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0L) )
                   == (HANDLE) -1 ) {
        printf("\nError: Unable to open console.\n");
        return( -1 );
    }
	wAttributesOld = csbi.wAttributes;

	srand((unsigned)time(NULL));


	//SetConsoleTextAttribute( hConsole, color);

	COOR UrEnt;								//The multiuse varable for Usr entered Coordinates
	char name[8];							//The name of the player
	int option;								//The option variable that holds the value of the option the user selected

	setup();
	makeBoard(board1);
	makeBoard(board2);

	end = false;
	cont = false;
	cout << "Welcome to Battleship\n\n";
	cout << "Enter your name: ";
	cin >> name;

	//refreshBoard(board1);
	//Setup
	play[1].admiral = 1;						//Sets the First fleet to player 1
	play[0].admiral = 0;						//Sets the second fleet to computer

	cout << "\n";
	aisetup();
	while(!end &&  !cont) {
		option = menu();
		menuFunc(option);
	}

	return 0;
}

void game() {
	int x;
	cont = false;
	char option;
	int z;
	COOR ships;
	char dir;


	if (!set) {
		for(z=0; z<5; z++) {
			refreshBoard(board1);
			cout << "\nEnter the Location you want to put your " << shipnames[z] << ": ";
			cin >> ships.x;
			cin >> ships.y;

			if(board1[ships.x][ships.y].status == 0) {
				cout << "\nEnter the Direction (u, d, l, r) ";
				cin >> dir;

				switch(z) {
					case 0:
						if((x = setPiece(&play[1].dest, play[1], board1, ships, dir)) == 0) {
							z--;
						}
						break;
					
					case 1:
						if((x = setPiece(&play[1].sub, play[1], board1, ships, dir)) == 0) {
							z--;
						}
						break;
					case 2:
						if((x = setPiece(&play[1].crus, play[1], board1, ships, dir)) == 0) {
							z--;
						}
						break;

					case 3:
						if((x = setPiece(&play[1].battle, play[1], board1, ships, dir)) == 0) {
							z--;
						}
						break;

					case 4:
						if((x = setPiece(&play[1].carr, play[1], board1, ships, dir)) == 0) {
							z--;
						}
						break;


				}
			}
			else {
				z--;
			}
		}
	}

	COOR hit;

	while (!end && !cont) {
		refreshBoard(board1);
		cout << "\n\nEnter your command (h for help):";
		//refreshBoard(board2);
		cin >> option;
		if(option == 'h') {
			system("CLS");
			cout << "Help\n--------------------------\n";
			cout << "h- help\nt- target\nc- Check Guesses/Hits\ne- exit\n\n";

		}
		else if(option == 't') {
			system("CLS");
			cout << "What coordinates do you want to hit? ";
			cin >> hit.x;
			cin >> hit.y;
			x = target(play[1], hit, board2, board1g);
		}
		else if(option == 's') {
			system("CLS");
			showGuesses(board1g);
		}
	}
}

void showGuesses(unit gb[10][10]) {
	int x, y;
	system("CLS");
	cout << "Guessed Spots\n------------------------\n";
	for(x=0; x<10; x++) {
		//cout << x << "|";
		cout << x << "|";
		for(y=0; y<10; y++) {
			if(gb[x][y].status == 0) {
				cout << "ø|";
			}
			else if(gb[x][y].status == 1) {
				cout << "?|";
			}
			else if(gb[x][y].status == 2) {
				cout << "X|";
			}
		}
		cout << "\n";
	}
	cout << " |0|1|2|3|4|5|6|7|8|9|\n";



}

int target(fleet fleet1, COOR trget, unit trgetBoard[10][10], unit gboard[10][10]) {
	if(trgetBoard[trget.x][trget.y].status == 1) {
		trgetBoard[trget.x][trget.y].status = 2;
		if(fleet1.admiral == 1) {
			MessageBox(NULL, "You hit a ship!", "HIT!", MB_OK );
			gboard[trget.x][trget.y].status = 2;
		}
		else {
			gboard[trget.x][trget.y].status = 0;	
		}
	}
	return 1;
}

void aisetup() {
	COOR start;
	char dir;
	int x;
	int y;
	int z;

	for(x=0; x<5; x++) {
		start.x = (rand()%10) + 1;
		start.y = (rand()%10) + 1;
		y = (rand()%3) + 0;
		switch(y) {
			case 0:
				dir = 'r';
				break;
			case 1:
				dir = 'd';
				break;
			case 2:
				dir = 'u';
				break;
			case 3:
				dir = 'l';
				break;
		}
		switch(x) {
			case 0:
				if((z = setPiece(&play[0].dest, play[0], board2, start, dir) == 0)) {
					x--;
				}
				break;
			case 1:
				if((z = setPiece(&play[0].sub, play[0], board2, start, dir) == 0)) {
					x--;
				}
				break;

			case 2:
				if((z = setPiece(&play[0].crus, play[0], board2, start, dir) == 0)) {
					x--;
				}
				break;

			case 3:
				if((z = setPiece(&play[0].battle, play[0], board2, start, dir) == 0)) {
					x--;
				}
				break;

			case 4:
				if((z = setPiece(&play[0].carr, play[0], board2, start, dir) == 0)) {
					x--;
				}
				break;

		}
	}

}

//Set Pieces Function
int setPiece(ship *ship1, fleet fleet1, unit board[10][10], COOR start, char dir) {
	int x, y;
	bool goodPoint = true;
	int z, w;

				x = start.x;
				y = start.y;
				ship1->front.x = x;
				ship1->front.y = y;
				board[x][y].type = ship1->type;
				board[x][y].status = 1;
				ship1->dir.orion = dir;
				if(dir == 'u' || dir == 'U') {
					ship1->dir.degr = 0;
					board[ship1->front.x][ship1->front.y].type=ship1->type;
					board[ship1->front.x][ship1->front.y].status = 1;
					board[ship1->front.x][ship1->front.y].color = ship1->color;
					for(z=x-1; z > x - ship1->dim; z--) {
						if (board[z][ship1->front.y].status == 0 && board[x][y].type == ship1->type && z > 0) {
							board[z][ship1->front.y].type= ship1->type;
							board[z][ship1->front.y].status = 1;
							board[z][ship1->front.y].color = ship1->color;

							//board[x-ship1->dim+1][y].status = 1;
							//board[x-ship1->dim+1][y].type = ship1->type;
						}
						else {
							for(z=x; z > x - ship1->dim; z--) {
								if(board[z][ship1->front.y].type == ship1->type) {
									board[z][ship1->front.y].status = 0;
									board[z][ship1->front.y].type = ' ';
									board[ship1->front.x][z].color = NULL;
									board[ship1->front.x][ship1->front.y].color = NULL;
									//board[x-ship1->dim+1][y].status = 0;
									//board[x-ship1->dim+1][y].type = ' ';
								}
							}
							
							goodPoint = false;
							break;
							
						}
					}
				}
				else if(dir == 'd' || dir == 'D') {
					ship1->dir.degr = 180;
					board[ship1->front.x][ship1->front.y].type=ship1->type;
					board[ship1->front.x][ship1->front.y].status = 1;
					board[ship1->front.x][ship1->front.y].color = ship1->color;
					for(z=x+1; z < x + ship1->dim; z++) {
						if (board[z][ship1->front.y].status == 0 && board[x][y].type == ship1->type && z < 10) {
							board[z][ship1->front.y].type= ship1->type;
							board[z][ship1->front.y].status= 1;
							board[z][ship1->front.y].color = ship1->color;

							//board[x+ship1->dim-1][y].status = 1;
							//board[x+ship1->dim-1][y].type = ship1->type;
							
						}
						else {
							for(z=x; z < x + ship1->dim; z++) {
								if(board[z][ship1->front.y].type == ship1->type) {
									board[z][ship1->front.y].status = 0;
									board[z][ship1->front.y].type = ' ';
									board[ship1->front.x][z].color = NULL;
									board[ship1->front.x][ship1->front.y].color = NULL;
									//board[x+ship1->dim-1][y].status = 0;
									//board[x+ship1->dim-1][y].type = ' ';
								}
							}
							
							goodPoint = false;
							break;
							
						}
					}

				}
				else if(dir == 'l' || dir == 'L') {
					ship1->dir.degr = 270;
					board[ship1->front.x][ship1->front.y].type=ship1->type;
					board[ship1->front.x][ship1->front.y].status = 1;
					board[ship1->front.x][ship1->front.y].color = ship1->color;
					for(z=y-1; z > y - ship1->dim; z--) {
						if (board[ship1->front.x][z].status == 0 && board[x][y].type == ship1->type &&  z > 0) {
							board[ship1->front.x][z].type= ship1->type;
							board[ship1->front.x][z].status = 1;
							board[ship1->front.x][z].color = ship1->color;

							//board[x][y-ship1->dim+1].status = 1;
							//board[x][y-ship1->dim+1].type = ship1->type;
						}
						else {
							for(z=y; z > y - ship1->dim; z--) {
								if(board[ship1->front.x][z].type == ship1->type) {
									board[ship1->front.x][z].status = 0;
									board[ship1->front.x][z].type = ' ';
									board[ship1->front.x][z].color = NULL;
									board[ship1->front.x][ship1->front.y].color = NULL;
									//board[x][y-ship1->dim+1].status = 0;
									//board[x][y-ship1->dim+1].type = ' ';
									
								}
							}
							
							goodPoint = false;
							break;
							
						}
					}

				}
				else if(dir == 'r' || dir == 'R') {
					ship1->dir.degr = 90;
					board[ship1->front.x][ship1->front.y].type=ship1->type;
					board[ship1->front.x][ship1->front.y].status = 1;
					board[ship1->front.x][ship1->front.y].color = ship1->color;
					for(z=y+1; z < y + ship1->dim; z++) {
						if (board[ship1->front.x][z].status == 0 && board[x][y].type == ship1->type && z < 10) {
							board[ship1->front.x][z].type= ship1->type;
							board[ship1->front.x][z].status = 1;
							board[ship1->front.x][z].color = ship1->color;

							//board[x][y+ship1->dim-1].status = 1;
							//board[x][y+ship1->dim-1].type = ship1->type;
						}
						else {
							for(z=y; z < y + ship1->dim; z++) {
								if(board[ship1->front.x][z].type == ship1->type) {
									board[ship1->front.x][z].status = 0;
									board[ship1->front.x][z].type = ' ';
									board[ship1->front.x][z].color = NULL;
									board[ship1->front.x][ship1->front.y].color = NULL;
									//board[x][y+ship1->dim-1].status = 0;
									//board[x][y+ship1->dim-1].type = ' ';
								}
							}
							
							goodPoint = false;
							break;
							
						}
					}

					}
		
	system("CLS");
	return 1;
}

//The codes for the board matrix, that translates the numbers into Data

//Functions

//Refresh Board
void refreshBoard(unit Disp[10][10]) {
	int x;
	int y;

	keyTemp = 1;
	keydone = false;
	system("CLS");
	
	printf("\nYour Board\n----------------------------------------\n\n");
	for(x=0; x<10; x++) {
		//cout << x << "|";
		printf("%d|",x);
		for(y=0; y<10; y++) {
			SetConsoleTextAttribute(hConsole, Disp[x][y].color | fRED | fGREEN | fBLUE);
			if(Disp[x][y].status == 2) {
				SetConsoleTextAttribute(hConsole, Disp[x][y].color);
				printf("ø");
			}
			else if(Disp[x][y].status == 2) {

			}
			else {
				printf(" ");
			}
			SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);
			printf("|");							//Should be .type
		}
		key();
		printf("\n");
	}
	printf(" |0|1|2|3|4|5|6|7|8|9|\n");
}

void key() {
	if (!keydone){
		switch (keyTemp) {
		case 1:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].dest.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Destroyer");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 2:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].sub.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Submarine");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 3:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].crus.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Crusier");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 4:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].battle.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Battleship");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 5:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].carr.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Air Carrier");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);
				keydone = true;
				break;
		}
		keyTemp++;
	}
}

//Initial Setup
void setup() {
	int x, y;
	//1 = player 1
	keydone = false;
	keyTemp = 1;

	play[1].dest.dim = DEST;
	play[1].dest.type = 'd';
	strcpy(play[1].dest.name,"Destroyer");
	play[1].dest.color = BLUE;
	play[1].sub.dim = SUB;
	play[1].sub.type='s';
	strcpy(play[1].sub.name,"Submarine");
	play[1].sub.color = RED;
	play[1].crus.dim = CRUS;
	play[1].crus.type = 'c';
	strcpy(play[1].crus.name,"Cruiser");
	play[1].crus.color = GREEN;
	play[1].battle.dim = BESP;
	play[1].battle.type = 'b';
	strcpy(play[1].battle.name,"Battleship");
	play[1].battle.color = RED | GREEN;
	play[1].carr.dim = AIRC;
	play[1].carr.type = 'a';
	strcpy(play[1].carr.name,"Carrier");
	play[1].carr.color = RED | BLUE;

	//0 = AI
	play[0].dest.dim = DEST;
	play[0].dest.type = 'd';
	strcpy(play[0].dest.name,"Destroyer");
	play[0].dest.color = BLUE;
	play[0].sub.dim = SUB;
	play[0].sub.type = 's';
	strcpy(play[0].sub.name,"Submarine");
	play[0].sub.color = RED;
	play[0].crus.dim = CRUS;
	play[0].crus.type = 'c';
	strcpy(play[0].crus.name,"Cruiser");
	play[0].crus.color = GREEN;
	play[0].battle.dim = BESP;
	play[0].battle.type = 'b';
	strcpy(play[0].battle.name,"Battleship");
	play[0].battle.color = RED | GREEN;
	play[0].carr.dim = AIRC;
	play[0].carr.type = 'a';
	strcpy(play[0].carr.name,"Carrier");
	play[0].carr.color = RED | BLUE;

	for(x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board1[x][y].status = 0;
			board2[x][y].status = 0;
			board1[x][y].color = fRED | fGREEN | fBLUE;
			board2[x][y].color = fRED | fGREEN | fBLUE;

		}
	}

	
	for(x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board1g[x][y].status = 1;
			board2g[x][y].status = 1;
		}
	}
	set = false;
}

//Menu function
int menu() {
	int option;
	cout << "Menu\n------------------------------\n";
	cout << "1.Start/Continue\n2.Quick Start\n3.Change Options\n4.About\n0.Exit\n--------------------------\n";
	cout << "Command: ";
	cin >> option;

	return option;
}

//Function that recieves the
void menuFunc(int option) {
	int x;
	switch (option) {
		case 1:

			cont=true;
			set = false;
			game();
			break;

		case 2:
			cont=true;
			autoSet();
			set = true;
			game();
			break;

		case 4:
			about();
			break;

		case 0:
			exit(0);
			break;
	}

	system("CLS");				//Clears screen

}

void autoSet() {
	COOR start;
	char dir;
	int x;
	int y;
	int z;

	for(x=0; x<5; x++) {
		start.x = (rand()%10) + 1;
		start.y = (rand()%10) + 1;
		y = (rand()%3) + 0;
		switch(y) {
			case 0:
				dir = 'r';
				break;
			case 1:
				dir = 'd';
				break;
			case 2:
				dir = 'u';
				break;
			case 3:
				dir = 'l';
				break;
		}
		switch(x) {
			case 0:
				if((z = setPiece(&play[1].dest, play[1], board1, start, dir) == 0)) {
					x--;
				}
				break;
			case 1:
				if((z = setPiece(&play[1].sub, play[1], board1, start, dir) == 0)) {
					x--;
				}
				break;

			case 2:
				if((z = setPiece(&play[1].crus, play[1], board1, start, dir) == 0)) {
					x--;
				}
				break;

			case 3:
				if((z = setPiece(&play[1].battle, play[1], board1, start, dir) == 0)) {
					x--;
				}
				break;

			case 4:
				if((z = setPiece(&play[1].carr, play[1], board1, start, dir) == 0)) {
					x--;
				}
				break;

		}
	}
}

void about() {
	MessageBox(NULL, "Created by Vineet Tiruvadi", "About", MB_OK);
}

void makeBoard(unit board[10][10]) {
	int x;
	int y;
	for (x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board[x][y].type = ' ';
		}
	}

}