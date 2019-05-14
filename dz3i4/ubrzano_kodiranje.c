#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ippcore.h"
#include "ippi.h"

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
	while (c == '\n' || c == '\r' || c == '\t' ||
		c == ' ' || c == '#') {
		if (c == '#') {
			while (c != EOF && c != '\n' && c != '\r') {
				c = getc(inFile);
			}
		}
		else {
			c = getc(inFile);
		}
	}

	if (c >= '0' && c <= '9') {
		int x = 0;

		while (c >= '0' && c <= '9') {
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

	if (c == EOF) {
		printf("ERROR: Premature end of file\n");
		return 0;
	}
	else {
		printf("ERROR: Not a non-negative integer\n");
		return 0;
	}

	return -1;

}


void convertToYCbCr(int numCol, int numRow) {
	int i, j;

	for (i = 0; i < numCol; i++) {
		for (j = 0; j < numRow; j++) {
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
	for (i = 0; i < numCol; i++) {
		for (j = 0; j < numRow; j++) {
			for (k = 0; k < 3; k++) {
				ycbcr[i][j][k] = ycbcr[i][j][k] - 128;
			}
		}
	}
}

void discreteCosineTransform(int numCol, int numRow) {
	int i, j, k, l;

	for (i = 0; i < numCol; i = i + 8) {
		for (j = 0; j < numRow; j = j + 8) {

			Ipp32f Y[8][8] = { 0 };
			Ipp32f CB[8][8] = { 0 };
			Ipp32f CR[8][8] = { 0 };

			Ipp32f yf[64] = { 0 };
			Ipp32f cbf[64] = { 0 };
			Ipp32f crf[64] = { 0 };

			Ipp32f YF[64] = { 0 };
			Ipp32f CBF[64] = { 0 };
			Ipp32f CRF[64] = { 0 };

			int KY[8][8] = { 0 };
			int KCB[8][8] = { 0 };
			int KCR[8][8] = { 0 };

			/* ================= IZRAVNAVANJE MATRICE U 1-D POLJE ========================== */

			for (k = 0; k < 8; k++) {
				for (l = 0; l < 8; l++) {
					yf[k * 8 + l] = ycbcr[k + i][l + j][0];
					cbf[k * 8 + l] = ycbcr[k + i][l + j][1];
					crf[k * 8 + l] = ycbcr[k + i][l + j][2];
				}
			}

			/* =============== IZVODJENJE IPP FUNKCIJA ZA UBRZANJE ======================= */

			ippiDCT8x8Fwd_32f_C1(yf, YF);
			ippiDCT8x8Fwd_32f_C1(cbf, CBF);
			ippiDCT8x8Fwd_32f_C1(crf, CRF);

			/* ============= VRACANJE IZRACUNATIH PODATAKA U 2-D POLJE =================== */

			for (k = 0; k < 64; k++) {
				Y[k / 8][k % 8] = YF[k];
				CB[k / 8][k % 8] = CBF[k];
				CR[k / 8][k % 8] = CRF[k];
			}

			for (k = 0; k < 8; k++) {
				for (l = 0; l < 8; l++) {
					dct[k + i][l + j][0] = (int)Y[k][l];
					dct[k + i][l + j][1] = (int)CB[k][l];
					dct[k + i][l + j][2] = (int)CR[k][l];
				}
			}

			/*================ KVANTIZACIJA KOMPONENTI =======================*/

			for (k = 0; k < 8; k++) {
				for (l = 0; l < 8; l++) {
					KY[k][l] = (round)(Y[k][l] / K1[k][l]);
					KCB[k][l] = (round)(CB[k][l] / K2[k][l]);
					KCR[k][l] = (round)(CR[k][l] / K2[k][l]);
				}
			}

			for (k = 0; k < 8; k++) {
				for (l = 0; l < 8; l++) {
					dctQuant[k + i][l + j][0] = KY[k][l];
					dctQuant[k + i][l + j][1] = KCB[k][l];
					dctQuant[k + i][l + j][2] = KCR[k][l];
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
	while (1) {
		j++;
		zigZag[k++] = dctQuant[v + i][s + j][faktor];

		while (j != 0) {
			i++;
			j--;

			zigZag[k++] = dctQuant[v + i][s + j][faktor];
		}
		i++;
		if (i > 7) {
			i--;
			break;
		}

		zigZag[k++] = dctQuant[v + i][s + j][faktor];

		while (i != 0) {
			i--;
			j++;
			zigZag[k++] = dctQuant[v + i][s + j][faktor];
		}
	}
	/*======================= DONJI TROKUT MATRICE ============================*/
	while (1) {
		j++;
		zigZag[k++] = dctQuant[v + i][s + j][faktor];
		while (j != 7) {

			j++;
			i--;

			zigZag[k++] = dctQuant[v + i][s + j][faktor];
		}
		i++;
		if (i > 7) {
			i--;
			break;
		}

		zigZag[k++] = dctQuant[v + i][s + j][faktor];

		while (i != 7) {
			i++;
			j--;
			zigZag[k++] = dctQuant[v + i][s + j][faktor];
		}
	}
}

int main(int argc, char *argv[]) {
	char type[3];
	int numRow, numCol, maxNum;
	int i, j, k;
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

	numCol = getHeader(readFile);
	if (numCol <= 0) {
		printf("Invalid width\n");
		return 0;
	}
	numRow = getHeader(readFile);
	if (numRow <= 0) {
		printf("Invalid height\n");
		return 0;
	}
	maxNum = getHeader(readFile);
	if (maxNum <= 0 || maxNum >= 65536) {
		printf("Invalid maxNum\n");
		return 0;
	}


	fseek(readFile, 1, SEEK_CUR);

	for (i = 0; i < numCol; i++) {
		for (j = 0; j < numRow; j++) {
			for (k = 0; k < 3; k++) {
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

	for (i = 0; i < numCol; i = i + 8) {
		for (j = 0; j < numRow; j = j + 8) {
			zigZagOrder(i, j, 0, zigZagArray);
			for (k = 0; k < 64; k++) {
				fprintf(writeFile, "%d ", zigZagArray[k]);
			}
		}
	}

	fprintf(writeFile, "\n");

	for (i = 0; i < numCol; i = i + 8) {
		for (j = 0; j < numRow; j = j + 8) {
			zigZagOrder(i, j, 1, zigZagArray);
			for (k = 0; k < 64; k++) {
				fprintf(writeFile, "%d ", zigZagArray[k]);
			}
		}
	}

	fprintf(writeFile, "\n");

	for (i = 0; i < numCol; i = i + 8) {
		for (j = 0; j < numRow; j = j + 8) {
			zigZagOrder(i, j, 2, zigZagArray);
			for (k = 0; k < 64; k++) {
				fprintf(writeFile, "%d ", zigZagArray[k]);
			}
		}
	}

	fprintf(writeFile, "\n");

	fclose(readFile);
	fclose(writeFile);
	return 0;
}