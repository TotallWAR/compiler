#include "stdafx.h"
#include "stdlib.h"
#include <string.h>
#include "../Headers/heap.h"

Heap& Heap::heap(int segmentSize)
{
	static Heap heap(segmentSize);
	return heap;
}

void* Heap::get_mem(int size)
{
	size_t memoryAllocate = size/clusterSize; //������� ��������� �� ������� ��� ����������� �������
	if (size%clusterSize)
		memoryAllocate++;
	memoryAllocate *= clusterSize; //������������� � �����
	Segment* activeSegment = current;
	Segment_def* serveciRecord;
	size_t segmentStart;
	for (activeSegment = current; activeSegment; activeSegment = activeSegment->prev)
	{
		serveciRecord = &(activeSegment->descriptor[activeSegment->descriptor_count]);
		segmentStart = (size_t)activeSegment->data;

		// �� ������ ����� �� �����, ��� � ��� ���� ����� ��� ������� ��� ���� ����� ����������
		//if (activeSegment->descriptor[activeSegment->descriptor_count].size >= region_count)
		if (serveciRecord->size >= memoryAllocate && activeSegment->descriptor_count < SEGMENTCOUNT)
		{ // ���� ������� � �����

			serveciRecord->size -= memoryAllocate;
			serveciRecord->offset += memoryAllocate;
			Segment_def* newServiceRecord = &(activeSegment->descriptor[activeSegment->descriptor_count+1]);;

			memmove(newServiceRecord, // �������� �� ���� ����������� ������ 
				serveciRecord,
				sizeof(Segment_def));

			// ��� ��� service_record �� ��� ��������� �� ������� ������, �� ������ ������ ��, ��� �������� �� ����� ������
			serveciRecord->size = memoryAllocate;
			serveciRecord->used = true;
			// � ����� ��������� ������ ���� ������ �������� � ������
			newServiceRecord->size -= memoryAllocate;
			newServiceRecord->offset += memoryAllocate;

			activeSegment->descriptor_count++;

			return (void*)(segmentStart + serveciRecord->offset);
		}
		else // ���� ���� �� ������ � �����
		{
			// �������� �� ������� ������������ � ���� �������� �������, ���������� �� �������
			for (int i = 0; i < activeSegment->descriptor_count; i++)
			{
				// ������� �������� ��������� �����
				if (activeSegment->descriptor[i].used)
				{
					continue;
				}
				// ������ �� ����� �����, ��� ������� ����� �� ������������
				// ������ �������� ������ �����
				if (activeSegment->descriptor[i].size < memoryAllocate)
				{
					continue;
				}
				// ������ �� �����, ��� � ��� ���� ��������� ����� ������������ �������
				// ����� � ���� ����������
				if (activeSegment->descriptor_count == SEGMENTCOUNT || activeSegment->descriptor[i].size == memoryAllocate) 
				{ // ���� ��� ����� ��� ����� ����������� � �������, �� ����� ���� ����� ��� ����������� �� �������
					// ���� ������ ����� ����� ��������������, �� ����� ���� ���� ����� ����
					activeSegment->descriptor[i].used = true;
					// ���������� ���������� ����� ����� ����� (����� �������� + �������� ������������ ������ ��������)
					return (void*)(segmentStart + activeSegment->descriptor[i].offset);
				}
				else
				{
					// ���� ����� ����, �� ����� ��� ���������� �� ��� �����
					int moveCount = activeSegment->descriptor_count - i + 1;
					size_t moveSize = moveCount * sizeof(Segment_def);
					memmove(&(activeSegment->descriptor[i+1]), &(activeSegment->descriptor[i]), moveSize);

					// ����������� ����������, ���������� �������� �����
					activeSegment->descriptor[i].size = memoryAllocate;
					activeSegment->descriptor[i].used = true;

					// ����������� ����������, ���������� ����������� ���������� �����
					activeSegment->descriptor[i+1].size -= memoryAllocate;
					activeSegment->descriptor[i+1].offset += memoryAllocate;

					// ����������� ���������� ��������������� ������������
					activeSegment->descriptor_count++;

					// ���������� ����� ������ �������� �����
					return (void*)(segmentStart + activeSegment->descriptor[i].offset);
				}
			}
		}
	}
	// �������� ������ ��������, ���������� � ����
	make_segment();

	// ��-��������, ��� ����� ������� � �������, �� ����������� �� ��������� header-a - ��������

	// ��������������� ���������
	serveciRecord = &current->descriptor[0];
	activeSegment = current;
	segmentStart = (size_t)activeSegment->data;

	// ���� ����� ����� ������� � �������
	Segment_def* new_service_record = &current->descriptor[1];
	size_t bytes_to_move = sizeof(Segment_def);
	memmove(new_service_record, // �������� �� ���� ����������� ������
		serveciRecord,
		bytes_to_move);

	// ��� ��� service_record �� ��� ��������� �� ������� ������, �� ������ ������ ��, ��� �������� �� ����� ������
	serveciRecord->size = memoryAllocate;
	serveciRecord->used = true;
	// � ����� ��������� ������ ���� ������ �������� � ������
	new_service_record->size -= memoryAllocate;
	new_service_record->offset += memoryAllocate;

	activeSegment->descriptor_count++;

	return (void*)(segmentStart + serveciRecord->offset);
}

