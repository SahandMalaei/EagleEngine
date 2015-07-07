#include "Eagle.h"

namespace ProjectEagle
{
	FileIO::FileIO()
	{
		readyToRead = readyToWrite = 0;
	}

	FileIO::~FileIO()
	{
		//delete file;
	}

	void FileIO::openFileForRead(char *address)
	{
		file.open(address, ios::binary | ios::in);

		readyToRead = 1;
	}

	void FileIO::openFileForWrite(char *address)
	{
		file.open(address, ios::binary | ios::out);

		readyToWrite = 1;
	}

	void FileIO::openFile(char *address, DWORD mode)
	{
		file.open(address, ios::binary | ios::in | ios::out);

		readyToRead = 1;
		readyToWrite = 1;
	}

	void FileIO::clear()
	{
		if(readyToWrite) file.clear();
	}

	void FileIO::write(void *data, int length)
	{
		if(!readyToWrite) return;

		file.write((char *)data, length);
	}

	void FileIO::writeByte(char c)
	{
		if(!readyToWrite) return;

		file.write(&c, 1);
	}

	void FileIO::writeShort(short s)
	{
		if(!readyToWrite) return;

		file.write((char *)&s, 2);
	}

	void FileIO::writeLong(long l)
	{
		if(!readyToWrite) return;

		file.write((char *)&l, 4);
	}

	void FileIO::writeFloat(float f)
	{
		if(!readyToWrite) return;

		file.write((char *)&f, 4);
	}

	void FileIO::writeString(char *s)
	{
		if(!readyToWrite) return;

		file.write(s, strlen(s) + 1);
	}

	char *FileIO::read(int length)
	{
		if(!readyToRead) return 0;

		char *d = new char[length];
		file.read(d, length);

		return d;
	}

	char FileIO::readByte()
	{
		if(!readyToRead) return 0;

		char c;
		file.read(&c, 1);

		return c;
	}

	short FileIO::readShort()
	{
		if(!readyToRead) return 0;

		short s;
		file.read((char *)&s, 2);

		return s;
	}

	long FileIO::readLong()
	{
		if(!readyToRead) return 0;

		long l = 0;
		file.read((char *)&l, 4);

		return l;
	}

	float FileIO::readFloat()
	{
		if(!readyToRead) return 0;

		float f;
		file.read((char *)&f, 4);

		return f;
	}

	char *FileIO::readString(int length)
	{
		if(!readyToRead) return 0;

		char *s = new char[length];
		file.read(s, length);
	}

	void FileIO::close()
	{
		file.close();

		readyToRead = readyToWrite = 0;
	}

	void FileIO::reset()
	{
		if(readyToRead || readyToWrite) close();
	}
};