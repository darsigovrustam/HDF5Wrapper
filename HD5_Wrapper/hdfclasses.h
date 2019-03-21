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
	hsize_t pointer;					// ��������� �� ������� � ������� ����� ������ � ����
private:
	int getDataSetInfo(DataSet *dataset, hsize_t *size);	// ���������� 1, ���� ������� ���� int. ���������� 0, ���� ������� ���� char. ������ � size ���������� ����� � ��������
	void* readData(DataSet *dataset, hsize_t *writedDataSize);	// ������ ������ �� ��������
	void writeData(void *data, DataSet *dataset, enHDFTtypes hdfType);											// ����� ������ � �������
	DataSet * createDataSet(const char *datasetName, Group *group, enHDFTtypes hdfType, DataSpace *dataspace);	// ������� �������
	DataSet * readDataSet(const char* name, Group *group);	// ������ �������
	void initDataSet();			// ������� ������ �������
	void * addArrayToArray(void *firstArray, void *secondArray, int firstArraySize, int secondArraySize);		// ���������� 2 ������� *void
public:
	const char * GetName();
	enHDFTtypes GetType();
	long GetLength();
	bool Seek(long _offset);
	long Read(void * _dest, long _cnt); 
	void Write(void * _src, long _cnt); 
	Stream(const char * _name, enHDFTtypes _type, Group* _group, bool init);				// ���� init �������� � �������, ���� ����� ����� �������, � �� �������
	~Stream();
};

class Folder : public IHDFFolder
{
private:
	Group *group;														// �� ����� ������ �������� Folder
	H5File *file;														// � ����� �������
private:
	Group * OpenGroup(const char * groupName);							// ��������� ������ � ����� � ���������� ����� �� ���
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

	Folder(H5File *file_);												// ����������� ��������� ���� �� Storage, � ��� �� ������������� ������
	Folder(H5File *file_, const char *_group);						// ����������� ��������� ���� �� Storage, � ��� �� ������������� ������ �����
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