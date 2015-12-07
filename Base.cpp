#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
using namespace std;

#define PI					3.14159265358979

/*  Standard sample size in bits  */
#define BITS_PER_SAMPLE		16

/*  Standard sample size in bytes  */
#define BYTES_PER_SAMPLE	(BITS_PER_SAMPLE/8)

/*  Number of channels  */
#define MONOPHONIC			1
//#define STEREOPHONIC		2


void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample,
double outputRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);
void readInput(char *filename);
void convolve(short x[], int N, float h[], int M, short y[], int P);


int main(int argc, char *argv[]) {
	

	char* inputFileName = NULL;
	char* IRFileName = NULL;
	char* outputFileName = NULL;
	if (argc == 4) {
		intputFileName = argv[1];
		IRFileName = argv[2];
		outputFileName = argv[3];
	}
	else {
		/*  The user did not supply the correct number of command-line
		arguments.  Print out a usage message and abort the program.  */
		fprintf(stderr, "Usage:  %s outputFilename\n", argv[0]);
		exit(-1);
	}

	

}


