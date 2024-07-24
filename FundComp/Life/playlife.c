// playlife.c
// The main driver
// Jack Rellinger
//
#include "lifefunc.h"

int main(int argc, char *argv[]) 
{
	char currboard[SIZE][SIZE];
	char tempboard[SIZE][SIZE];

	initializeBoard(currboard);
	
	// chooses what to do based on number of command line arguments	
	if(argc == 1){
		interactivePlay(currboard, tempboard);
	}else if(argc == 2){
		batchMode(argv[1], currboard, tempboard);
	}else{
		printf("Too many arguments.\n");
	}
	
	return 0;
}
