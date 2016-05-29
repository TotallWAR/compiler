#ifndef AUTOMAT_H
#define AUTOMAT_H
#include "../Headers/list.h"
#include "parser.h"

class automat
{
public:
	enum Token //
	{
		String,					//0
		SingleLineComment,		//1
		MultilineComment,		//2
		Digit,					//3
		Char,					//4
		Bracket,				//5
		ReservedWord,			//6
		Star,					//7
		CompOper,				//8
		LogicOper,				//9
		BiteOp,					//10
		Assignment,				//11
		SysFunction,			//12
		Separator,				//13
		UserType,				//14
		Space,					//15
		NewLine,				//16
		ReservedType,			//17
		Condition,				//18
		ApOp,					//19
		IncOrDec				//20
	};
	automat(char * filename);
	~automat();
	struct parseResult
	{
		Token token;
		char* string;

		parseResult(Token _token, char* _string)
		{
			token = _token;
			string = _string;
		}
	};
	parseResult* stringParse(char* s);
private:
	parser* Parser;

};
#endif