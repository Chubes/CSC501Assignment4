#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include "SoundFile.h"
using namespace std;

void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample,
						double outputRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);
void readInput(char *filename);
void convolve(float x[], int N, float h[], int M, float y[], int P);
void convolve(SoundFile impulse);
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

	

	cout << "Input Signal: " << input->signalSize << ", Impulse Size: " << impulse->signalSize << endl;
	int outputSize = input->signalSize + impulse->signalSize - 1;
	float *outputSignal = new float[outputSize];
	
	float *x = new float[input->signalSize];
	for (int i = 0; i < input->signalSize; i++) {
		x[i] = (float)input->signal[i]/32768;
	}
	
	float *h = new float[impulse->signalSize];
	for (int i = 0; i < impulse->signalSize; i++) {
		h[i] = (float)impulse->signal[i]/32768;
	}
	convolve(x, input->signalSize, h, impulse->signalSize, outputSignal, outputSize);

	int i;

	/*  Calculate the number of sound samples to create,
	rounding upwards if necessary  */
	int numSamples = outputSize;


	/*  Open a binary output file stream for writing */
	FILE *outputFileStream = fopen(outputFileName, "wb");

	/*  Write the WAVE file header  */
	writeWaveFileHeader(input->channels, numSamples, input->bitsPerSample,
		input->sampleRate, outputFileStream);

	for (i = 0; i < numSamples; i++)
		fwriteShortLSB(rint(outputSignal[i]*32767*0.9), outputFileStream);


	/*  Close the output file stream  */
	fclose(outputFileStream);


	return 0;

}

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

size_t fwriteShortLSB(short int data, FILE *stream)
{
	unsigned char array[2];

	array[1] = (unsigned char)((data >> 8) & 0xFF);
	array[0] = (unsigned char)(data & 0xFF);
	return fwrite(array, sizeof(unsigned char), 2, stream);
}
void convolve(float x[], int N, float h[], int M, float y[], int P){
	
	int n, m;
	/*	Make sure the output buffer is the right size: P = N + M - 1	*/
	if (P != (N + M - 1)) {
		printf("Output signal vector is the wrong size\n"); printf("It is %-d, but should be %-d\n", P, (N + M - 1)); printf("Aborting convolution\n"); return;
	}
	/* Clear the output buffer y[] to all zero values */
	for (n = 0; n < P; n++)
		y[n] = 0.0;
	/* Do the convolution */ /* Outer loop: process each input value x[n] in turn */
							 /* Inner loop: process x[n] with each sample of h[] */
	for (n = 0; n < N; n++) {
		for (m = 0; m < M; m++)
			y[n + m] += (x[n] * h[m]);
	}

}

