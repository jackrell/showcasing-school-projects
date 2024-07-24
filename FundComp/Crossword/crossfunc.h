//crossfunc.h
//Jack Rellinger
//The header file for crossword program
//

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MXWORDS 20
#define MXLEN 16 // accounting for null char
#define BSIZE 15

typedef struct{
	char orient[7]; // to describe orientation (across or down)
	int row;
	int col;
} Clue;

void initializeBoards(char [][BSIZE], char [][BSIZE]);
void displayBoard(char [][BSIZE], FILE*);
void interactiveMode(char [][MXLEN], char [][BSIZE], char [][BSIZE]);
void sortWords(char [][MXLEN], int);
void capitalWords(char [][MXLEN], int);
int iFileMode(char [], char [][MXLEN], char [][BSIZE], char [][BSIZE]);
int ioFileMode(char [], char [], char [][MXLEN], char [][BSIZE], char [][BSIZE]);
int iFileRead(char [], char [][MXLEN]);
int fillBoards(char [][MXLEN], int, char [][BSIZE], char [][BSIZE], char [][MXLEN], Clue []);
bool checkFit(char [], int row, int col, int letter, char [][BSIZE], int*, char [][MXLEN], Clue []);
void placeWord(char [], int row, int col, int lbl, int lal, char [][BSIZE], bool vertical, Clue [], int);
void displayClues(Clue [], char [][MXLEN], int, FILE*);
