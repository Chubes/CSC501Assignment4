#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include "SoundFile.h"
using namespace std;

#define SWAP(a,b)  tempr=(a);(a)=(b);(b)=tempr




void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample,
							double outputRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);
void readInput(char *filename);
void four1(double data[], int nn, int isign);
SoundFile *input;

char* inputFileName = NULL;
char* IRFileName = NULL;
char* outputFileName = NULL;

int main(int argc, char *argv[]) {



	if (argc == 4) {
		inputFileName = argv[1];
		IRFileName = argv[2];
		outputFileName = argv[3];
	}
	else {
		/*  The user did not supply the correct number of command-line
		arguments.  Print out a usage message and abort the program.  */
		fprintf(stderr, "Usage:  %s outputFilename\n", argv[0]);
		exit(-1);
	}

	input = new SoundFile();
	input->readInput(inputFileName);
	SoundFile *impulse = new SoundFile();
	impulse->readInput(IRFileName);

	double *x = new double[input->signalSize];
	for (int i = 0; i < input->signalSize; i++) {
		x[i] = (double)input->signal[i] / 32768.0;
	}

	double *h = new double[impulse->signalSize];
	for (int i = 0; i < impulse->signalSize; i++) {
		h[i] = (double)impulse->signal[i] / 32768;
	}


	int complexSize;
	if (input->signalSize > impulse->signalSize) {
		complexSize = input->signalSize;
	}
	else {
		complexSize = impulse->signalSize;
	}

	int power = (int)log2(complexSize) + 1;
	power = pow(2, power);
	complexSize = 2 * power;

	double *complexX = new double[complexSize];
	double *complexH = new double[complexSize];
	//pad both with zeroes
	for (int i = 0; i < complexSize; i += 2) {
		complexX[i] = 0.0;
		complexH[i] = 0.0;
	}

	for (int i = 0; i < input->signalSize; i++) {
		complexX[2 * i] = x[i];
	}

	for (int i = 0; i < impulse->signalSize; i++) {
		complexH[2 * i] = h[i];
	}
	int i;

	four1(complexX-1, power, 1);
	four1(complexH-1, power, 1);

	double *complexY = new double[complexSize];
	for (i = 0; i < power; i += 2) {
		complexY[i] = complexX[i] * complexH[i] - complexX[i + 1] * complexH[i + 1];
		complexY[i + 1] = complexX[i + 1] * complexH[i] + complexX[i] * complexH[i + 1];
		
	}
	four1(complexY - 1, power, -1);

	int numSamples = power;



	FILE *outputFileStream = fopen(outputFileName, "wb");


	writeWaveFileHeader(input->channels, numSamples, input->bitsPerSample, input->sampleRate, outputFileStream);
	//scale by the size of the number of samples	
	for (i = 0; i < power; i += 2) {
		complexY[i] /= (double)power;
		complexY[i + 1] /= (double)power;
	}
	
	for (i = 0; i < numSamples; i++) {
		fwriteShortLSB(rint((short)(complexY[i] * 32767)), outputFileStream);
	}


	fclose(outputFileStream);
	
	return 0;

}
//taken from the testtone.c file given to us
void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample,
	double outputRate, FILE *outputFile)
{
	/*  Calculate the total number of bytes for the data chunk  */
	int dataChunkSize = channels * numberSamples * (bitsPerSample / 8);

	/*  Calculate the total number of bytes for the form size  */
	int formSize = 36 + dataChunkSize;

	/*  Calculate the total number of bytes per frame  */
	short int frameSize = channels * (bitsPerSample / 8);

	/*  Calculate the byte rate  */
	int bytesPerSecond = (int)ceil(outputRate * frameSize);

	/*  Write header to file  */
	/*  Form container identifier  */
	fputs("RIFF", outputFile);

	/*  Form size  */
	fwriteIntLSB(formSize, outputFile);

	/*  Form container type  */
	fputs("WAVE", outputFile);

	/*  Format chunk identifier (Note: space after 't' needed)  */
	fputs("fmt ", outputFile);

	/*  Format chunk size (fixed at 16 bytes)  */
	fwriteIntLSB(16, outputFile);

	/*  Compression code:  1 = PCM  */
	fwriteShortLSB(1, outputFile);

	/*  Number of channels  */
	fwriteShortLSB((short)channels, outputFile);

	/*  Output Sample Rate  */
	fwriteIntLSB((int)outputRate, outputFile);

	/*  Bytes per second  */
	fwriteIntLSB(bytesPerSecond, outputFile);

	/*  Block alignment (frame size)  */
	fwriteShortLSB(frameSize, outputFile);

	/*  Bits per sample  */
	fwriteShortLSB(bitsPerSample, outputFile);

	/*  Sound Data chunk identifier  */
	fputs("data", outputFile);

	/*  Chunk size  */
	fwriteIntLSB(dataChunkSize, outputFile);
}
/******************************************************************************
*
*       function:       fwriteIntLSB
*
*       purpose:        Writes a 4-byte integer to the file stream, starting
*                       with the least significant byte (i.e. writes the int
*                       in little-endian form).  This routine will work on both
*                       big-endian and little-endian architectures.
*
*       internal
*       functions:      none
*
*       library
*       functions:      fwrite
*
******************************************************************************/
//taken from the testtone.c file given to us
size_t fwriteIntLSB(int data, FILE *stream)
{
	unsigned char array[4];

	array[3] = (unsigned char)((data >> 24) & 0xFF);
	array[2] = (unsigned char)((data >> 16) & 0xFF);
	array[1] = (unsigned char)((data >> 8) & 0xFF);
	array[0] = (unsigned char)(data & 0xFF);
	return fwrite(array, sizeof(unsigned char), 4, stream);
}
/******************************************************************************
*
*       function:       fwriteShortLSB
*
*       purpose:        Writes a 2-byte integer to the file stream, starting
*                       with the least significant byte (i.e. writes the int
*                       in little-endian form).  This routine will work on both
*                       big-endian and little-endian architectures.
*
*       internal
*       functions:      none
*
*       library
*       functions:      fwrite
*
******************************************************************************/
//taken from the testtone.c file given to us
size_t fwriteShortLSB(short int data, FILE *stream)
{
	unsigned char array[2];

	array[1] = (unsigned char)((data >> 8) & 0xFF);
	array[0] = (unsigned char)(data & 0xFF);
	return fwrite(array, sizeof(unsigned char), 2, stream);
}

//  The four1 FFT from Numerical Recipes in C,
//  p. 507 - 508.
//  Note:  changed float data types to double.
//  nn must be a power of 2, and use +1 for
//  isign for an FFT, and -1 for the Inverse FFT.
//  The data is complex, so the array size must be
//  nn*2. This code assumes the array starts
//  at index 1, not 0, so subtract 1 when
//  calling the routine

//taken from the test.c file given to us

void four1(double data[], int nn, int isign)
{
	unsigned long n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;

	n = nn << 1;
	j = 1;

	for (i = 1; i < n; i += 2) {
		if (j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j + 1], data[i + 1]);
		}
		m = nn;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}

	mmax = 2;
	while (n > mmax) {
		istep = mmax << 1;
		theta = isign * (6.28318530717959 / mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i <= n; i += istep) {
				j = i + mmax;
				tempr = wr * data[j] - wi * data[j + 1];
				tempi = wr * data[j + 1] + wi * data[j];
				data[j] = data[i] - tempr;
				data[j + 1] = data[i + 1] - tempi;
				data[i] += tempr;
				data[i + 1] += tempi;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		mmax = istep;
	}
}

