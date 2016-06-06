
#ifndef PARSER_H
#define PARSER_H

#include "file_reader.h"
#include "Headers\list.h"

class parser
{
private:
	fileReader* file_reader;
	List* parse_triads;
	List* start_conditions;
	int start_conditionI;

	struct Triad
	{
		char* current_state;
		char* next_state;
		char* symbol;

		Triad(char* _currentState, char* _symbol, char* _nextState)
		{
			current_state = _currentState;
			symbol = _symbol;
			next_state = _nextState;
		}
	};

	void add_triad(char* currState, char*symbol, char*nextState);

	List* split(char*);
	char* three_concat(char*, char*, char*);

public:

	parser(char* fileName);
	~parser();
	void string_parse(char*);
	char* find(char*, char*);
	char* get_start_contition();
	void reset_start_conditionI();
};
#endif