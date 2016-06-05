#pragma once
#ifndef HASH_H
#define HASH_H
#include "../Headers/list.h"
#include "../Headers/hash.h"

class Hash
{
public:
	Hash(int _n1, int _n2, int _n3, int _n4, int _n5);// нки -- кол-во значений, который может принять каждый ключ
	~Hash();
	
	List* find_list(char* key_word);
protected:
	virtual int key1(char* key_word) { return 0; };//способы получения ключей 
	virtual int key2(char* key_word) { return 0; };
	virtual int key3(char* key_word) { return 0; };
	virtual int key4(char* key_word) { return 0; };
	virtual int key5(char* key_word) { return 0; };
        int combine_keys(int k1, int k2, int k3, int k4, int k5)// уникальность значений ключа, хэш-функция(получение и ключа и значения)
		{  
			return 	k5+
				k4*n4+
				k3*n3*n4+
				k2*n2*n3*n4+
				k1*n1*n2*n3*n4;
		}
	List** table;
	int    n1, n2, n3, n4, n5;
	int table_count;
	int get_n1() { return n1; };
	int get_n2() { return n2; };
	int get_table_count(){ return table_count; }
};

struct Article
{
	char* word;
	char* description;
};

class Diction_list : public List
{
 public:
	Diction_list():List(sizeof(Article)){}; //такой же лист, элементы размера артикл
	~Diction_list(); // purge all Article data

	void     put(Article* article); // allocate memory for word & description
	Article* find(char* word);
	void     del(Article* article) { del(article->word); };
	void     del(char* word);
};

class Diction : public Hash
{
public: 
	Diction();
	~Diction();
	
	int key1(char* key_word) 
	{ 
		int f = key_word[0];
		return f > 0 ? f%get_n1() : 0;
	}
	int key2(char* key_word)
	{
		int f = key_word[1];
		return f > 0 ? f%get_n2() : 0;
	}
	Article* find (char* word);
	Article* auto_create(char* word);
};
#endif