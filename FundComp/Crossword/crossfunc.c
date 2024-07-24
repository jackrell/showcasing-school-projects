//crossfunc.c
//Jack Rellinger
//Functions file for crossword program
//

#include "crossfunc.h"

// initialize boards
void initializeBoards(char solBoard[][BSIZE], char puzBoard[][BSIZE])
{
	for (int i = 0; i < BSIZE; i ++){
		for (int j = 0; j < BSIZE; j ++){
			solBoard[i][j] = '.';
			puzBoard[i][j] = '#';
		}
	}
}

// displays board
void displayBoard(char board[][BSIZE], FILE* fp)
{
	for (int i = 0; i < BSIZE+2; i ++) fprintf(fp, "_");
	fprintf(fp, "\n");
	for (int i = 0; i < BSIZE; i ++){
		fprintf(fp, "|");
		for (int j = 0; j <	BSIZE; j ++) fprintf(fp, "%c", board[i][j]);
		fprintf(fp, "|\n");
	}
	for (int i = 0; i < BSIZE+2; i ++) fprintf(fp, "-");
	fprintf(fp, "\n");
}

void interactiveMode(char wordArr[][MXLEN], char solBoard[][BSIZE], char puzBoard[][BSIZE])
{
	char word[MXLEN * 2]; //allow word to initially accept large input
	int count = 0;
	bool alpha;

	//	
	// User interaction
	//
	printf("Enter a list of words (enter a period to stop):\n");
	while(count<20){
		scanf("%s", word);
		if(word[0] == '.') break;
		// checking if alphabetical
		alpha = true;
		for(int i = 0; i<strlen(word); i++){
			alpha = isalpha(word[i]);
			if(alpha == false){ 
				printf("Not a valid word input.\n");
				break;
			}
		}
		if(alpha == false) continue;
		// checking if word is at least 2 characters and less than 15
		if(strlen(word) < 2){
			printf("Word is too short.\n");
			continue;
		}
		if(strlen(word) >= MXLEN){ // >= because MXLEN accounts for null, strlen doesn't
			printf("Word is too long.\n");
			continue;
		} 
		strcpy(wordArr[count], word);
		count++;
	}

	sortWords(wordArr, count);
	capitalWords(wordArr, count);

	// array to store words that have been placed
	char placedWords[MXWORDS][MXLEN];
	// array to store clue information
	Clue cluesInfo[MXWORDS];

	int placedCount = fillBoards(wordArr, count, solBoard, puzBoard, placedWords, cluesInfo);
	
	// displaying output
	printf("Solution:\n");	
	displayBoard(solBoard, stdout);
	printf("\nPuzzle:\n");
	displayBoard(puzBoard, stdout);
	displayClues(cluesInfo, placedWords, placedCount, stdout);
}


// runs the input file mode, integer to return errors
int iFileMode(char infile[], char wordArr[][MXLEN], char solBoard[][BSIZE], char puzBoard[][BSIZE])
{
	int count = iFileRead(infile, wordArr);
	// if file name is invalid or file is empty
	if(count == 0){
		printf("Invalid input file.\n");
		return 1;
	}
	
	// array to store words that have been placed
	char placedWords[MXWORDS][MXLEN];
	// struct array to store clue information
	Clue cluesInfo[MXWORDS];

	int placedCount = fillBoards(wordArr, count, solBoard, puzBoard, placedWords, cluesInfo);
	
	// displaying output	
	printf("Solution:\n");	
	displayBoard(solBoard, stdout);
	printf("\nPuzzle:\n");
	displayBoard(puzBoard, stdout);
	displayClues(cluesInfo, placedWords, placedCount, stdout);
}

// runs the input output file mode, integer to return errors
int ioFileMode(char infile[], char outfile[], char wordArr[][MXLEN], char solBoard[][BSIZE], char puzBoard[][BSIZE])
{
	int count = iFileRead(infile, wordArr);
	if(count == 0){
		printf("Invalid input file.\n");
		return 1;
	}
	
	// array to store words that have been placed
	char placedWords[MXWORDS][MXLEN];
	// struct array to store clue information
	Clue cluesInfo[MXWORDS];
	int placedCount = fillBoards(wordArr, count, solBoard, puzBoard, placedWords, cluesInfo);
	
	FILE* fp;
	fp = fopen(outfile, "w");
	
	// prints output to stdout but also prints to output file
	printf("Solution:\n");
	fprintf(fp, "Solution:\n");	
	displayBoard(solBoard, stdout);
	displayBoard(solBoard, fp);
	printf("\nPuzzle:\n");
	fprintf(fp, "\nPuzzle:\n");
	displayBoard(puzBoard, stdout);
	displayBoard(puzBoard, fp);
	displayClues(cluesInfo, placedWords, placedCount, stdout);
	displayClues(cluesInfo, placedWords, placedCount, fp);
	printf("\nAbove output saved to file: %s\n", outfile);
}


