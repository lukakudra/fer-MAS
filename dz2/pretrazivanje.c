#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DIMENSION 512

int picture1[DIMENSION][DIMENSION]; // lenna.pgm
int picture2[DIMENSION][DIMENSION]; // lenna1.pgm
int wanted_block[16][16] = {0};
int horizontal_shift = 0;
int vertical_shift = 0;
double mad = 999.0;

int getHeader(FILE *inFile) {
	int c;
	c = getc(inFile);
	while(c == '\n' || c == '\r' || c == '\t' ||
		  c == ' ' || c == '#') {
		if(c == '#') {
			while (c != EOF && c != '\n' && c != '\r') {
				c = getc(inFile);
			}
		} else {
			c = getc(inFile);
		}
	}
	
	if (c >= '0' && c <= '9') {
		int x = 0;
		
		while(c >= '0' && c<= '9') {
			int y = x;
			int rez = 0;
			x = 10 * x + c - '0';
			rez = (int)(x / 10);
			if (rez != y || x < 0) {
				printf("ERROR: Value too large to fit in an integer\n");
				return 0;
			}
			c = getc(inFile);
		}
		
		if (c != EOF) {
			ungetc(c, inFile);
		}
		
		return x;
	}
	
	if(c == EOF) {
		printf("ERROR: Premature end of file\n");
		return 0;
	} else {
		printf("ERROR: Not a non-negative integer\n");
		return 0;
	}
	
	return -1;
		
}

void readWantedBlock(int numCol, int numRow, int block) {
	int i, j, k, l;
	int block_counter = 0;
	for(i=0; i<numCol; i=i+16) {
		for(j=0; j<numRow; j=j+16) {
			if(block_counter == block) {
				for(k=0; k<16; k++) {
					for(l=0; l<16; l++) {
						wanted_block[k][l] = picture2[k+i][l+j];
					}
				}
			}
			block_counter++;
		}
	}
}

void calculateMAD(int horizontal_begin, int horizontal_end, int vertical_begin, int vertical_end, int column, int row) {
	int i, j, k, l;
	double sum = 0;
	double difference = 0;
	double mean = 0;
	for(i=vertical_begin; i<=vertical_end; i++) {
		for(j=horizontal_begin; j<=horizontal_end; j++) {
			for(k=0; k<16; k++) {
				for(l=0; l<16; l++) {
					difference = abs(wanted_block[k][l] - picture1[k+i][l+j]);
					sum = sum + difference;
				}
			}
			
			mean = sum / 256;
			// trazimo najmanju vrijednost mad - a
			if(mean < mad) {
				mad = mean;
				horizontal_shift = -1 * column * 16 + j;
				vertical_shift = -1 * row * 16 + i;
			}
			
			sum = 0;
		}
	}
}

int main(int argc, char *argv[]) {
	char type[3];
	int numRow1, numCol1, maxNum1;
	int numRow2, numCol2, maxNum2;
	int block;
	int row = 0, column = 0;
	int horizontal_begin = 0, horizontal_end = 0;
	int vertical_begin = 0, vertical_end = 0;
	int i, j, k;
	FILE *inFile1, *inFile2;
	
	/* ============== CITANJE PRVE DATOTEKE - "lenna.pgm" ======================== */
	
	inFile1 = fopen("lenna.pgm", "rb");
	if (inFile1 == NULL) {
		fprintf(stderr, "Could not open file %s for reading in binary", argv[1]);
		return 0;
	}
	
	fscanf(inFile1, "%2s", type);
	type[2] = '\0';
	if (strcmp(type, "P5") != 0) {
		printf("This file is not of type P5");
		return 0;
	}
	
	numCol1 = getHeader(inFile1);
	if(numCol1 <= 0) {
		printf("Invalid width\n");
		return 0;
	}
	
	numRow1 = getHeader(inFile1);
	if(numRow1 <= 0) {
		printf("Invalid height\n");
		return 0;
	}
	
	maxNum1 = getHeader(inFile1);
	if(maxNum1 <= 0 || maxNum1 >= 65536) {
		printf("Invalid maxNum\n");
		return 0;
	}
	
	
	fseek(inFile1, 1, SEEK_CUR);
	
	for(i=0; i<numCol1; i++) {
		for(j=0; j<numRow1; j++) {
			fread(&picture1[i][j], 1, 1, inFile1);
		}
	}
	
	/* ============== CITANJE DRUGE DATOTEKE - "lenna1.pgm" ======================= */

	inFile2 = fopen("lenna1.pgm", "rb");
	if (inFile2 == NULL) {
		fprintf(stderr, "Could not open file %s for reading in binary", argv[1]);
		return 0;
	}
	
	fscanf(inFile2, "%2s", type);
	type[2] = '\0';
	if (strcmp(type, "P5") != 0) {
		printf("This file is not of type P5");
		return 0;
	}
	
	numCol2 = getHeader(inFile2);
	if(numCol2 <= 0) {
		printf("Invalid width\n");
		return 0;
	}
	
	numRow2 = getHeader(inFile2);
	if(numRow2 <= 0) {
		printf("Invalid height\n");
		return 0;
	}
	
	maxNum2 = getHeader(inFile2);
	if(maxNum2 <= 0 || maxNum2 >= 65536) {
		printf("Invalid maxNum\n");
		return 0;
	}
	
	fseek(inFile2, 1, SEEK_CUR);
	
	for(i=0; i<numCol2; i++) {
		for(j=0; j<numRow2; j++) {
			fread(&picture2[i][j], 1, 1, inFile2);
		}
	}
	
	block = atoi(argv[1]); // ulazni argument je redni broj bloka
	if(!(block >= 0 && block <= 1023)) {
		printf("Block must be in interval [0, 1023]\n");
		return 0;
	}
	// citanje referentnog bloka iz lenna1.pgm
	readWantedBlock(numCol2, numRow2, block);
	
	/* ======================= ODREDIVANJE GRANICA PRETRAGE ===================== */
	
	row = block / 32;
	column = block % 32;
		
	horizontal_begin = column * 16 - 16;
	if(horizontal_begin < 0) {
		horizontal_begin = 0;
	}
	
	horizontal_end = column * 16 + 16;
	if(horizontal_end > (numCol1 - 16)) {
		horizontal_end = numCol1 - 16;
	}
	
	vertical_begin = row * 16 - 16;
	if(vertical_begin < 0) {
		vertical_begin = 0;
	}
	
	vertical_end = row * 16 + 16;
	if(vertical_end > (numRow1 - 16)) {
		vertical_end = numRow1 - 16;
	}
	
	// izracun pomaka i mad - a
	calculateMAD(horizontal_begin, horizontal_end, vertical_begin, vertical_end, column, row);
	
	printf("(%d,%d)\n", horizontal_shift, vertical_shift);
	
	fclose(inFile1);
	fclose(inFile2);
	return 0;
}