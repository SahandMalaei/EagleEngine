#ifndef _FILEIO_H
#define _FILEIO_H 1

#include "Eagle.h"

#define FILE_INPUT 0
#define FILE_OUTPUT 1
#define FILE_IO 2

#define FILE_BINARY 0
#define FILE_TEXT 1

namespace ProjectEagle
{
	class FileIO
	{
	private:
		fstream file;

		bool readyToWrite, readyToRead;

	public:
		FileIO();
		~FileIO();

		void openFileForRead(char *address);
		void openFileForWrite(char *address);
		void openFile(char *address, DWORD mode);

		void clear();

		void reset();
		void close();

		bool isEndOfFile(){return file.eof();}
		bool isGood(){return file.good();}

		void write(void *data, int length);

		void writeByte(char c);
		void writeShort(short s);
		void writeLong(long l);
		void writeFloat(float f);
		void writeString(char *s);

		char *read(int size);
		char readByte();
		short readShort();
		long readLong();
		float readFloat();
		char *readString(int length);
	};
};

#endif