#include "stdafx.h"
#include "automath.h"
#include "Parser.h"

#include <iostream>

automat::Token automat::str_to_token(char* s)
{
	if (!strcmp(s, "SLZSymbolComment"))
		return Token::SingleLineComment;

	if (!strcmp(s, "MLZSymbolComment"))
		return Token::MultilineComment;

	if (!strcmp(s, "zeroSymbolString"))
		return Token::String;

	if (!strcmp(s, "isreservedWord"))
		return Token::ReservedWord;

	if (!strcmp(s, "zeroSymbolDigit"))
		return Token::Digit;

	if (!strcmp(s, "zeroSymbolChar"))
		return Token::Char;

	if (!strcmp(s, "isbracket"))
		return Token::Bracket;

	if (!strcmp(s, "isStar"))
		return Token::Star;

	if (!strcmp(s, "isCompOper"))
		return Token::CompOper;

	if (!strcmp(s, "isLogicOper"))
		return Token::LogicOper;

	if (!strcmp(s, "isBiteOp"))
		return Token::BiteOp;

	if (!strcmp(s, "isAssignment"))
		return Token::Assignment;

	if (!strcmp(s, "isSeparator"))
		return Token::Separator;

	if (!strcmp(s, "isUserType"))
		return Token::UserType;

	if (!strcmp(s, "isSpace"))
		return Token::Space;

	if (!strcmp(s, "isNewLine"))
		return Token::NewLine;

	if (!strcmp(s, "isReservedType"))
		return Token::ReservedType;

	if (!strcmp(s, "isCondition"))
		return Token::Condition;

	if (!strcmp(s, "isSysFunc"))
		return Token::SysFunction;

	if (!strcmp(s, "isArOp"))
		return Token::ApOp;

	if (!strcmp(s, "isIncOrDec"))
		return Token::IncOrDec;

}

bool automat::is_letter(char c)
{
	if ((c >= (char)'a') && (c <= (char)'z')) return true;
	if ((c >= (char)'A') && (c <= (char)'Z')) return true;

	return false;
}

automat::automat(char* fileName)
{
	Parser = new parser(fileName);
}

automat::~automat()
{
	if (Parser) delete Parser;
}

int automat::words_automat(char* s, char* startCondition, char* endCondition)
{
	char* currentCondition = startCondition;
	int i = 0;
	for (; i < strlen(s) && is_letter(s[i]); i++);
	char* temp = new char[i];
	Copy(temp, s, i);
	currentCondition = Parser->find(currentCondition, temp);

	if (!strcmp(currentCondition, endCondition))
	{
		return i;
	}

	return 0;
}

char* automat::one_char_string(char c)
{
	char* temp = new char[2];
	temp[0] = c;
	temp[1] = '\0';
	return temp;
}

automat::parseResult* automat::string_parse(char* s)
{
	Parser->reset_start_conditionI();
	char* startCondition = Parser->get_start_contition();

	//���������� ��� ��������
	while (startCondition)
	{
		//��������� ������� ��� ����������������� ����
		if (!strcmp(startCondition, "isReservedWord"))
		{
			int lenght = words_automat(s, startCondition, "reservedWord");
			if (lenght == 0)
			{
				startCondition = Parser->get_start_contition();
				continue;
			}

			char* word = new char[lenght];
			Copy(word, s, lenght);
			return new parseResult(Token::ReservedWord, word);

		}

		//��������� ������� ��� ��������� �������
		if (!strcmp(startCondition, "isSysFunc"))
		{
			int lenght = words_automat(s, startCondition, "sysFunc");
			if (lenght == 0)
			{
				startCondition = Parser->get_start_contition();
				continue;
			}

			char* word = new char[lenght];
			Copy(word, s, lenght);
			return new parseResult(Token::SysFunction, word);
		}

		//��������� ������� ��� ����������� ����� ������
		if (!strcmp(startCondition, "isReservedType"))
		{
			int lenght = words_automat(s, startCondition, "reservedType");
			if (lenght == 0)
			{
				startCondition = Parser->get_start_contition();
				continue;
			}

			char* word = new char[lenght];
			Copy(word, s, lenght);
			return new parseResult(Token::ReservedType, word);

		}

		//��������� ������� ��� �������� �����������
		if (!strcmp(startCondition, "isCondition"))
		{
			int lenght = words_automat(s, startCondition, "condition");
			if (lenght == 0)
			{
				startCondition = Parser->get_start_contition();
				continue;
			}

			char* word = new char[lenght];
			Copy(word, s, lenght);
			return new parseResult(Token::Condition, word);

		}

		char* currentCondition = startCondition;
		int fragmentLenght = 0;

		//��� ������������ ���������
		for (int i = 0; i < strlen(s); i++)
		{
			char* temp = one_char_string(s[i]);

			currentCondition = Parser->find(currentCondition, temp);


			if (strcmp(currentCondition, "error")) fragmentLenght++;

			if (!strcmp(currentCondition, "error0"))
			{
				break;
			}

			if (!strcmp(currentCondition, "error") || i == strlen(s) - 1)
			{
				if (fragmentLenght != 0)
				{
					char* word = new char[fragmentLenght];
					Copy(word, s, fragmentLenght);
					return new parseResult(str_to_token(startCondition), word);
				}

				else
				{
					break;
				}
			}


			if (!strcmp(currentCondition, "charError"))
				throw ("CharError: ������ ��������� ��������.");

			if (!strcmp(currentCondition, "stringerror"))
				throw ("StringError: ������ ��������� ��������.");
		}

		startCondition = Parser->get_start_contition();
	}
}

List* automat::work(char* s)
{

	parseResult* result;

	List* tokens = new List(sizeof(parseResult));

	while (strlen(s))
	{
		result = string_parse(s);

		tokens->add(result);

		char* temp = new char[strlen(s) - strlen(result->string) + 1];
		Copy(temp, s + strlen(result->string), strlen(s) - strlen(result->string));
		delete[] s;
		s = temp;
	}

	return tokens;
}