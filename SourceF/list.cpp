#include "stdafx.h"
#include <cstring>
#include "../Headers/heap.h"
#include "../Headers/list.h"

static Heap heap;

List::List(int _element_size, int _element_count)
{
	first = 0;
	last = 0;
	first_index = last_index = 0;
	element_size = _element_size;
	element_count = _element_count;
	_error = false;
	this->segment_�ount = 0;
}
List::~List()
{
	Segment* current;
	Segment* next;
	current = first;
	while (current)
	{
		next = current->next;
		delete_segment(current);
		current = next;
	}
	this->segment_�ount = 0;

}

void* List::get(int pos)
{
	void* ourEl;
	if (count() < pos || pos < 0)
	{
		_error = true;
		return 0;
	}


	if (pos + first_index < element_count) // ���� � ������ ��������
		ourEl = (void*)((size_t)(first->data) + (first_index + pos)*element_size);
	else
	{ // ���� �� � ������ �������� 
		struct Segment* ourSegm = first->next;
		pos = pos - (element_count - first_index);
		// ��� ���� ������ �������� � ������� �������� ourSegm
		while (pos >= element_count)
		{
			ourSegm = ourSegm->next;
			pos -= element_count;
		}
		ourEl = (void*)((size_t)(ourSegm->data) + pos*element_size);
	}
	return ourEl;
}

char* List::add(void* data)
{
	_error = false;
	if (first == 0 ||
		(last_index % element_count == 0 && last->next == 0))
	{ // ���� ��������� ���, ��� ����� ������� �� ���������� � ��������� �������
		new_segment(); // new_segment() ��������������� last
		memmove(last->data, data, element_size);
		char* offset = (char *)last->data;
		last_index++;
		return offset;
	}
	else
	{ // ���� ����� ������� ���������� � ������������ ������� last
		int segment_index = last_index % element_count;
		memmove((void*)((size_t)last->data + segment_index*element_size), data, element_size);
		char* offset = (char *)((char *)last->data + segment_index*element_size);
		last_index++;
		return offset;
	}
	
}

void List::take_first(void* store)
{
	// ���� ��������� ���, �� ����� ������
	if (count() < 1)
	{
		_error = true;
		return;
	}
	_error = false;
	// ������� �������� ������� � store
	memcpy(store, (void*)((size_t)first->data + first_index*element_size), element_size);
	// ����������� ������ ������� ��������
	first_index++;

	if (first_index == element_count)
	{ // ���� ������ ������� ����� �� ������� ������� ��������
		Segment* new_first = first->next;
		// ������� ������ ������ �������
		delete_segment(first);
		// ����������� ������ ������� �������� �� ����� ������
		first = new_first;
		// �������� ������ � ��������� �������
		first_index -= element_count;
		last_index -= element_count;
	}
}

void List::take_last(void* store)
{
	// ���� ��������� ���, �� ����� ������
	if (count() < 1)
	{
		_error = true;
		return;
	}
	_error = false;
	// ������� ������� ������ �������� ������ ������ ���������� ��������
	// ��� ��� last_index ��������� �� ������ ��������, ���������� �� ���������, ���� -1
	int segment_index = (last_index - 1) % element_count;
	// �������� ������� � ���� �������� � store
	memcpy(store, (void*)((size_t)last->data + segment_index*element_size), element_size);
	// ��������� ������ ���������� ��������
	last_index--;
	//int n = *((int*)store); // �����
	if (segment_index == 0)
	{ // ���� ������ �� ��������� ��������� ���� � ������ ��������
	  // �� ���� ������� ���� � ��� ���� �������
		// ������� ������ ��������� ������� - delete_segment() ��� ��������������� ������
		delete_segment(last);
	}
}

void List::take(int pos, void* store)
{
	// �� ������ ������ ���������� ������, ��� pos ��������� �� ������ ��� ��������� ��������
	// ����� �������� ��������������� "�������" take_****
	if (pos == 0)
	{
		take_first(store);
		return;
	}
	else if (pos == count()-1)
	{
		take_last(store);
		return;
	}
	
	// ������� �������� ������ ������� � store
	// ����� �������� �������� � ������� this->get()
	memcpy(store, get(pos), element_size);

	// ��� ��� �� ����������� � ���, ��� take ����� ���������, �� ����� ���������� ��� ������������
	// ������� �� ������ �������� ����� ���� � ������� memcpy
	// ��� ��������� �� �������� ���� ����� �������������� ���������
	for (int i = pos + 1; i < last_index; i++)
	{
		memcpy(get(i-1), get(i), element_size);
	}

	last_index--;

	// ����� �� - ���� ��������� ������� �������� ����, �� ������� ���

	if (last_index % element_count == 0)
	{ // ���� ������ �� ��������� ��������� ���� � ������ ��������
		// �� ���� ������� ���� � ��� ���� �������
		// ������� ������ ��������� ������� - delete_segment() ��� ��������������� ������
		delete_segment(last);
	}
}

