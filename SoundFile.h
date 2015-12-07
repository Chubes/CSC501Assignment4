#ifndef SoundFileH
#define SoundFileH

class CWav
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

	short *signal;
	int SignalSize;
	char* 	Data;
public:

	void readInput(char *fileName);

};

#endif
