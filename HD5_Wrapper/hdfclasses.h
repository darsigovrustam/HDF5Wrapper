#include "hdfinterfaces.h"

using namespace HDF5Wrapper;

class Stream : public IHDFStream
{
private:
	Group * group;
	DataSpace *dataspace;
	DataSet *dataset;

	// Указатель на позицию с которой нужно писать в файл
	hsize_t pointer;	

	// Количество вообще записанных данных в данный стрим
	long writedDataCount;
	
	enHDFTtypes type;
	const char *name;

private:
	~Stream();

	// Возвращает 1, если датасет типа int. Возвращает 0,
	// если датасет типа char. Кладет в size количество ячеек в датасете
	int getDataSetInfo(DataSet *dataset, hsize_t *size);					
	
	// Читает данные из датасета
	void * readData(DataSet *dataset, hsize_t *writedDataSize);				
	
	// Пишет данные в датасет
	void writeData(void *data, DataSet *dataset, enHDFTtypes hdfType);		
	
	// Создает датасет
	DataSet * createDataSet(const char *datasetName, Group *group,
		enHDFTtypes hdfType, DataSpace *dataspace);	
	
	// Читает датасет из группы
	DataSet * readDataSet(const char* name, Group *group);	
	
	// Создает пустой датасет
	void initDataSet();														
	
	// Складывает 2 массива *void
	void * addArrayToArray(void *firstArray, void *secondArray,
		int firstArraySize, int secondArraySize);		

public:
	// Флаг init ставится в единицу, если стрим нужно создать, а не открыть
	Stream(const char *_name, enHDFTtypes _type, Group *_group, bool init);

	const char *	GetName();
	enHDFTtypes		GetType();
	long			GetLength();

	bool Seek		(long _offset);
	long Read		(void **_dest,	long _cnt);
	void Write		(void *_src,	long _cnt);
};

class Folder : public IHDFFolder
{
private:
	// На какую группу сылается Folder
	Group *group;										

	// В каком класссе
	H5File *file;														

private:
	~Folder();

	// Открывает группу в файле и возвращает хэндл на нее. 
	// Если такой группы нет, то создает
	Group * OpenGroup(const char *groupName);							

public:
	// Конструктор принимает файл от Storage,
	// а так же устонавливает корень
	Folder(H5File *file_);					

	// Конструктор принимает файл от Storage,
	// а так же устонавливает нужную папку
	Folder(H5File *file_, const char *_group);							
	
	IHDFFolder * GetFolder(const char *_name);
	IHDFFolder * GetFolder(long _index);
	IHDFFolder * GetParent();

	IHDFStream * GetStream(const char *_name);
	IHDFStream * GetStream(long _index);
	IHDFStream * CreateStream(const char *_name, enHDFTtypes _type);

	long GetCountFolder();
	long GetCountStream();
	const char * GetName();
};

class Storage : public IHDFStorage
{
private:
	H5File *file;

private:
	~Storage();

public:
	IHDFFolder * Create(const char * _path);
	IHDFFolder * Open(const char * _path);
};