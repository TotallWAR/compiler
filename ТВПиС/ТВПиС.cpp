#include "stdafx.h"
#include "Defaults.h"
#include "ASTBuilder.h"
#include "Exception.h"

#include <ctime>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(nullptr));

	try
	{
		ASTBuilder* builder = new ASTBuilder("1.cpp");
		builder->Build();
		builder->Run();
	}
	catch (Exception* exception)
	{
		setlocale(LC_ALL, "Russian");
		cout << exception->GetMessage() << " Line: " << exception->GetLine() << "." << endl;
	}

	return 0;
}