#pragma once
#include <fstream>

using namespace std;

class fileWriter
{
private:
	ofstream* file;



public:
	void CloseFile();
	fileWriter(char*);
	~fileWriter();

	void WriteLine(char*);
};
