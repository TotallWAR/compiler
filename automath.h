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
	parseResult* string_parse(char* s);


	int words_automat(char*, char*, char*);
	List* work(char*);
private:
	parser* Parser;

	Token str_to_token(char*);
	bool is_letter(char);
	char* one_char_string(char);
	void Copy(char* destination, char* source, int lenght)
	{
		for (int i = 0; i < lenght; i++)
		{
			destination[i] = source[i];
		}
		destination[lenght] = '\0';
	}
};
#endif