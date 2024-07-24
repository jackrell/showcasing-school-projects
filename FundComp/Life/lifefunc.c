// lifefunc.c
// The functions for playlife.c
// Jack Rellinger
//
#include "lifefunc.h"

// initializes the board to be empty
void initializeBoard(char board[SIZE][SIZE])
{
	for (int i = 0; i < SIZE; i ++){
		for (int j = 0; j < SIZE; j ++) board[i][j] = ' ';
	}
}

// displays life board with box around it
void displayBoard(char board[SIZE][SIZE])
{
	for (int i = 0; i < SIZE+2; i ++) printf("%c", '_');
	printf("\n"); 
	for (int i = 0; i < SIZE; i ++){
		printf("%c", '|');
		for (int j = 0; j < SIZE; j ++) printf("%c", board[i][j]);
		printf("%c\n", '|');
	}
	for (int i = 0; i < SIZE+2; i ++) printf("%c", '-');
	printf("\n"); 
}

// activated when in interactive play mode
void interactivePlay(char currboard[][SIZE], char tempboard[][SIZE])
{
	displayBoard(currboard);
	// flag and choice variables
	bool done = false;
	char choice;
	int row;
	int col;
	
	while(!done){
		choice = choiceChar();
		switch (choice){
			case 'a':
				scanf("%d %d", &row, &col);
				getchar();
				addCell(row, col, currboard);
				break;
			case 'r':
				scanf("%d %d", &row, &col);
				rmCell(row, col, currboard);
				break;
			case 'n':
				getchar(); // hack to get newline char
				applyRules(currboard, tempboard);
				break;
			case 'p':
				getchar(); //
				continuousPlay(currboard, tempboard);
				break;
			case 'q':
				done = true;
				break;
			default:
				printf("invalid input, enter another choice\n\n");
		}
	}
	printf("Successful quit.\n");
}

// gets character choice and prints menu
char choiceChar()
{
	char n;
	printmenu();
	scanf("%c", &n);
	return n; 
}

// prints menu options
void printmenu()
{
	printf("'a' followed by two integers: add new live cell at 'row' 'column'\n");
	printf("'r' followed by two integers: remove a cell from 'row' 'column'\n");
	printf("'n': advance program to next stage\n");
	printf("'q': quit\n");
	printf("'p': play game continuously (control C to stop)\n");
	printf("Enter your choice: ");
}

// adds a 'X' cell at coords i j
void addCell(int i, int j, char currboard[][SIZE])
{
	currboard[i][j] = 'X';
	displayBoard(currboard);
}

// removes a 'X' cell if there is one there
void rmCell(int i, int j, char currboard[][SIZE])
{
	getchar();
	if (currboard[i][j] == 'X') currboard[i][j] = ' ';
	else printf("No cell to remove.\n");
	displayBoard(currboard);
}

// acts on the amount of neighbors according to the rules, updates the life board
void applyRules(char currboard[][SIZE], char tempboard[][SIZE])
{
	//check conditions 3 neighbors become live
	//2 neighbors and is live stays live
	//anything else dies
	int neighbors;
	for (int i = 0; i < SIZE; i++){
		for (int j = 0; j < SIZE; j++){
			neighbors = checkNeighbors(i, j, currboard);
			if (neighbors == 3){ tempboard[i][j] = 'X';
			} else if ((neighbors == 2) && (currboard[i][j] == 'X')){ tempboard[i][j] = 'X';
			} else { tempboard[i][j] = ' ';}
		}	
	}
	
	//update currboard with new values
	for(int i = 0; i < SIZE; i++){
		for(int j = 0; j < SIZE; j++) currboard[i][j] = tempboard[i][j];
	}
	
	displayBoard(currboard);
}

// checks the number of neighbors
int checkNeighbors(int i, int j, char currboard[][SIZE])
{
	int neighbors = 0;
	int di, dj;
	// change in each coord. to check neighbor of i,  j
	int deltaIJ[8][2] = {
	{-1, -1}, {-1, 0}, {-1, 1},
	{0, -1}, 		   {0, 1},
	{1, -1},  {1, 0},  {1, 1}
	};
	
	for(int k = 0; k < 8; k ++) {
		// adds and subtract value of 1 or -1 from i and j to get potential neighbor loc
		di = i + deltaIJ[k][0];
		dj = j + deltaIJ[k][1];	
	
		if(di >= 0 && di < SIZE && dj >= 0 && dj < SIZE && currboard[di][dj] == 'X')
			neighbors++;
	}
	
	return neighbors;	
}


void continuousPlay(char currboard[][SIZE], char tempboard[][SIZE])
{
	while(true){
		system("clear");
		applyRules(currboard, tempboard);
		usleep(250000);
	}
}


// runs program in batch mode, reading input from file into 'a' commands then continuously playing
int batchMode(char infile[], char currboard[][SIZE], char tempboard[][SIZE])
{
	FILE *fp;
	
	fp = fopen(infile, "r");
	if(!fp){
		printf("Not a valid file name.\n");
		return 1;
	}	
	
	char c;
	int i, j;
	while(1){
		fscanf(fp, "%c %d %d", &c, &i, &j); // program kept reading lines twice, hack to stop
		fscanf(fp, "%c %d %d", &c, &i, &j);
		if (c == 'p'){
			 break;
		}else{
			//printf("%d %d\n", i, j);
			addCell(i, j, currboard);
		}
	}	
	continuousPlay(currboard, tempboard);

}	
