/* File to read the input of a wav file, based on a 
* file from previous years TA found here:
* http://people.ucalgary.ca/~asarrafs/Abbas_Sarraf/F11-501/F11-501.html
*
*
*/

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

	inFile.seekg(4, ios::beg);
	inFile.read((char*)&chunkSize, 4); 

	inFile.seekg(16, ios::beg);
	inFile.read((char*)&subChunk1Size, 4);

	inFile.seekg(20, ios::beg);
	inFile.read((char*)&format, sizeof(short)); 
	inFile.read((char*)&channels, sizeof(short)); 													
	inFile.read((char*)&sampleRate, sizeof(int)); 													
	inFile.read((char*)&byteRate, sizeof(int)); 												  
	inFile.read((char*)&blockAlign, sizeof(short)); 
	inFile.read((char*)&bitsPerSample, sizeof(short)); 
	if (subChunk1Size == 18) { //handle if subchunk size isn't 16
		inFile.seekg(42, ios::beg);
		inFile.read((char*)&dataSize, sizeof(int)); 
	}
	else
	{
		inFile.seekg(40, ios::beg);
		inFile.read((char*)&dataSize, sizeof(int)); 
	}

	data = new char[dataSize];
	inFile.seekg(44, ios::beg);
	inFile.read(data, dataSize);

	inFile.close(); // close the input file

	signal = NULL;
	//if 1 Byte per sample, add one to one
	if (bitsPerSample == 8)
	{
		signalSize = dataSize;
		signal = new short[signalSize];
		for (int i = 0; i < dataSize; i++)
			signal[i] = (short)((unsigned char)data[i]);

	}
	//if 2 bytes per sample combine them and add them to signal
	else  {
		signalSize = dataSize / 2;
		signal = new short[signalSize];
		short val;
		for (int i = 0; i < dataSize; i += 2)
		{
			val = (short)((unsigned char)data[i]);
			val += (short)((unsigned char)data[i + 1]) * 256; //shift by 8 spots and add
			signal[i / 2] = val;
		}
	}
}