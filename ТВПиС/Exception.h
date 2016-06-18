#pragma once

class Exception
{
	char* _message;
	int _line;

public:
	// Line value equals -1 if line is not specified.
	Exception(char* message, int line = -1);
	~Exception();

	char* GetMessage();
	int GetLine();
};