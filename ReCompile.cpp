// ReCompile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Headers/hash.h"
#include "Headers/heap.h"
#include "Headers/list.h"
#include "Headers/list_adv.h"
#include <iostream>
#include <string>
static Heap heap;

int compareMethod(const void* e1, const void* e2)
{
	int* n1 = (int*)e1;
	int* n2 = (int*)e2;
	return *n1 - *n2;
}
int _tmain(int argc, _TCHAR* argv[])
{
	
//	List list(sizeof(int));
//	int a;
//	int limit = 22;
//	for (int i = 0; i < limit; i++)
//	{
//		a = i;
//		list.add(&a);
//	}
//
//	list.sort(false, compareMethod);
//
//	for (int j = 0; j < limit; j++)
//	{
//		list.take_first(&a);
//		std::cout << j << "..." << a << std::endl;
//	}
	
	setlocale(LC_ALL, "Russian");
	Diction diction;
	Article* record;
	record = diction.auto_create("Baba-Yaga");
	record = diction.auto_create("Pink Elephant");

	record = diction.find("Pink Elephant");

	if (record)
	{
		std::cout << "Word: " << record->word << std::endl;
		std::cout << "Description: " << record->description << std::endl;
	}
	else
	{
		std::cout << "Record not found" << std::endl;
	}
	return 0; 
}

