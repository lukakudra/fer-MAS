#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DIMENSION 512
#define PI 3.141592654

int rgb[DIMENSION][DIMENSION][3];
int ycbcr[DIMENSION][DIMENSION][3];
int dct[DIMENSION][DIMENSION][3];
int dctQuant[DIMENSION][DIMENSION][3];

int zigZagArray[64];

short K1[8][8] = { { 16, 11, 10, 16, 24, 40, 51, 61 },
{ 12, 12, 14, 19, 26, 58, 60, 55 },
{ 14, 13, 16, 24, 40, 57, 69, 56 },
{ 14, 17, 22, 29, 51, 87, 80, 62 },
{ 18, 22, 37, 56, 68, 109, 103, 77 },
{ 24, 36, 55, 64, 81, 104, 113, 92 },
{ 49, 64, 78, 87, 103, 121, 120, 101 },
{ 72, 92, 95, 98, 112, 100, 103, 99 } };

short K2[8][8] = { { 17, 18, 24, 47, 99, 99, 99, 99 },
{ 18, 21, 26, 66, 99, 99, 99, 99 },
{ 24, 26, 56, 99, 99, 99, 99, 99 },
{ 47, 66, 99, 99, 99, 99, 99, 99 },
{ 99, 99, 99, 99, 99, 99, 99, 99 },
{ 99, 99, 99, 99, 99, 99, 99, 99 },
{ 99, 99, 99, 99, 99, 99, 99, 99 },
{ 99, 99, 99, 99, 99, 99, 99, 99 } };

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


void convertToYCbCr(int numCol, int numRow) {
	int i, j;
	
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			int r = (int)rgb[i][j][0];
			int g = (int)rgb[i][j][1];
			int b = (int)rgb[i][j][2];
			ycbcr[i][j][0] = (int)(0.299 * r + 0.587 * g + 0.114 * b);
			ycbcr[i][j][1] = (int)(-0.1687 * r - 0.3313 * g + 0.5 * b + 128);
			ycbcr[i][j][2] = (int)(0.5 * r - 0.4187 * g - 0.0813 * b + 128);
			
		}
	}
}

void shiftNegative128(int numCol, int numRow) {
	int i, j, k;
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			for(k=0; k<3; k++) {
				ycbcr[i][j][k] = ycbcr[i][j][k] - 128;
			}
		}
	}
}

void discreteCosineTransform(int numCol, int numRow) {
	int i, j, k, l, u, v;
	float Cu, Cv;
	float sumay = 0.0;
	float sumacb = 0.0;
	float sumacr = 0.0;
	
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			
			int y[8][8] = {0};
			int cb[8][8] = {0};
			int cr[8][8] = {0};
			
			int Y[8][8] = {0};
			int CB[8][8] = {0};
			int CR[8][8] = {0};
			
			int KY[8][8] = {0};
			int KCB[8][8] = {0};
			int KCR[8][8] = {0};
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					y[k][l] = ycbcr[k+i][l+j][0];
					cb[k][l] = ycbcr[k+i][l+j][1];
					cr[k][l] = ycbcr[k+i][l+j][2];
				}
			}
			for(u=0; u<8; u++) {
				for(v=0; v<8; v++) {
					if(u == 0) {
						Cu = 1.0 / sqrt(2.0);
					} else {
						Cu = 1.0;
					}
					if(v == 0) {
						Cv = 1.0 / sqrt(2.0);
					} else {
						Cv = 1.0;
					}
					sumay = 0.0;
					sumacb = 0.0;
					sumacr = 0.0;
					
					for(k = 0; k<8; k++) {
						for(l = 0; l<8; l++) {
							sumay += (float)y[k][l] * cos((2.0*(float)k + 1.0)*(float)u*PI / 16.0)*cos((2.0*(float)l + 1.0)*(float)v*PI / 16.0);
							sumacb += (float)cb[k][l] * cos((2.0*(float)k + 1.0)*(float)u*PI / 16.0)*cos((2.0*(float)l + 1.0)*(float)v*PI / 16.0);
							sumacr += (float)cr[k][l] * cos((2.0*(float)k + 1.0)*(float)u*PI / 16.0)*cos((2.0*(float)l + 1.0)*(float)v*PI / 16.0);
						}
					}
					Y[u][v] = (int)((1.0 / 4) * Cu * Cv * sumay);
					CB[u][v] = (int)((1.0 / 4) * Cu * Cv * sumacb);
					CR[u][v] = (int)((1.0 / 4) * Cu * Cv * sumacr);
				}
			}
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					dct[k+i][l+j][0] = Y[k][l];
					dct[k+i][l+j][1] = CB[k][l];
					dct[k+i][l+j][2] = CR[k][l];
				}
			}
			
			/*================ KVANTIZACIJA KOMPONENTI =======================*/
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					KY[k][l] = (round)(Y[k][l] / K1[k][l]);
					KCB[k][l] = (round)(CB[k][l] / K2[k][l]);
					KCR[k][l] = (round)(CR[k][l] / K2[k][l]);
				}
			}
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					dctQuant[k+i][l+j][0] = KY[k][l];
					dctQuant[k+i][l+j][1] = KCB[k][l];
					dctQuant[k+i][l+j][2] = KCR[k][l];
				}
			}
				
		}
	}
}

