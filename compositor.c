#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
	
struct myRow {
	int length;
	char *charArray;
	};
	
struct myStructs {
	int length;
	struct myRow *structArray;
	};
	
struct myRow 
createStruct(int size) {
	struct myRow newRow;
	newRow.length = size;
	newRow.charArray = malloc(sizeof(char)*size);
	return newRow;
}

void
set(struct myRow *s, int c, int position) {
	s->charArray[position] = c;
}
		
void
addImage(struct myStructs *arrayRows, FILE *artfile, int r, int c) {
	int chr; //character we read in
	int rows; //row to write in
	int cols = 0; //col to write in
	int rowsize; //size of array in each struct in arrayRows
	int initSize = arrayRows->length; //initial number of rows in arrayRows
	int size = arrayRows->length; //number of rows in arrayRows
	int i;
	int j;
	
	if (initSize <= r) { //fill in array with rows until offset
		arrayRows->length = r+1; 
		arrayRows->structArray = realloc(arrayRows->structArray, sizeof(struct myRow) * arrayRows->length);
		size = arrayRows->length;
		for (i = initSize; i < r; i++) { //create structs
			arrayRows->structArray[i] = createStruct(1); 
			set(&arrayRows->structArray[i], '\n', 0); 
		} 
		arrayRows->structArray[r] = createStruct(c+1); 
		for (j = 0; j < c; j++) { //insert spaces in last row up to c-1
			set(&arrayRows->structArray[r], ' ', j);
		}
	}
	rows = r; //current row is first row of picture (offset)
	cols = c; // current column is first col of picture (offset)
	int initRow = arrayRows->structArray[r].length - 1; //next element of row to fill

	while ((chr = getc(artfile)) != EOF) {		
		while (rows >= size) { //while current row is greater than number of rows in arrayRows
			size *=2;
			arrayRows->length = size;
			arrayRows->structArray = realloc(arrayRows->structArray, sizeof(struct myRow)*arrayRows->length);
		}	
		if (cols == c) { //set initRow for this new row. If row did not previously exist, createStruct
			if (initSize <= rows) {
				arrayRows->structArray[rows] = createStruct(c+1);
				initRow = 0;
			}
			else {
				initRow = arrayRows->structArray[rows].length - 1; 
			}
		}	
		rowsize = arrayRows->structArray[rows].length; 
		
		while (cols >= rowsize) { //while current column is greater than rowsize (num of cols in row)
				rowsize *=2;
				arrayRows->structArray[rows].length = rowsize;
				arrayRows->structArray[rows].charArray = realloc(arrayRows->structArray[rows].charArray, sizeof(char)*arrayRows->structArray[rows].length);
		}
		
		if (chr == '\n') { //if character is newline
			while (cols > c && arrayRows->structArray[rows].charArray[cols-1] == ' ') {
				cols = cols - 1;
			}
			if (initRow <= cols) { //trim array unless it is less than initial size
				set(&arrayRows->structArray[rows], chr, cols);
				arrayRows->structArray[rows].length = cols + 1;
				arrayRows->structArray[rows].charArray = realloc(arrayRows->structArray[rows].charArray, sizeof(char)* arrayRows->structArray[rows].length); 
			}
			rows = rows + 1;
			cols = c;			
		}
		else if (chr == ' ') {
			while (initRow <= c) { //set spaces in row up till c if space is first char in row
				set(&arrayRows->structArray[rows], ' ', initRow);
				initRow = initRow + 1;
			}
			if (initRow <= cols) {// check this
				set(&arrayRows->structArray[rows], chr, cols);
			}	
			cols = cols + 1;
		}
		else {
			while (initRow <= c) { //set spaces in row up till c-1 if char is first in row
				set(&arrayRows->structArray[rows], ' ', initRow);
				initRow = initRow + 1;
			}
			set(&arrayRows->structArray[rows], chr, cols);
			cols = cols + 1;
		}
	}
	if (rows >= initSize) { //trim number of rows to correct size
		arrayRows->length = rows;
		arrayRows->structArray = realloc(arrayRows->structArray, sizeof(struct myRow) * arrayRows->length);
		}
}

void
printImage(struct myStructs *arrayStructs) {
	int numStructs = arrayStructs->length;
	struct myRow *sarray = arrayStructs->structArray;
	int i;
	int j;
	
	for (i = 0; i < numStructs; i++) {
		int numChars = sarray[i].length;
		char *carry = sarray[i].charArray;
		for (j = 0; j < numChars; j++) {
				putchar(carry[j]);
		}
	}
}

void
freeAll(struct myStructs *arrayStructs) {
	int i;
	for (i = 0; i < arrayStructs->length; i++) {
		free(arrayStructs->structArray[i].charArray);
		//free(&(arrayStructs->structArray[i])); CANT DO BECAUSE THIS WAS NEVER MALLOCED
	}
	free(arrayStructs->structArray);
	free(arrayStructs);
}
			
			
int main(int argc, char **argv) {
	FILE *art; //inputted art file
	int r; //inputted starting row
	int c; //inputted starting col
	int size = 1; //size of array of structs
	char filename[2048]; //filename buffer 
	struct myStructs *arrayStructs; //struct myStruct that contains array of structs
	int i;

	arrayStructs = malloc(sizeof(struct myStructs)); //malloc struct with size of myStruct
	arrayStructs->structArray = malloc(sizeof(struct myRow) * size); //create array of size * size of myRow structs 
	arrayStructs->length = size; //initialize length of array of myRow structs as size 1
	for (i = 0; i < size; i++) {
		arrayStructs->structArray[i] = createStruct(1);
		set(&arrayStructs->structArray[i], '\n', 0);
	}
	while(scanf("%d %d %s", &r, &c, filename) == 3) {
		art = fopen(filename, "r");
		if (art == 0) {
			perror(filename);
			freeAll(arrayStructs);
			exit(1);
		}
		addImage(arrayStructs, art, r, c);
		fclose(art);
	}
	printImage(arrayStructs);
	freeAll(arrayStructs);
}
