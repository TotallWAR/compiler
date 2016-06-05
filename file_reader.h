#ifndef FILEREADER_H
#define FILEREADER_H

#include <iostream>
#include <fstream>

class fileReader
{
private:
	std::ifstream* file;
	void openFile(char * fileName);
	void closeFile();
public:
	fileReader(char* fileName);
	~fileReader();
	char* readLine();
	char* readFile();
};
#endif