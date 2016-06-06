#pragma once
#include <fstream>

using namespace std;

class fileWriter
{
private:
	ofstream* file;

	void CloseFile();

public:
	fileWriter(char*);
	~fileWriter();

	void WriteLine(char*);
};