// bubble sort algorithm used to sort the words
void sortWords(char wordArr[][MXLEN], int count)
{
	for(int i = 0; i < count; i++){
		for(int j = i + 1; j < count; j++){
			if(strlen(wordArr[i]) < strlen(wordArr[j])){
				char temp[MXLEN];
				strcpy(temp, wordArr[i]);
				strcpy(wordArr[i], wordArr[j]);
				strcpy(wordArr[j], temp);
			}
		}	
	}
}

// makes all words in wordArr capitalized
void capitalWords(char wordArr[][MXLEN], int count)
{
	for(int i = 0; i < count; i ++)
		for(int j = 0; j < strlen(wordArr[i]); j ++) wordArr[i][j] = toupper(wordArr[i][j]);
}

// reads in file contents to fill puzzle, returns 0 for an error (or if file is wordless)
int iFileRead(char infile[], char wordArr[][MXLEN])
{
	printf("Using input file: %s\n\n", infile);
	FILE *fp;
	fp = fopen(infile, "r");
	if(!fp) return 0;

	char word[MXLEN * 2]; // allow word to initially accept large input
	int count = 0;
	bool alpha;

	while(count<20){
		fscanf(fp, "%s", word); // different from above
		if(word[0] == '.') break;
		// checking if alphabetical
		alpha = true;
		for(int i = 0; i<strlen(word); i++){
			alpha = isalpha(word[i]);
			if(alpha == false) break;
		}
		if(alpha == false) continue;
		// checking if word is at least 2 characters and less than 15
		if(strlen(word) < 2) continue;
		if(strlen(word) >= MXLEN) continue;
		strcpy(wordArr[count], word);
		count++;
	}
	
	sortWords(wordArr, count);
	capitalWords(wordArr, count);
	
	return count;		
}

// fills the solution board and puzzle board based on the rules
int fillBoards(char wordArr[][MXLEN], int count, char solBoard[][BSIZE], char puzBoard[][BSIZE], char placedWords[][MXLEN], Clue cluesInfo[])
{	
	int placedCount = 0;
	int *placedCP = &placedCount;

	// placing longest word centered horizontally in middle of board
	int offset1;
	int offset2;
	if((BSIZE - strlen(wordArr[0])) % 2 == 0){
		offset1 = (BSIZE - strlen(wordArr[0])) / 2;
		offset2 = offset1;
	}else{
		offset1 = (BSIZE - strlen(wordArr[0])) / 2;
		offset2 = offset1 + 1;
	}
	int middleIndex = BSIZE / 2;
	for(int i = offset1; i < BSIZE - offset2; i++){	
		solBoard[middleIndex][i] = wordArr[0][i - offset1];
	}
	// update clue info
	strcpy(cluesInfo[placedCount].orient, "Across");
	cluesInfo[placedCount].row = middleIndex+1;
	cluesInfo[placedCount].col = offset1+1;
	// update placed words array
	strcpy(placedWords[0], wordArr[0]);
	placedCount++;
	
	bool loopDone = false;
	
	for(int i = 1; i < count; i++){ // for loop used as a counter and i is word index
		loopDone = false;
		for(int j = 0; j < BSIZE; j ++){ // j is row
			if(loopDone) break;
			for(int k = 0; k < BSIZE; k ++){ // k is col
				if(loopDone) break;
				for(int l = 0; l < strlen(wordArr[i]); l ++){ // l is letter index
					if(wordArr[i][l] == solBoard[j][k]){
						loopDone = checkFit(wordArr[i], j, k, l, solBoard, placedCP, placedWords, cluesInfo); // return true if fit and placed
						if(loopDone){
							break;
						}else{
							continue;
						}
					}
					// message if word cannot be placed
					if((l == strlen(wordArr[i]) - 1) && (j == k) && (k == BSIZE - 1))
						printf("Could not place word: %s\n\n", wordArr[i]);
				}
			}
		}
	}
	
	// fill puzzle board
	for (int j = 0; j < BSIZE; j++)
		for (int k = 0; k < BSIZE; k++)
			if (solBoard[j][k] != '.') puzBoard[j][k] = ' ';
		
	
	return placedCount;
}

