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
	inFile.read((char*)&ChunkSize, 4); // read the ChunkSize

	inFile.seekg(16, ios::beg);
	inFile.read((char*)&SubChunk1Size, 4); // read the SubChunk1Size

	inFile.seekg(20, ios::beg);
	inFile.read((char*)&Format, sizeof(short)); // read the file format.  This should be 1 for PCM


	inFile.read((char*)&Channels, sizeof(short)); // read the # of channels (1 or 2)

													//inFile.seekg(24, ios::beg);
	inFile.read((char*)&SampleRate, sizeof(int)); // read the samplerate

													//inFile.seekg(28, ios::beg);
	inFile.read((char*)&ByteRate, sizeof(int)); // read the byterate

												  //inFile.seekg(32, ios::beg);
	inFile.read((char*)&BlockAlign, sizeof(short)); // read the blockalign

													  //inFile.seekg(34, ios::beg);
	inFile.read((char*)&BitsPerSample, sizeof(short)); // read the bitspersample
	if (SubChunk1Size == 18) { //handle if subchunk size isn't 16
		inFile.seekg(42, ios::beg);
		inFile.read((char*)&DataSize, sizeof(int)); // read the size of the data
	}
	else
	{
		inFile.seekg(40, ios::beg);
		inFile.read((char*)&DataSize, sizeof(int)); // read the size of the data
	}

												  // read the data chunk
	Data = new char[DataSize];
	inFile.seekg(44, ios::beg);
	inFile.read(Data, DataSize);

	inFile.close(); // close the input file

	signal = NULL;

	if (BitsPerSample == 8)
	{
		signalSize = DataSize;
		signal = new short[signalSize];
		for (int i = 0; i < DataSize; i++)
			signal[i] = (short)((unsigned char)Data[i]);

	}
	else  {
		signalSize = DataSize / 2;
		signal = new short[signalSize];
		short val;
		for (int i = 0; i < DataSize; i += 2)
		{
			val = (short)((unsigned char)Data[i]);
			val += (short)((unsigned char)Data[i + 1]) * 256;
			signal[i / 2] = val;
		}
	}
}