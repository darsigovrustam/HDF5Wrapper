#include "hdfinterfaces.h"

using namespace HDF5Wrapper;

class Stream : public IHDFStream
{
private:
	Group * group;
	DataSpace *dataspace;
	DataSet *dataset;

	// ��������� �� ������� � ������� ����� ������ � ����
	hsize_t pointer;	

	// ���������� ������ ���������� ������ � ������ �����
	long writedDataCount;
	
	enHDFTtypes type;
	const char *name;

private:
	~Stream();

	// ���������� 1, ���� ������� ���� int. ���������� 0,
	// ���� ������� ���� char. ������ � size ���������� ����� � ��������
	int getDataSetInfo(DataSet *dataset, hsize_t *size);					
	
	// ������ ������ �� ��������
	void * readData(DataSet *dataset, hsize_t *writedDataSize);				
	
	// ����� ������ � �������
	void writeData(void *data, DataSet *dataset, enHDFTtypes hdfType);		
	
	// ������� �������
	DataSet * createDataSet(const char *datasetName, Group *group,
		enHDFTtypes hdfType, DataSpace *dataspace);	
	
	// ������ ������� �� ������
	DataSet * readDataSet(const char* name, Group *group);	
	
	// ������� ������ �������
	void initDataSet();														
	
	// ���������� 2 ������� *void
	void * addArrayToArray(void *firstArray, void *secondArray,
		int firstArraySize, int secondArraySize);		

public:
	// ���� init �������� � �������, ���� ����� ����� �������, � �� �������
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
	// �� ����� ������ �������� Folder
	Group *group;										

	// � ����� �������
	H5File *file;														

private:
	~Folder();

	// ��������� ������ � ����� � ���������� ����� �� ���. 
	// ���� ����� ������ ���, �� �������
	Group * OpenGroup(const char *groupName);							

public:
	// ����������� ��������� ���� �� Storage,
	// � ��� �� ������������� ������
	Folder(H5File *file_);					

	// ����������� ��������� ���� �� Storage,
	// � ��� �� ������������� ������ �����
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