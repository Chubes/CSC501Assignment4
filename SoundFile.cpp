#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include "SoundFile.h"

using namespace std;

void SoundFile::readInput(char *filename)
{

	ifstream inFile(filename, ios::in | ios::binary);

	printf("Reading wav file...\n"); // for debugging only

	inFile.seekg(4, ios::beg);
	inFile.read((char*)&chunkSize, 4); // read the ChunkSize

	inFile.seekg(16, ios::beg);
	inFile.read((char*)&subChunk1Size, 4); // read the SubChunk1Size

	inFile.seekg(20, ios::beg);
	inFile.read((char*)&format, sizeof(short)); // read the file format.  This should be 1 for PCM


	inFile.read((char*)&channels, sizeof(short)); // read the # of channels (1 or 2)

													//inFile.seekg(24, ios::beg);
	inFile.read((char*)&sampleRate, sizeof(int)); // read the samplerate

													//inFile.seekg(28, ios::beg);
	inFile.read((char*)&byteRate, sizeof(int)); // read the byterate

												  //inFile.seekg(32, ios::beg);
	inFile.read((char*)&blockAlign, sizeof(short)); // read the blockalign

													  //inFile.seekg(34, ios::beg);
	inFile.read((char*)&bitsPerSample, sizeof(short)); // read the bitspersample
	if (subChunk1Size == 18) { //handle if subchunk size isn't 16
		inFile.seekg(42, ios::beg);
		inFile.read((char*)&dataSize, sizeof(int)); // read the size of the data
	}
	else
	{
		inFile.seekg(40, ios::beg);
		inFile.read((char*)&dataSize, sizeof(int)); // read the size of the data
	}

												  // read the data chunk
	data = new char[dataSize];
	inFile.seekg(44, ios::beg);
	inFile.read(data, dataSize);

	inFile.close(); // close the input file

	signal = NULL;

	if (bitsPerSample == 8)
	{
		signalSize = dataSize;
		signal = new short[signalSize];
		for (int i = 0; i < dataSize; i++)
			signal[i] = (short)((unsigned char)data[i]);

	}
	else  {
		signalSize = dataSize / 2;
		signal = new short[signalSize];
		short val;
		for (int i = 0; i < dataSize; i += 2)
		{
			val = (short)((unsigned char)data[i]);
			val += (short)((unsigned char)data[i + 1]) * 256;
			signal[i / 2] = val;
		}
	}
}