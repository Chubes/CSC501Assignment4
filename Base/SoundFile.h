#ifndef SoundFileH
#define SoundFileH

class SoundFile
{
private:
	
public:
	int 	chunkSize;
	int		subChunk1Size;
	short 	format;
	short 	channels;
	int   	sampleRate;
	int   	byteRate;
	short 	blockAlign;
	short 	bitsPerSample;
	int		dataSize;
	short	*signal;
	int		signalSize;
	char* 	data;
public:

	void readInput(char *fileName);

};

#endif