void List::sort(bool dir, int (*method)(const void*, const void*))
{ // �������, ��� dir == true ��� ������������ ������� ����������
	if (method == 0)
		return;
	int count = this->count();
	void* buffer = heap.get_mem(element_size); // ������ ��� ������� ��� ������������
	for (int i = 0; i < count - 1; i++)
	{
		void* starting = get(i);
		void* max_or_min = starting;
		for (int j = i+1; j < count; j++)
		{
			// method ������ ���� �������� ���������, ����������� ��������� �� ��� ��������
			// ���������� 1 ���� ������ ������� ������ �������, 0 ���� ��� �����, -1 ���� ������ ������ �������
			void* current = get(j);
			//int n = *((int*)current); //�����
			int comparison_result = method(max_or_min, current);
			if (dir)
			{
				// ��� ��� dir == true ��� ������������� �������, ��� �������� ����������
				// �������� ��������� �� -1 - ��� ����� ����������� �������� ���� ���������� � ������ method
				comparison_result *= -1;
			}
			// ���� ������������ ������� ������ ��������
			// (���� ����������� ������� ������ ��������)
			// --- � ����������� �� dir ---
			// �� ��������������� ��������� ������������� (������������) ��������
			if (comparison_result < 0)
			{
				max_or_min = current;
			}
		}
		// ������ �� ����� ����� ������������� (��� ������������) ��������
		// ���� ���� ������� - �� "�������", �.�. �� i-��, �� ������ �� �������
		if (starting != max_or_min)
		{
			memcpy(buffer, starting, element_size);
			memcpy(starting, max_or_min, element_size);
			memcpy(max_or_min, buffer, element_size);
		}
	}
}

int List::count()
{
	_error = false;
	return last_index - first_index;
}

void List::new_segment()
{
	// �������� ������ ��� ���� ��������� �������� (�.�. ��� ��� ���������: data, next, prev)
	Segment* new_segment = (Segment*)heap.get_mem(sizeof(Segment));
	// ����� �������� ������ ��� LISTSIZE ��������� � ����������� ��������� �� ��� ������� � data
	new_segment->data = heap.get_mem(element_count*element_size);
	if (first == 0)
	{ // ���� ��� ������ �������, �� �� �� � ���������
		first = last = new_segment;
		new_segment->next = new_segment->prev = 0;
		first_index = last_index = 0;
	}
	else
	{ // ���� �� ������, �� �� ����� ���������
		new_segment->prev = last;
		last->next = new_segment;
		new_segment->next = 0;
		last = new_segment;
	}
	segment_�ount++;
}


List::Segment* List::get_segment(int id)
{
	Segment* i = first;
	int n = 0;

	if (id > segment_�ount || id < 0) return nullptr;

	while (n != id)
	{
		i = i->next;
		n++;
	}
	return i;
}

void List::delete_segment(Segment* seg)
{
	// ���� ������� ������, �� ������ ������� - �� ������
	if (seg == 0)
		return;

	// ������� ������ ������� �� ������� (������)
	// ���������� �� ����������?
	if (seg->prev == 0)
	{ 
		// ���� ������ ������� - ������, �� ������ ������� ��������� ������� ��������
		first = seg->next;
	}
	else
	{
		// ����� ��������� ������ next ����������� �������� �� ��������� �� ������
		seg->prev->next = seg->next;
	}

	// ���������� �� ���������?
	if (seg->next == 0)
	{ 
		// ���� ������ ������� - ���������, �� ��������������� ��������� �������
		last = seg->prev;
	}
	else
	{
		// ����� ��������� ������ prev ���������� �������� �� ���������� ����� ������
		seg->next->prev = seg->prev;
	}
	// � ����������� ������ - ������� ������ ��������, ����� ���� ��������� � �����������
	heap.free_mem(seg->data);
	heap.free_mem(seg);
	segment_�ount--;
}

void List::copy_element(void* destination, void* source)
{
	for (int i = 0; i < element_size; i++)
		*((char*)destination + i) = *((char*)source + i);
}

void* List::get_accessor(int n)
{
	n += first_index;

	
	Segment* current_segment = get_segment(n / element_count);
	int element_index = n % element_count;

	char* source = (char*)current_segment->data + element_index*element_size;
	return source;
}

void List::set_accessor(void* data, int n)
{
	n += first_index;
	Segment* currentSegment = get_segment(n / element_count);
	int elementIndex = n % element_count;

	char* source = (char*)currentSegment->data + elementIndex*element_size;
	copy_element(source, data);
	
}

