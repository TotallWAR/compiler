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
	size_t memoryAllocate = size/clusterSize; //сколько кластеров мы выделим для запрошенной области
	if (size%clusterSize)
		memoryAllocate++;
	memoryAllocate *= clusterSize; //преобразовали в байты
	Segment* activeSegment = current;
	Segment_def* serveciRecord;
	size_t segmentStart;
	for (activeSegment = current; activeSegment; activeSegment = activeSegment->prev)
	{
		serveciRecord = &(activeSegment->descriptor[activeSegment->descriptor_count]);
		segmentStart = (size_t)activeSegment->data;

		// на данном этапе мы знаем, что у нас есть место как минимум под один новый дескриптор
		//if (activeSegment->descriptor[activeSegment->descriptor_count].size >= region_count)
		if (serveciRecord->size >= memoryAllocate && activeSegment->descriptor_count < SEGMENTCOUNT)
		{ // если влезает в конец

			serveciRecord->size -= memoryAllocate;
			serveciRecord->offset += memoryAllocate;
			Segment_def* newServiceRecord = &(activeSegment->descriptor[activeSegment->descriptor_count+1]);;

			memmove(newServiceRecord, // сместить на один собственный размер 
				serveciRecord,
				sizeof(Segment_def));

			// так как service_record всё ещё указывает на нулевую запись, мы просто меняем то, что осталось по этому адресу
			serveciRecord->size = memoryAllocate;
			serveciRecord->used = true;
			// у новой сервисной записи тоже меняем смещение и размер
			newServiceRecord->size -= memoryAllocate;
			newServiceRecord->offset += memoryAllocate;

			activeSegment->descriptor_count++;

			return (void*)(segmentStart + serveciRecord->offset);
		}
		else // таки если не влезло в конец
		{
			// проходим по массиву дескрипторов и ищем свободну область, подходящую по размеру
			for (int i = 0; i < activeSegment->descriptor_count; i++)
			{
				// сначала проверим занятость куска
				if (activeSegment->descriptor[i].used)
				{
					continue;
				}
				// теперь мы точно знаем, что текущий кусок не используется
				// теперь проверим размер куска
				if (activeSegment->descriptor[i].size < memoryAllocate)
				{
					continue;
				}
				// теперь мы знаем, что у нас есть свободный кусок достаточного размера
				// хотим в него воткнуться
				if (activeSegment->descriptor_count == SEGMENTCOUNT || activeSegment->descriptor[i].size == memoryAllocate) 
				{ // если нет места под новые дескрипторы в массиве, то отдаём весь кусок вне зависимости от размера
					// если размер куска равен запрашиваемому, то ясное дело тоже отдаём весь
					activeSegment->descriptor[i].used = true;
					// возвращаем абсолютный адрес этого куска (адрес сегмента + смещение относительно начала сегмента)
					return (void*)(segmentStart + activeSegment->descriptor[i].offset);
				}
				else
				{
					// если место есть, то делим наш дескриптор на две части
					int moveCount = activeSegment->descriptor_count - i + 1;
					size_t moveSize = moveCount * sizeof(Segment_def);
					memmove(&(activeSegment->descriptor[i+1]), &(activeSegment->descriptor[i]), moveSize);

					// настраиваем дескриптор, отвечающий занятому куску
					activeSegment->descriptor[i].size = memoryAllocate;
					activeSegment->descriptor[i].used = true;

					// настраиваем дескриптор, отвечающий оставшемуся свободному куску
					activeSegment->descriptor[i+1].size -= memoryAllocate;
					activeSegment->descriptor[i+1].offset += memoryAllocate;

					// увеличиваем количество задействованных дескрипторов
					activeSegment->descriptor_count++;

					// возвращаем адрес нового занятого куска
					return (void*)(segmentStart + activeSegment->descriptor[i].offset);
				}
			}
		}
	}
	// создание нового сегмента, добавление в него
	make_segment();

	// по-хорошему, это стоит вынести в функцию, но ограничения на изменения header-a - СПРОСИТЬ

	// переприсваиваем указатели
	serveciRecord = &current->descriptor[0];
	activeSegment = current;
	segmentStart = (size_t)activeSegment->data;

	// этот кусок можно вынести в функцию
	Segment_def* new_service_record = &current->descriptor[1];
	size_t bytes_to_move = sizeof(Segment_def);
	memmove(new_service_record, // сместить на один собственный размер
		serveciRecord,
		bytes_to_move);

	// так как service_record всё ещё указывает на нулевую запись, мы просто меняем то, что осталось по этому адресу
	serveciRecord->size = memoryAllocate;
	serveciRecord->used = true;
	// у новой сервисной записи тоже меняем смещение и размер
	new_service_record->size -= memoryAllocate;
	new_service_record->offset += memoryAllocate;

	activeSegment->descriptor_count++;

	return (void*)(segmentStart + serveciRecord->offset);
}

void Heap::free_mem(void* address_void)
{
	// счётчик сдвига таблицы дескрипторов
	int shiftCount = 0;
	size_t address = (size_t)address_void;

	Segment* activeSegment = current;
	size_t segmentStart;

	// сначала ищем нужный сегмент
	while (true)
	{
		segmentStart = (size_t)activeSegment->data;
		if (address >= segmentStart && address < segmentStart + SEGMENTSIZE)
			break;
		activeSegment = activeSegment->prev;
	}
	
	// нашли нужный сегмент, теперь работаем внутри него с дескрипторами
	// сначала для удобства вынесем смещение относительно начала этого сегмента в отдельную переменную
	size_t offset = address - segmentStart;

	int i = 0;
	while (offset < activeSegment->descriptor[i].offset)
	{
		i++;
	}

	// если никто ничего не сломал, то сейчас offset и active_segment->descriptor[i].offset равны между собой
	// надо учитывать, что пользователь - дебил, поэтому проверяем равенство, а то всё может сломаться
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

	// теперь в самом верхнем из 1/2/3 свободных дескрипторов содержится общий размер свободного куска
	// осталось сместить нижнюю часть массива дескрипторов на количество записей, содержащееся в shift_count - счётчике сдвига

	// если shift_count == 0, то ничего не делаем и возвращаемся из функции
	if (shiftCount == 0)
		return;
	
	// теперь мы знаем, что сдвиг точно будет

	int moveCount;
	size_t moveSize;
	if (isNextFree)
	{ // если следующий дескриптор свободен, то сдвиг будет начиная с индекса i+2
		moveCount = activeSegment->descriptor_count - i - 1;
		moveSize = moveCount * sizeof(Segment_def);
		memmove(&activeSegment->descriptor[i + 2 - shiftCount],
			&activeSegment->descriptor[i + 2],
			moveSize);
	}
	else // если же следующий дескриптор занят, то сдвиг будет начиная с индекса i+1
	{ // соответственно, сдвигаемых элементов будет на 1 больше, чем в предыдущем случае
		// в этом блоке shift_count будет равен 1
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
