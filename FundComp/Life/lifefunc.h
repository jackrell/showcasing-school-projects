// lifefunc.h
// The header file
// Jack Rellinger
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 40

void initializeBoard(char [][SIZE]);
void displayBoard(char [][SIZE]);
void interactivePlay(char [][SIZE], char [][SIZE]);
char choiceChar();
void printmenu();
void addCell(int, int, char [][SIZE]);
void rmCell(int, int, char [][SIZE]);
void applyRules(char [][SIZE], char [][SIZE]);
int checkNeighbors(int, int, char [][SIZE]);
void continuousPlay(char [][SIZE], char [][SIZE]);
int batchMode(char infile[], char [][SIZE], char [][SIZE]);	

