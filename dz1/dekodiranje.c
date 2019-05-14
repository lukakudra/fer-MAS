#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DIMENSION 512
#define PI 3.141592654

int dctQuant[DIMENSION][DIMENSION][3];
int dct[DIMENSION][DIMENSION][3];
int ycbcr[DIMENSION][DIMENSION][3];
int rgb[DIMENSION][DIMENSION][3];

int zigZagArray[64];

int lineY[DIMENSION * DIMENSION];
int lineCB[DIMENSION * DIMENSION];
int lineCR[DIMENSION * DIMENSION];

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

void zigZagOrderReversed(int v, int s, int faktor, int zigZag[64]) {
	
	int k = 0;
    int i = 0;
    int j = 0;
         
	/*======================= GORNJI TROKUT MATRICE ============================*/
	dctQuant[v + i][s + j][faktor] = zigZag[k++]; 
    while(1) {
        j++;
		dctQuant[v + i][s + j][faktor] = zigZag[k++];
             
        while(j!=0) {
			i++;
            j--;
                 
			dctQuant[v + i][s + j][faktor] = zigZag[k++];
        }
        i++;
        if(i>7) {
            i--;
            break;
        }
             
		dctQuant[v + i][s + j][faktor] = zigZag[k++];
 
        while(i!=0) {
            i--;
            j++;
			dctQuant[v + i][s + j][faktor] = zigZag[k++];
        }
    }
	/*======================= DONJI TROKUT MATRICE ============================*/
    while(1) {
        j++;
        dctQuant[v + i][s + j][faktor] = zigZag[k++];   
        while(j!=7) {
        
            j++;
            i--;
                 
			dctQuant[v + i][s + j][faktor] = zigZag[k++];
        }
        i++;
        if(i>7) {
			i--;
            break;
        }
 
		dctQuant[v + i][s + j][faktor] = zigZag[k++];
 
        while(i!=7) {
            i++;
            j--;
 			dctQuant[v + i][s + j][faktor] = zigZag[k++];
        }
    }
}


void discreteCosineTransformReversed(int numCol, int numRow) {
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
					KY[k][l] = dctQuant[k+i][l+j][0];
					KCB[k][l] = dctQuant[k+i][l+j][1];
					KCR[k][l] = dctQuant[k+i][l+j][2];
				}
			}
			
			/*======================= DEKVANTIZACIJA KOMPONENTI ==================*/
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					Y[k][l] = (round)(KY[k][l] * K1[k][l]);
					CB[k][l] = (round)(KCB[k][l] * K2[k][l]);
					CR[k][l] = (round)(KCR[k][l] * K2[k][l]);
				}
			}
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					dct[k+i][l+j][0] = Y[k][l];
					dct[k+i][l+j][1] = CB[k][l];
					dct[k+i][l+j][2] = CR[k][l];
				}
			}
						
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					
					sumay = 0.0;
					sumacb = 0.0;
					sumacr = 0.0;
					
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
							
							sumay += Cu * Cv * (float)Y[u][v] * cos((2.0*(float)k + 1.0)*(float)u*PI / 16.0)*cos((2.0*(float)l + 1.0)*(float)v*PI / 16.0);
							sumacb += Cu * Cv * (float)CB[u][v] * cos((2.0*(float)k + 1.0)*(float)u*PI / 16.0)*cos((2.0*(float)l + 1.0)*(float)v*PI / 16.0);
							sumacr += Cu * Cv * (float)CR[u][v] * cos((2.0*(float)k + 1.0)*(float)u*PI / 16.0)*cos((2.0*(float)l + 1.0)*(float)v*PI / 16.0);
							
						}
					}
					
					sumay = sumay * (1.0/4);
					sumacb = sumacb * (1.0/4);
					sumacr = sumacr * (1.0/4);
					
					y[k][l] = (int)sumay;
					cb[k][l] = (int)sumacb;
					cr[k][l] = (int)sumacr;
					
				}
			}
			
			for(k=0; k<8; k++) {
				for(l=0; l<8; l++) {
					ycbcr[k+i][l+j][0] = y[k][l];
					ycbcr[k+i][l+j][1] = cb[k][l];
					ycbcr[k+i][l+j][2] = cr[k][l];
				}
			}
		}
	}
}

