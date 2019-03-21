#include "hdfinterfaces.h"

using namespace HDF5Wrapper;

class Stream : public IHDFStream
{
private:
	Group* group;
	DataSpace* dataspace;
	DataSet* dataset;
	enHDFTtypes type;
	const char *name;
	hsize_t pointer;					// Указатель на позицию с которой нужно писать в файл
private:
	int getDataSetInfo(DataSet *dataset, hsize_t *size);	// Возвращает 1, если датасет типа int. Возвращает 0, если датасет типа char. Кладет в size количество ячеек в датасете
	void* readData(DataSet *dataset, hsize_t *writedDataSize);	// Читает данные из датасета
	void writeData(void *data, DataSet *dataset, enHDFTtypes hdfType);											// Пишет данные в датасет
	DataSet * createDataSet(const char *datasetName, Group *group, enHDFTtypes hdfType, DataSpace *dataspace);	// Создает датасет
	DataSet * readDataSet(const char* name, Group *group);	// Читает датасет
	void initDataSet();			// Создает пустой датасет
	void * addArrayToArray(void *firstArray, void *secondArray, int firstArraySize, int secondArraySize);		// Складывает 2 массива *void
public:
	const char * GetName();
	enHDFTtypes GetType();
	long GetLength();
	bool Seek(long _offset);
	long Read(void * _dest, long _cnt); 
	void Write(void * _src, long _cnt); 
	Stream(const char * _name, enHDFTtypes _type, Group* _group, bool init);				// Флаг init ставится в единицу, если стрим нужно создать, а не открыть
	~Stream();
};

class Folder : public IHDFFolder
{
private:
	Group *group;														// На какую группу сылается Folder
	H5File *file;														// В каком класссе
private:
	Group * OpenGroup(const char * groupName);							// Открывает группу в файле и возвращает хэндл на нее
public:
	IHDFFolder *  GetFolder(const char * _name);
	IHDFFolder *  GetFolder(long _index);
	IHDFFolder *  GetParent();

	IHDFStream *  CreateStream(const char * _name, enHDFTtypes _type);
	IHDFStream *  GetStream(const char * _name);
	IHDFStream *  GetStream(long _index);

	long GetCountFolder();
	long GetCountStream();
	const char * GetName();

	Folder(H5File *file_);												// Конструктор принимает файл от Storage, а так же устонавливает корень
	Folder(H5File *file_, const char *_group);						// Конструктор принимает файл от Storage, а так же устонавливает нужную папку
	~Folder();
};
class Storage : public IHDFStorage
{
private:
	H5File *file;
public:
	IHDFFolder * Create(const char * _path);
	IHDFFolder * Open(const char * _path);
	~Storage();
};