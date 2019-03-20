#include "hdfinterfaces.h"

using namespace HDF5Wrapper;

class Stream : public IHDFStream
{
public:
	const char * GetName();
	enHDFTtypes GetType();
	long GetLength();
	bool Seek(long _offset);
	long Read(void * _dest, long _cnt); 
	void Write(void * _src, long _cnt); 
};

class Folder : public IHDFFolder
{
private:
	Group *group;								// На какую группу сылается Folder
	H5File *file;								// В каком класссе
private:
	Group * OpenGroup(const char * groupName);	// Открывает группу в файле и возвращает хэндл на нее
	//herr_t file_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata);
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

	Folder(H5File *file_);													// Конструктор принимает файл от Storage, а так же устонавливает корень
	Folder(H5File *file_, const char *groupName);							// Конструктор принимает файл от Storage, а так же устонавливает нужную папку
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