void shiftPositive128(int numCol, int numRow) {
	int i, j, k;
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			for(k=0; k<3; k++) {
				ycbcr[i][j][k] = ycbcr[i][j][k] + 128;
			}
		}
	}
}

void convertToRGB(int numCol, int numRow) {
	int i, j;
	
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			int y = (int)ycbcr[i][j][0];
			int cb = (int)ycbcr[i][j][1];
			int cr = (int)ycbcr[i][j][2];
			
			rgb[i][j][0] = (int)(1 * y + 0 * (cb - 128) + 1.40200 * (cr - 128)); 
			rgb[i][j][1] = (int)(1 * y - 0.344414 * (cb - 128) - 0.71414 * (cr - 128));
			rgb[i][j][2] = (int)(1 * y + 1.77200 * (cb - 128) + 0 * (cr - 128));
		}
	}
}

int main(int argc, char *argv[]) {
	
	FILE *readFile, *writeFile;
	int i, j, k, l;
	int numCol, numRow;
	
	readFile = fopen(argv[1], "r");
	if (readFile == NULL) {
		fprintf(stderr, "Could not open file %s for reading", argv[1]);
		return 0;
	}
	
	fscanf(readFile, "%d", &numCol);
	fscanf(readFile, "%d", &numRow);
	
	fseek(readFile, 1, SEEK_CUR);

	for(int i=0; i<numCol*numRow; i++) {
		fscanf(readFile, "%d ", &lineY[i]);
	}
	for(int i=0; i<numCol*numRow; i++) {
		fscanf(readFile, "%d ", &lineCB[i]);
	}
	for(int i=0; i<numCol*numRow; i++) {
		fscanf(readFile, "%d ", &lineCR[i]);
	}
	
	int iterator = 0;
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			for(k=0; k<64; k++) {
				zigZagArray[k] = lineY[iterator];
				iterator += 1;
			}
			zigZagOrderReversed(i, j, 0, zigZagArray);
		}
	}
	
	iterator = 0;
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			for(k=0; k<64; k++) {
				zigZagArray[k] = lineCB[iterator];
				iterator += 1;
			}
			zigZagOrderReversed(i, j, 1, zigZagArray);
		}
	}
	
	iterator = 0;
	for(i=0; i<numCol; i=i+8) {
		for(j=0; j<numRow; j=j+8) {
			for(k=0; k<64; k++) {
				zigZagArray[k] = lineCR[iterator];
				iterator += 1;
			}
			zigZagOrderReversed(i, j, 2, zigZagArray);
		}
	}
	
	/*======================= FUNKCIJE ==========================================*/
	
	discreteCosineTransformReversed(numCol, numRow);
	shiftPositive128(numCol, numRow);
	convertToRGB(numCol, numRow);
	
	/*====================== ZAPIS U DATOTEKU ===================================*/
	
	writeFile = fopen("out.ppm", "wb+");
	
	fprintf(writeFile, "P6");
	fprintf(writeFile, "\n");
	fprintf(writeFile, "%d %d", numCol, numRow);
	fprintf(writeFile, "\n");
	fprintf(writeFile, "%d", 255);
	fprintf(writeFile, "\n");
	
	for(i=0; i<numCol; i++) {
		for(j=0; j<numRow; j++) {
			for(k=0; k<3; k++) {
				if(rgb[i][j][k] < 0) {
					rgb[i][j][k] = 0;
				} else if(rgb[i][j][k] > 255) {
					rgb[i][j][k] = 255;
				}
				fprintf(writeFile, "%c", rgb[i][j][k]);
			}
		}
	}
	
	fclose(readFile);
	fclose(writeFile);
	
	return 0;
}