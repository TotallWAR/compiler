#include "stdafx.h"
#include <cstring>
#include "../Headers/heap.h"
#include "../Headers/hash.h"
#include "../Headers/list.h"
//Heap heap;
Hash::Hash(int _n1, int _n2, int _n3, int _n4, int _n5) //для чего эти n?
{
	n1 = _n1 >1 ? _n1 : 1;
	n2 = _n2 >1 ? _n2 : 1;
	n3 = _n3 >1 ? _n3 : 1;
	n4 = _n4 >1 ? _n4 : 1;
	n5 = _n5 >1 ? _n5 : 1;
	table_count = n1*n2*n3*n4*n5;
	table = (List**)Heap::heap().get_mem(table_count * sizeof(List*));//n1*n2*n3*n4*n5 -- кол-во комбинаций всех ключей
}
Hash::~Hash()
{
	Heap::heap().free_mem(table);
}
List* Hash::find_list(char* key_word)
{
	int k1, k2, k3, k4, k5;
	k1 = key1(key_word);
	k2 = key2(key_word);
	k3 = key3(key_word);
	k4 = key4(key_word);
	k5 = key5(key_word);
	int index = combine_keys(k1, k2, k3, k4, k5)%table_count;
	return table[index];

}
Diction_list::~Diction_list()
{
	Article* current = (Article*)Heap::heap().get_mem(sizeof(Article));
	while (true)
	{
		take_first(current);

		//если не смогли прочитать элемент добавить
		if (error() == true)
		{
			break;
		}
		Heap::heap().free_mem(current->word);
		Heap::heap().free_mem(current->description);
	}
	Heap::heap().free_mem(current);
}
void Diction_list::put(Article* article)
{
	Article* copy = (Article *)Heap::heap().get_mem(sizeof(Article));
	size_t word_length = strlen(article->word) + 1;
	size_t descr_length = strlen(article->description)+1;
	//память в артикле-копии
	copy->description = (char*)Heap::heap().get_mem(descr_length);
	copy->word = (char*)Heap::heap().get_mem(word_length);
	//копируем строки из article в copy
	memcpy(copy ->word,article -> word,word_length);
	memcpy(copy->description,article->description,descr_length);
	//добавим в лист
	add(copy);
	Heap::heap().free_mem(copy);
}
Article* Diction_list::find(char* word)
{
	for (int i = 0; i < count(); i++)
	{
		Article* current = (Article*)get(i);
		if (strcmp(word, current->word) == 0)
		{
			return current;
		}
	}
	return NULL;
}
void Diction_list::del(char* word)
{
	for (int i = 0; i < count(); i++)
	{
		Article* current = (Article*)get(i);
		if (strcmp(word, current->word) == 0)
		{
			Article buf;
			take(i, &buf);
			Heap::heap().free_mem(buf.word);
			Heap::heap().free_mem(buf.description);
			return;
		}
	}
}
Diction::Diction() : Hash(33, 33, 0, 0, 0)
{
	for (int i = 0; i < get_table_count(); i++)
	{
		table[i] = new Diction_list();
	}
}
Diction::~Diction()
{
	// через цикл удаляем все созданные таблицы
	for (int i = 0; i < get_table_count(); i++)
	{
		delete(table[i]);
	}
}
Article* Diction::find(char* word)
{
	// по хэшу определяем, в каком именно листе (возможно) лежит наша запись
	Diction_list* list = (Diction_list*)find_list(word);
	// потом в этом листе ищем её
	return list->find(word);
	
}
Article* Diction::auto_create(char* word)
{
	//определяем, в каком листе дложна быть наша запись
	Diction_list* list = (Diction_list*)find_list(word);
	Article* article = (Article*)Heap::heap().get_mem(sizeof(Article));
	char description[] = "Description"; //шаблон описания
	article->word = word;
	article->description = description;
	list->put(article); //добавляем артикль в список
	Heap::heap().free_mem(article);
	return (Article*)list->get(list->count() - 1);
}