// checks intersection orientation, if the word fits
// then calls a function to place the word if it fits
bool checkFit(char word[],  int row, int col, int letter, char solBoard[][BSIZE], int* placedCP,char placedWords[][MXLEN], Clue cluesInfo[])
{
	bool horizontal = false;
	bool vertical = false;
	int neighborArr[][2] = {{-1, 1}, {1,1}, {1, -1}, {-1, -1}}; // to check other elements in neighborhood
	int lbl = letter; // length of word before the specific letter
	int lal = strlen(word) - letter - 1; // length of word after the specific letter
	
	bool fit = false;	

	if (solBoard[row][col-1] != '.' || solBoard[row][col+1] != '.') horizontal = true;
	if (solBoard[row-1][col] != '.' || solBoard[row+1][col] != '.') vertical = true;
	
	// checking if intersection is already filled
	if (horizontal && vertical) return fit;
	
	// checking rest of neighborhood
	for(int i = 0; i < 4; i ++){
		if (solBoard[row + neighborArr[i][0]][col + neighborArr[i][0]] != '.') return fit;
	}
	
	// checking vertical fit
	if (horizontal){
		// checking if fits on board
		if (row - lbl < 0) return fit;
		if (row + lal >= BSIZE) return fit;
		// checking if overlap or neighbors
		for(int i = 1; i <= lbl; i ++){
			if (solBoard[row-i][col-1] != '.' || solBoard[row-i][col+1] != '.' || solBoard[row-i][col] != '.') return fit;
		}

		if (solBoard[row-lbl-1][col] != '.') return fit;

		for(int i = 1; i <= lal; i ++){
			if (solBoard[row+i][col-1] != '.' || solBoard[row+i][col+1] != '.' || solBoard[row+i][col] != '.') return fit;
		}

		if (solBoard[row+lal+1][col] != '.') return fit;

	// place word
	placeWord(word, row, col, lbl, lal, solBoard, vertical, cluesInfo, *placedCP);
	strcpy(placedWords[*placedCP], word);
	// increase placedCount
	(*placedCP)++;
	fit = true;
	}
	
	// checking horizontal fit
	if (vertical){
		// checking if fits on board
		if (col - lbl < 0) return fit;
		if (col + lal >= BSIZE) return fit;
		//checking if overlap or neighbors
		for(int i = 1; i <= lbl; i ++){
			if (solBoard[row-1][col-i] != '.' || solBoard[row+1][col-i] != '.' || solBoard[row][col-i] != '.') return fit;
		}
	
		if (solBoard[row][col-lbl-1] != '.') return fit;

		for(int i = 1; i <= lal; i ++){
			if (solBoard[row-1][col+i] != '.' || solBoard[row+1][col+i] != '.' || solBoard[row][col+i] != '.') return fit;
		}

		if (solBoard[row][col+lal+1] != '.') return fit;
	
	// place word
	placeWord(word, row, col, lbl, lal, solBoard, vertical, cluesInfo, *placedCP);
	strcpy(placedWords[*placedCP], word);
	// increase placedCount
	(*placedCP)++;
	fit = true;
	}
	
	return fit;
}

// function that places word based on orientation, updating the clue data
void placeWord(char word[], int row, int col, int lbl, int lal, char solBoard[][BSIZE], bool vertical, Clue cluesInfo[], int placedCount)
{
	if (vertical){
		strcpy(cluesInfo[placedCount].orient, "Across");
		cluesInfo[placedCount].row = row + 1;
		cluesInfo[placedCount].col = col - lbl + 1;
		for (int i = col - lbl; i <= col + lal; i ++) solBoard[row][i] = word[i - (col-lbl)];
	}else{
		strcpy(cluesInfo[placedCount].orient, "Down");
		cluesInfo[placedCount].row = row - lbl + 1;
		cluesInfo[placedCount].col = col + 1;
		for (int i = row - lbl; i <= row + lal; i ++) solBoard[i][col] = word[i - (row - lbl)];
	}
}

// displays the clue data
void displayClues(Clue cluesInfo[], char placedWords[][MXLEN], int count, FILE* fp){
	fprintf(fp, "\nClues:\nrow, col | orientation | scrambled clue\n");
	// print in order moving down rows
	for (int i = 0; i < BSIZE; i++){
		for (int k = 0; k < count; k ++){
			if(cluesInfo[k].row == i) fprintf(fp, "%2d,%2d %-7s %-16s\n", cluesInfo[k].row, cluesInfo[k].col, cluesInfo[k].orient, strfry(placedWords[k]));
		}
	}
}
