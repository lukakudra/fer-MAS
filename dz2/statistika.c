#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIMENSION 512

int slika[DIMENSION][DIMENSION];
unsigned long brojac0=0, brojac1=0, brojac2=0, brojac3=0, brojac4=0,
			  brojac5=0, brojac6=0, brojac7=0, brojac8=0, brojac9=0,
			  brojac10=0, brojac11=0, brojac12=0, brojac13=0, brojac14=0,
			  brojac15=0;

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

void decToBinary(char *number, int count, int n) {
	char buffer[10];
	itoa(n, buffer, 2);
	int length = strlen(buffer);
	while(length < 8) {
		strcat(number, "0");
		length++;
	}
	strcat(number, buffer);
}

void findCategory(char *number) {
	char subbuff[5];
	memcpy(subbuff, &number[0], 4);
	subbuff[4] = '\0';
	
	if(strcmp(subbuff, "0000") == 0) {
		brojac0++;
	} else if(strcmp(subbuff, "0001") == 0) {
		brojac1++;
	} else if(strcmp(subbuff, "0010") == 0) {
		brojac2++;
	} else if(strcmp(subbuff, "0011") == 0) {
		brojac3++;
	} else if(strcmp(subbuff, "0100") == 0) {
		brojac4++;
	} else if(strcmp(subbuff, "0101") == 0) {
		brojac5++;
	} else if(strcmp(subbuff, "0110") == 0) {
		brojac6++;
	} else if(strcmp(subbuff, "0111") == 0) {
		brojac7++;
	} else if(strcmp(subbuff, "1000") == 0) {
		brojac8++;
	} else if(strcmp(subbuff, "1001") == 0) {
		brojac9++;
	} else if(strcmp(subbuff, "1010") == 0) {
		brojac10++;
	} else if(strcmp(subbuff, "1011") == 0) {
		brojac11++;
	} else if(strcmp(subbuff, "1100") == 0) {
		brojac12++;
	} else if(strcmp(subbuff, "1101") == 0) {
		brojac13++;
	} else if(strcmp(subbuff, "1110") == 0) {
		brojac14++;
	} else if(strcmp(subbuff, "1111") == 0) {
		brojac15++;
	}
	
}

int main(int argc, char *argv[]) {
	char type[3];
	int numRow, numCol, maxNum;
	int i, j;
	FILE *readFile;
	
	readFile = fopen(argv[1], "rb");
	if (readFile == NULL) {
		fprintf(stderr, "Could not open file %s for reading in binary", argv[1]);
		return 0;
	}
	fscanf(readFile, "%2s", type);
	type[2] = '\0';
	if (strcmp(type, "P5") != 0) {
		printf("This file is not of type P5");
		return 0;
	}
	
	numCol = getHeader(readFile);
	if(numCol <= 0) {
		printf("Invalid width\n");
		return 0;
	}
	numRow = getHeader(readFile);
	if(numRow <= 0) {
		printf("Invalid height\n");
		return 0;
	}
	maxNum = getHeader(readFile);
	if(maxNum <= 0 || maxNum >= 65536) {
		printf("Invalid maxNum\n");
		return 0;
	}
	
	
	fseek(readFile, 1, SEEK_CUR);
	
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			fread(&slika[i][j], 1, 1, readFile);
		}
	}
	
	/* =========== PRETVARANJE U BINARNI BROJ I RACUNANJE STATISTIKE ========== */
	
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			char number[20] = {'\0'};
			decToBinary(number, 20, slika[i][j]);
			findCategory(number);
		}
	}
	
	int ukupniBrojSimbola = brojac0+brojac1+brojac2+brojac3+brojac4+
							brojac5+brojac6+brojac7+brojac8+brojac9+
							brojac10+brojac11+brojac12+brojac13+brojac14+
							brojac15;
							
	printf("Category0: %d / %d\n", brojac0, ukupniBrojSimbola);
	printf("Category1: %d / %d\n", brojac1, ukupniBrojSimbola);
	printf("Category2: %d / %d\n", brojac2, ukupniBrojSimbola);
	printf("Category3: %d / %d\n", brojac3, ukupniBrojSimbola);
	printf("Category4: %d / %d\n", brojac4, ukupniBrojSimbola);
	printf("Category5: %d / %d\n", brojac5, ukupniBrojSimbola);
	printf("Category6: %d / %d\n", brojac6, ukupniBrojSimbola);
	printf("Category7: %d / %d\n", brojac7, ukupniBrojSimbola);
	printf("Category8: %d / %d\n", brojac8, ukupniBrojSimbola);
	printf("Category9: %d / %d\n", brojac9, ukupniBrojSimbola);
	printf("Category10: %d / %d\n", brojac10, ukupniBrojSimbola);
	printf("Category11: %d / %d\n", brojac11, ukupniBrojSimbola);
	printf("Category12: %d / %d\n", brojac12, ukupniBrojSimbola);
	printf("Category13: %d / %d\n", brojac13, ukupniBrojSimbola);
	printf("Category14: %d / %d\n", brojac14, ukupniBrojSimbola);
	printf("Category15: %d / %d\n", brojac15, ukupniBrojSimbola);
	
	fclose(readFile);
	return 0;
}