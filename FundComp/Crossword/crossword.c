//crossword.c
//Jack Rellinger
//The main driver for the crossword program.
//

#include "crossfunc.h"

int main(int argc, char*argv[])
{

	char solBoard[BSIZE][BSIZE];
	char puzBoard[BSIZE][BSIZE];
	char wordArr[MXWORDS][MXLEN];
	
	initializeBoards(solBoard, puzBoard);

	if (argc == 1 || argc == 2 || argc == 3){
		printf("Anagram Crossword Puzzle Generator\n");
		printf("----------------------------------\n\n");
	}

	int err; // ends program if file name(s) are invalid
	if (argc == 1){
		interactiveMode(wordArr, solBoard, puzBoard);
	}else if(argc == 2){
		err = iFileMode(argv[1], wordArr, solBoard, puzBoard);
		if(err == 1) return 0;
	}else if(argc == 3){
		err = ioFileMode(argv[1], argv[2], wordArr, solBoard, puzBoard);
		if(err == 1) return 0;
	}else{
		printf("Too many command line arguments.\n");
	}
	return 0;
}
