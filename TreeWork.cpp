#include"stdafx.h"
#include "TreeWork.h"
#include "automath.h"
#include "file_reader.h"

TreeWork::TreeWork(char* SourceCode)
{
	automat* Automat = new automat("Automat.txt");
	fileReader* fR = new fileReader(SourceCode);
	tokens = Automat->work(fR->readSourceCode());
	if (Automat) delete Automat;
	if (fR) delete fR;
	// ���������� ������ ������ � ������ ����.
	tokens = OldTokenToNewToken(tokens);
	PrintList(tokens);

	// ����������� ����������� ��������.
	Prioritize(tokens);

//	currentScope = nullptr;
}
// ����������� �������� ���������� ��� �������� � ������ tokens
// �� 1 �� 16, ��� 16 - ��������� ���������.
void TreeWork::Prioritize(List* tokenList)
{
	NewToken* current;
	NewToken* prev;

	for (int i = 0; i < tokenList->count(); i++)
	{
		current = ((NewToken*)(tokenList->get_accessor(i)));
		prev = NULL;
		if (i > 0)
		{
			prev = ((NewToken*)(tokenList->get_accessor(i - 1)));
		}

		if (isOperator(current->type))
		{
			std::string s = "";
			s += current->string;

			// �������.
			if (s == ",")
			{
				current->priority = 1;
				continue;
			}

			// ������������.
			if (s == "=")
			{
				current->priority = 2;
				continue;
			}

			// ��������� �������� �������� (�����������).
			if ((s == "?") || (s == ":"))
			{
				current->priority = 3;
				continue;
			}

			// ���������� ���.
			if (s == "||")
			{
				current->priority = 4;
				continue;
			}

			// ���������� �.
			if (s == "&&")
			{
				current->priority = 5;
				continue;
			}

			// ��������� ���.
			if (s == "|")
			{
				current->priority = 6;
				continue;
			}

			// ��������� ����������� ���.
			if (s == "^")
			{
				current->priority = 7;
				continue;
			}

			// ������������� ��������� (�����������), ������ ������ (�����������),
			// ������� ���� � �����, ���������� ��������� � ���������.
			if ((s == "+") || (s == "-") || (s == "++") || (s == "--"))
				current->priority = 15;

			// �������� ���� � �����.
			if (((s == "+") || (s == "-")) && ((prev->type == automat::Token::Digit) || prev->type == automat::Token::UserType
				|| prev->type == automat::Token::IncOrDec || !strcmp(prev->string, ")") || !strcmp(prev->string, "]")))
			{
				current->priority = 12;
				continue;
			}

			// ��������� �.
			if ((s == "&") && (prev->type == automat::Token::UserType))
			{
				current->priority = 8;
				continue;
			}

			// ��������� � �����������.
			if ((s == "==") || (s == "!="))
			{
				current->priority = 9;
				continue;
			}

			// ������, ������, ������ ��� �����, ������ ��� �����.
			if ((s == "<") || (s == ">") || (s == "<=") || (s == ">="))
			{
				current->priority = 10;
				continue;
			}

			// ���������, �������, ��������� ������� �� �������.
			if (((s == "*") || (s == "/") || (s == "%")) && ((prev->type == automat::Token::Digit)
				|| (prev->type == automat::Token::UserType) || !strcmp(prev->string, "]") || !strcmp(prev->string, ")")))
			{
				current->priority = 13;
				continue;
			}

			// ���������� ��������� � ���������, ����� �������� �� ��������� (�����������), ������ �������� �������.
			if (((s == "++") || (s == "--") || (s == "->") || (s == "[")) && (prev->type == automat::Token::UserType) || (s == "]"))
				current->priority = 16;

			// ��������� �������
			if (current->type == automat::SysFunction)
				current->priority = 15;
		}
	}
}
List* TreeWork::OldTokenToNewToken(List* old)
{
	List* newTokenList = new List(sizeof(NewToken));
	int countLines = 1;

	for (int i = 0; i < old->count(); i++)
	{
		automat::parseResult* oldToken = (automat::parseResult*)old->get_accessor(i);

		// ����� ��������, ��������� ����� � ������������.
		if (oldToken->token == automat::Token::Space ||
			oldToken->token == automat::Token::NewLine ||
			oldToken->token == automat::Token::SingleLineComment ||
			oldToken->token == automat::Token::MultilineComment)
		{
			if (oldToken->token == automat::NewLine) countLines++;
			continue;
		}

		if (oldToken->token == automat::ReservedWord)
		{
			if (strcmp(oldToken->string, "true") == 0)
			{
				oldToken->token = automat::Digit;
				oldToken->string = "1";
			}
			if (strcmp(oldToken->string, "false") == 0)
			{
				oldToken->token = automat::Digit;
				oldToken->string = "0";
			}
		}

		// ���� �����, �� ��������� ��� ��������, ����� 0.
		double value = 0;

		if (oldToken->token == automat::Token::Digit)
		{
			value = StrToDouble(oldToken->string);
		}

		if (oldToken->token == automat::Token::SysFunction)
		{
			// ��� ��������� �������, ������������ �������� (����, ����� input � output),
			// �������� value = 1.
			if (strcmp(oldToken->string, "output") && strcmp(oldToken->string, "input"))
			{
				value = 1;
			}
		}


		// ���������� ��� �������� ������������� ����� ������� Prioritize(List* tokenList).
		NewToken* newToken = new NewToken(oldToken->token, oldToken->string, value, 0, countLines);
		newTokenList->add(newToken);
	}

	return newTokenList;
}
double TreeWork::StrToDouble(char* s)
{
	return IsFraction(s) ? atof(s) : strtol(s, nullptr, 0);
}
// ���������� true, ���� ����� �������� �������, � false � ��������� ������.
bool TreeWork::IsFraction(char* s)
{
	bool e = false;
	bool x = false;

	for (int i = 0; i < strlen(s); i++)
	{
		if (s[i] == '.') return true;
		if (s[i] == 'e' || s[i] == 'E') e = true;
		if (s[i] == 'x' || s[i] == 'X') x = true;
	}

	if (e && !x) return true;
	return false;
}
// ���������� true, ���� token �������� ��������� � false � ��������� ������.
bool TreeWork::isOperator(automat::Token token)
{
	switch (token)
	{
	case automat::Token::ApOp:
	case automat::Token::Assignment:
	case automat::Token::BiteOp:
	case automat::Token::CompOper:
	case automat::Token::Condition:
	case automat::Token::IncOrDec:
	case automat::Token::LogicOper:
	case automat::Token::ReservedWord:
	case automat::Token::Separator:
	case automat::Token::Star:
	case automat::Token::SysFunction:
		return true;
	default:
		return false;
	}
}
// ����� ������ ������, ���������� ������ ��������� ��������.
void TreeWork::PrintList(List* tokens)
{
	char* stringToken[] = { "String", "SingleLineComment", "MultilineComment",
		"Digit", "Char", "Bracket", "ReservedWord", "Star", "CompOper", "LogicOper", "BiteOp",
		"Assignment", "SysFunction", "Separator", "UserType", "Space", "NewLine", "ReservedType",
		"Condition", "ApOp", "IncOrDec" };

	std::cout << "Type\t\tValue\t\tPriority\n";
	for (int i = 0; i < tokens->count(); i++)
	{
		NewToken* token = (NewToken*)tokens->get_accessor(i);

		std::cout << stringToken[token->type] << " ";
		std::cout << token->string << " ";
		std::cout << token->priority << " ";
		std::cout << token->value << " ";
		std::cout << token->lineIndex << std::endl;
	}
}