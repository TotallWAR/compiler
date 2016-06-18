#include "stdafx.h"
#include "Exception.h"
Exception::Exception(char* message, int line)
{
	_message = message;
	_line = line;
}

Exception::~Exception()
{	_line = 0;
}
char* Exception::GetMessage()
{
	return _message;
}
int Exception::GetLine()
{
	return _line;
}