void zigZagOrder(int v, int s, int faktor, int zigZag[64]) {
	
	int k = 0;
    int i = 0;
    int j = 0;
         
	/*======================= GORNJI TROKUT MATRICE ============================*/
	zigZag[k++] = dctQuant[v + i][s + j][faktor];
    while(1) {
        j++;
		zigZag[k++] = dctQuant[v + i][s + j][faktor];
             
        while(j!=0) {
			i++;
            j--;
                 
			zigZag[k++] = dctQuant[v + i][s + j][faktor];
        }
        i++;
        if(i>7) {
            i--;
            break;
        }
             
		zigZag[k++] = dctQuant[v + i][s + j][faktor];
 
        while(i!=0) {
            i--;
            j++;
			zigZag[k++] = dctQuant[v + i][s + j][faktor];
        }
    }
	/*======================= DONJI TROKUT MATRICE ============================*/
    while(1) {
        j++;
        zigZag[k++] = dctQuant[v + i][s + j][faktor];   
        while(j!=7) {
        
            j++;
            i--;
                 
			zigZag[k++] = dctQuant[v + i][s + j][faktor];
        }
        i++;
        if(i>7) {
			i--;
            break;
        }
 
		zigZag[k++] = dctQuant[v + i][s + j][faktor];
 
        while(i!=7) {
            i++;
            j--;
 			zigZag[k++] = dctQuant[v + i][s + j][faktor];
        }
    }
}

int main(int argc, char *argv[]) {
	char type[3];
	int numRow, numCol, maxNum;
	int i, j, k, l;
	FILE *readFile, *writeFile;
	
	/* ============================= CITANJE IZ DATOTEKE ===============================*/
	
	readFile = fopen(argv[1], "rb");
	if (readFile == NULL) {
		fprintf(stderr, "Could not open file %s for reading in binary", argv[1]);
		return 0;
	}
	fscanf(readFile, "%2s", type);
	type[2] = '\0';
	if (strcmp(type, "P6") != 0) {
		printf("This file is not of type P6");
		return 0;
	}
	
	// fscanf(readFile, "%d", &numCol);
	// fscanf(readFile, "%d", &numRow);
	// fscanf(readFile, "%d", &maxNum);
	
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
			for(k=0; k<3; k++) {
				fread(&rgb[i][j][k], 1, 1, readFile);
			}
		}
	}
	
	/*========================= FUNKCIJE =================================*/
	
	convertToYCbCr(numCol, numRow);
	shiftNegative128(numCol, numRow);
	discreteCosineTransform(numCol, numRow);
	
	/*======================= PISANJE U DATOTEKU ===============================*/
	
	writeFile = fopen("out.txt", "w+");
	
	fprintf(writeFile, "%d %d", numCol, numRow);
	
	fprintf(writeFile, "\n\n");
	
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			zigZagOrder(i, j, 0, zigZagArray);
			for(k=0; k<64; k++) {
				fprintf(writeFile, "%d ", zigZagArray[k]);
			}
		}
	}
	
	fprintf(writeFile, "\n");
	
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			zigZagOrder(i, j, 1, zigZagArray);
			for(k=0; k<64; k++) {
				fprintf(writeFile, "%d ", zigZagArray[k]);
			}
		}
	}
	
	fprintf(writeFile, "\n");
	
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			zigZagOrder(i, j, 2, zigZagArray);
			for(k=0; k<64; k++) {
				fprintf(writeFile, "%d ", zigZagArray[k]);
			}
		}
	}
	
	fprintf(writeFile, "\n");
	
	fclose(readFile);
	fclose(writeFile);
	return 0;
}