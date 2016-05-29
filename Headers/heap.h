#ifndef LAB_HEAP_H
#define LAB_HEAP_H

#define SEGMENTSIZE 65539
#define SEGMENTCOUNT 1024

class Heap
{
public:
	static Heap& heap(int segmentSize = SEGMENTSIZE);
	Heap(int _segment_size = SEGMENTSIZE, int _clusterSize = sizeof(int))
	{
		segment_size = _segment_size;
		clusterSize = _clusterSize;
		current = 0;
	}
	~Heap()
	{
		delete_segments();
	}
	void*      get_mem(int size);
	void       free_mem(void*);
private:
	struct Segment_def//один дескриптор 
	{
		bool      used;//использован или нет
		int       size;//размер выделенной обсласти 
		size_t     offset;//смещение, чтобы найти этот участок
		//void* поменяли на size_t, потому что для положительного смещения этот тип гораздо удобнее, а отрицательного у нас нет
	};

	struct Segment//таблица, для каждого сегмента выделяется массив дескрипторов 
	{
		void*        data;//указатель на один сегмент 64кб
		Segment*     prev; // ссылка на предыдущий сегмент
		Segment_def  descriptor[SEGMENTCOUNT]; // указатель на массив дескрипторов
		int          descriptor_count; // кол-во дескрипторов (сколько выделено штук)
	};

	int       make_segment();//функция по создания сегмента
	void      delete_segments();//удаляет все сегменты
	int       segment_size;
	int	clusterSize;

		Segment*  current;
};

#endif