void Heap::free_mem(void* address_void)
{
	// ������� ������ ������� ������������
	int shiftCount = 0;
	size_t address = (size_t)address_void;

	Segment* activeSegment = current;
	size_t segmentStart;

	// ������� ���� ������ �������
	while (true)
	{
		segmentStart = (size_t)activeSegment->data;
		if (address >= segmentStart && address < segmentStart + SEGMENTSIZE)
			break;
		activeSegment = activeSegment->prev;
	}
	
	// ����� ������ �������, ������ �������� ������ ���� � �������������
	// ������� ��� �������� ������� �������� ������������ ������ ����� �������� � ��������� ����������
	size_t offset = address - segmentStart;

	int i = 0;
	while (offset < activeSegment->descriptor[i].offset)
	{
		i++;
	}

	// ���� ����� ������ �� ������, �� ������ offset � active_segment->descriptor[i].offset ����� ����� �����
	// ���� ���������, ��� ������������ - �����, ������� ��������� ���������, � �� �� ����� ���������
	if (offset != activeSegment->descriptor[i].offset)
		return;

	bool isNextFree = false;
	if (activeSegment->descriptor[i+1].used == false)
	{
		isNextFree = true;
		shiftCount++;
		activeSegment->descriptor[i].size += activeSegment->descriptor[i + 1].size;
	}
	if (i > 0 && activeSegment->descriptor[i - 1].used == false)
	{
		shiftCount++;
		activeSegment->descriptor[i - 1].size += activeSegment->descriptor[i].size;
	}

	// ������ � ����� ������� �� 1/2/3 ��������� ������������ ���������� ����� ������ ���������� �����
	// �������� �������� ������ ����� ������� ������������ �� ���������� �������, ������������ � shift_count - �������� ������

	// ���� shift_count == 0, �� ������ �� ������ � ������������ �� �������
	if (shiftCount == 0)
		return;
	
	// ������ �� �����, ��� ����� ����� �����

	int moveCount;
	size_t moveSize;
	if (isNextFree)
	{ // ���� ��������� ���������� ��������, �� ����� ����� ������� � ������� i+2
		moveCount = activeSegment->descriptor_count - i - 1;
		moveSize = moveCount * sizeof(Segment_def);
		memmove(&activeSegment->descriptor[i + 2 - shiftCount],
			&activeSegment->descriptor[i + 2],
			moveSize);
	}
	else // ���� �� ��������� ���������� �����, �� ����� ����� ������� � ������� i+1
	{ // ��������������, ���������� ��������� ����� �� 1 ������, ��� � ���������� ������
		// � ���� ����� shift_count ����� ����� 1
		moveCount = activeSegment->descriptor_count - i;
		moveSize = moveCount * sizeof(Segment_def);
		memmove(&activeSegment->descriptor[i + 1 - shiftCount],
			&activeSegment->descriptor[i + 1],
			moveSize);
	}
	activeSegment->descriptor_count -= shiftCount;
}

int Heap::make_segment()
{
	struct Segment* s = new Segment;
	s->data = malloc(segment_size);
	if (s == NULL || s->data == NULL)
		throw "heap exception";
	s->prev = current;
	current = s;
	s->descriptor_count = 0;
	s->descriptor[0].offset = 0;
	s->descriptor[0].used = false;
	s->descriptor[0].size = SEGMENTSIZE;
	return 0;
}

void Heap::delete_segments()
{
	while (current)
	{
		Segment* next = current->prev;
		free(current->data);
		free(current);
		current = next;
	}
}
