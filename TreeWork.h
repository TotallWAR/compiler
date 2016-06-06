#include "Headers/list.h"
#include "automath.h"

class TreeWork
{
private:
	List* tokens;
	bool isOperator(automat::Token token);
	void Prioritize(List* tokenList);
	bool IsFraction(char* s);
	List* OldTokenToNewToken(List*);
	double StrToDouble(char* s);
	void PrintList(List* tokens);
//	Scope* currentScope;
	struct NewToken
	{
		automat::Token type;
		char* string;
		double value;
		int priority;
		int lineIndex;

		NewToken(automat::Token type, char* string, double value, int priority, int lineIndex)
		{
			this->type = type;
			this->string = string;
			this->priority = priority;
			this->value = value;
			this->lineIndex = lineIndex;
		}
	};
public:
	TreeWork(char *);
	~TreeWork();


};
