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
	Group *group;								// �� ����� ������ �������� Folder
	H5File *file;								// � ����� �������
private:
	Group * OpenGroup(const char * groupName);	// ��������� ������ � ����� � ���������� ����� �� ���
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

	Folder(H5File *file_);													// ����������� ��������� ���� �� Storage, � ��� �� ������������� ������
	Folder(H5File *file_, const char *groupName);							// ����������� ��������� ���� �� Storage, � ��� �� ������������� ������ �����
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