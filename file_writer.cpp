#include "stdafx.h"
#include "file_writer.h"

void fileWriter::CloseFile()
{
	if (file) file->close();
}
fileWriter::fileWriter(char* filename)
{
	file = new ofstream();
	file->open(filename);
}
fileWriter::~fileWriter()
{
	fileWriter::CloseFile();
}

void fileWriter::WriteLine(char* s)
{
	*file << s << endl;
}