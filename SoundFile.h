#ifndef SoundFileH
#define SoundFileH

class SoundFile
{
private:
	
public:
	int 	ChunkSize;
	int		SubChunk1Size;
	short 	Format;
	short 	Channels;
	int   	SampleRate;
	int   	ByteRate;
	short 	BlockAlign;
	short 	BitsPerSample;
	int		DataSize;
	short	*signal;
	int		signalSize;
	char* 	Data;
public:

	void readInput(char *fileName);

};

#endif
