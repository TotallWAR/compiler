#include "stdafx.h"
#include "FileReader.h"
#define LINESIZE 1025
fileReader::fileReader(char* fileName)
{
	file = new std::ifstream();
	openFile(fileName);
}
fileReader::~fileReader()
{
	closeFile();
}
void fileReader::openFile(char* fileName)
{
	if (fileName != nullptr)
		file->open(fileName);

	if (!file->is_open())
		std::cout << "error in openFile";
}
char* fileReader::readLine()
{
	if (!file || file->eof())
	{
		return nullptr;
	}

	char* temp = new char[LINESIZE];
	file->getline(temp, LINESIZE);

	return temp;
}
char* fileReader::readFile()
{
	if (!file || file->eof())
	{
		return nullptr;
	}
	char* file = new char[1];
	file[0] = '\0';

	while (char* line = readLine())
	{
		char* temp = new char[strlen(file) + strlen(line) + 2];
		strncpy(temp, file, strlen(file));
		strncpy(temp + strlen(file), line, strlen(line));
		strncpy(temp + strlen(temp), "\n", 1);
		delete[] file;
		delete[] line;
		file = temp;
	}

	return file;
}
void fileReader::closeFile()
{
	if (file) file->close();
}