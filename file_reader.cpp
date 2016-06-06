#include "stdafx.h"
#include "file_reader.h"
#include <fstream>
#include "automath.h"
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
		automat::Copy(temp, file, strlen(file));
		automat::Copy(temp + strlen(file), line, strlen(line));
		automat::Copy(temp + strlen(temp), "\n", 1);
		delete[] file;
		delete[] line;
		file = temp;
	}

	return file;
}
char* fileReader::readSourceCode()
{
	char* temp = readFile();
	char* temp2 = new char[strlen(temp) + 1];

	int i, j;
	for (i = 0, j = 0; i < strlen(temp); i++)
	{
		if (temp[i] == '\t')
		{
			temp2[j++] = ' ';
		}

		else if (temp[i] == '\\' && i + 1 < strlen(temp) && temp[i + 1] == 'n')
		{
			temp2[j++] = '\n';
			i++;
		}
		else
		{
			temp2[j++] = temp[i];
		}
	}
	temp2[j] = '\0';

	return temp2;
}
void fileReader::closeFile()
{
	if (file) file->close();
}