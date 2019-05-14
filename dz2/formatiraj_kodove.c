#include <stdio.h>
#include <stdlib.h>

int main(void) {
	FILE *writeFile;
	writeFile = fopen("0036491975_3zadatak.txt", "w+");
	
	fprintf(writeFile, "%s %s\n", "0", "111000000");
	fprintf(writeFile, "%s %s\n", "1", "111001");
	fprintf(writeFile, "%s %s\n", "2", "101");
	fprintf(writeFile, "%s %s\n", "3", "0110");
	fprintf(writeFile, "%s %s\n", "4", "1111");
	fprintf(writeFile, "%s %s\n", "5", "001");
	fprintf(writeFile, "%s %s\n", "6", "010");
	fprintf(writeFile, "%s %s\n", "7", "110");
	fprintf(writeFile, "%s %s\n", "8", "100");
	fprintf(writeFile, "%s %s\n", "9", "0111");
	fprintf(writeFile, "%s %s\n", "10", "0000");
	fprintf(writeFile, "%s %s\n", "11", "0001");
	fprintf(writeFile, "%s %s\n", "12", "11101");
	fprintf(writeFile, "%s %s\n", "13", "1110001");
	fprintf(writeFile, "%s %s\n", "14", "11100001");
	fprintf(writeFile, "%s %s", "15", "111000001");
	
	fclose(writeFile);
	return 0;
}