#include "stdafx.h"
#include "Headers\list.h"
#include  "file_reader.h"
#include "Parser.h"

parser::parser(char* fileName)
{
	start_conditionI = 0;
	parse_triads = new List(sizeof(Triad));
	start_conditions = new List(20);//20 токенов (т.е. числа, скобки, операции и тд)

	//	fileWritter = nullptr;//new FileWritter(LOG);

	file_reader = new fileReader(fileName);
	char* s;
	while ((s = file_reader->readLine()) != NULL)
	{
		string_parse(s);
	}
	delete file_reader;
}
parser::~parser()
{
	if (parse_triads) delete parse_triads;
	if (start_conditions) delete start_conditions;
	//	if (fileWritter) delete fileWritter;
}

//разбивает каждую строку на состояния и слова и запихивает в соответствующие списки
void parser::string_parse(char* s)
{
	if (strlen(s) == 0) //если длина строки 0 -- выходим
		return;
	if (s[0] == '#')
		return; //Комментарии, то выходим
	if (strlen(s)>5) // после start 
	{
		char* start = new char[6];
		strncpy(start, s, 5);
		if (!strcmp(start, "start"))//если строка начинается с start
		{
			char* startCondition = new char[strlen(s) - 6];
			strncpy(startCondition, s + 6, strlen(s) - 6);
			start_conditions->add(startCondition);
			return;
		}
	}

	//Поиск позиций начала подстрокc НА РАЗБОР СОЛО
	int tabPositions[2];
	for (int i = 0, j = 0; i < (signed)strlen(s) && j < 2; i++)
	{
		if (s[i] == '\t') tabPositions[j++] = i + 1;
	}

	char* current_state = new char[tabPositions[0]];
	char* symbols = new char[tabPositions[1] - tabPositions[0]];
	char* next_state = new char[strlen(s) - tabPositions[1]];

	strncpy(current_state, s, tabPositions[0] - 1);
	strncpy(symbols, s + tabPositions[0], tabPositions[1] - tabPositions[0] - 1);
	strncpy(next_state, s + tabPositions[1], strlen(s) - tabPositions[1]);

	List* symbolList = split(symbols);

	int symbolCount = symbolList->count();
	for (int i = 0; i < symbolCount; i++)
	{
		char* temp = (char*)symbolList->get(0);
		add_triad(current_state, next_state, temp);
	}

	delete symbolList;
}

//добавление состояния в список состояний
void parser::add_triad(char* current_state, char* symbol, char* next_state)
{
	Triad* temp = new Triad(current_state, symbol, next_state);
	parse_triads->add(temp);
}

//на strcpy ругалось, решили эту зафигачить
inline void copy(char* destination, char* source, int lenght)
{
	for (int i = 0; i < lenght; i++)
	{
		destination[i] = source[i];
	}
	destination[lenght] = '\0';
}


char* parser::three_concat(char* s1, char* s2, char* s3)
{
	//временный
	char* temp = new char[strlen(s1) + strlen(s2) + strlen(s3) + 1];//нулевой, ну яснА
	copy(temp, s1, strlen(s1));
	copy(temp + strlen(s1), "\t", 1);
	copy(temp + strlen(s1) + 1, s2, strlen(s2));
	copy(temp + strlen(s1) + 1 + strlen(s2), "\t", 1);
	copy(temp + strlen(s1) + 1 + strlen(s2) + 1, s3, strlen(s3));

	return temp;
}

//обычный сплит на запятые, табуляции и тд, возвращается соответственно список
List* parser::split(char* s)
{
	char* symbol = new char[20];
	List* symbolList = new List(20); //Список разделенных слов

	//Оператор запятая
	if (strlen(s) == 1 && s[0] == ',')
	{
		symbol[0] = ',';
		symbol[1] = '\0';

		symbolList->add(symbol);
		return symbolList;
	}

	//Если escape-последовательность
	if (strlen(s) == 2 && s[0] == '\\')
	{
		if (s[1] == 'n')
			symbol[0] = '\n';
		if (s[1] == 't')
			symbol[0] = '\t';
		if (s[1] == '0')
			symbol[0] = '\0';
		symbol[1] = '\0';

		symbolList->add(symbol);
		return symbolList;
	}

	//Если несколько символов или слов, разделенных запятой
	int startPosition = 0;
	int i = 0;
	for (i = 0; i < (signed)strlen(s); i++) //!!!!!!!!!!!!!!!!!!!!
	{
		if (s[i] == ',')
		{
			strncpy(symbol, s + startPosition, i - startPosition);
			startPosition = i + 1;
			symbolList->add(symbol);
		}
	}
	strncpy(symbol, s + startPosition, i - startPosition);
	symbolList->add(symbol);
	return symbolList;
}


//получение дескриптора первого свободного участка для записи в листе
char* parser::get_start_contition()
{
	if (start_conditionI<start_conditions->count())
		return ((char*)start_conditions->get_accessor(start_conditionI++));

	return nullptr;
}

//обнуление начального кондишина
void parser::reset_start_conditionI()
{
	start_conditionI = 0;
}

//находим состояние в которое перевелось по предыдущему и по слову
char* parser::find(char* condition, char* word)
{
	Triad* temp = nullptr;

	for (int i = 0; i < parse_triads->count(); i++)
	{
		temp = (Triad*)parse_triads->get_accessor(i);
		if (!strcmp(temp->current_state, condition) && (!strcmp(temp->symbol, word)))
		{
			char* writeString = three_concat(temp->current_state, temp->symbol, temp->next_state);

			//if (fileWritter) fileWritter->WriteLine(writeString);!!!!!!!!!!!!!!!!!!!!!!!!!!!!aklsdflkasdflkasjdlkfjaskldfjlkasjdf

			return temp->next_state;
		}
		if (!strcmp(temp->current_state, condition) && (!strcmp(temp->symbol, "...")))
		{
			char* writeString = three_concat(temp->current_state, word, temp->next_state);

			//if (fileWritter) fileWritter->WriteLine(writeString);!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			return temp->next_state;
		}
	}
}