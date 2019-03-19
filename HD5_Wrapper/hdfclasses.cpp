#define H5_BUILT_AS_DYNAMIC_LIB


#include "hdfclasses.h"


const char * GetName()																															///////
{
	return "ASD";
}
enHDFTtypes GetType()																															///////
{
	return htByte;
}
long GetLength()																															///////
{
	return 0;
}
bool Seek(long _offset)																															///////
{
	return false;
}
long Read(void * _dest, long _cnt)																															///////
{
	return 0;
}
void Write(void * _src, long _cnt)																															///////
{
	return;
}




IHDFFolder* Folder::GetFolder(const char * _name)
{
	char* fullname;
	fullname = (char*)GetName();
	if (strcmp(fullname, "/") != 0)
	{
		strcat(fullname, "/");
	}
	strcat(fullname, _name);					// ������� ����� ������ ����

	return new Folder(file, fullname);						// ������� folder ����� ������
}

Group * Folder::OpenGroup(const char * groupName)		// ���������� ������������ ������, ���� ������� ��
{
	try
	{
		return new Group(file->openGroup(groupName));
	}
	catch (Exception e)
	{
		return new Group(file->createGroup(groupName));
	}
}

IHDFFolder *   Folder::GetFolder(long _index)																																///////
{
	return NULL;
}
IHDFFolder *  Folder::GetParent()																																///////
{
	return NULL;
}

IHDFStream *   Folder::CreateStream(const char * _name, enHDFTtypes _type)																																///////
{
	return NULL;
}
IHDFStream *   Folder::GetStream(const char * _name)																																///////
{
	return NULL;
}
IHDFStream *   Folder::GetStream(long _index)																																///////
{
	return NULL;
}

long  Folder::GetCountFolder()																														
{
	char *name;					// ���
	long count = 0;				// ���������� ������ �  �����
	int i = 0;

	name = (char*)GetName();
	
	while (name[i] != 0)		// ��������� ������� ������ � ������ �����
	{
		if (name[i++] == '/')
		{
			count++;
		}
	}
	return count - 1;			// ������� ���������� ��������� �����
}
long  Folder::GetCountStream()																																///////
{
	return 0;
}
const char *  Folder::GetName()																																
{
	size_t len;
	char *buffer;
	std::string name;

	len = H5Iget_name(group->getId(), NULL, 0);			// ������ ����� �����
	buffer = new char[len + 1];							// �������� ������
	H5Iget_name(group->getId(), buffer, len + 1);		// ������ ���
	name = buffer;										// �������� ��� � ������
	return buffer;										// ������� ������ ����
}

Folder::Folder(H5File *file_)
{
	file = file_;						// ������� ����
	group = OpenGroup("/");				// ���������� ������
}
Folder::Folder(H5File *file_, const char *groupName)
{
	file = file_;						// ������� ����
	group = OpenGroup(groupName);		// ���������� ������ ������
}

Folder::~Folder()
{
	delete group;
	delete file;
}



IHDFFolder* Storage::Create(const char * _path)
{
	file = new H5File(_path, H5F_ACC_TRUNC);		// ������� ����
	return new Folder(file);						// ������� folder � ������������� ������
}

IHDFFolder * Storage::Open(const char * _path)
{
	try													// ���� ����� ���� ����������
	{
		file = new H5File(_path, H5F_ACC_RDWR);			// ������� ����
		return new Folder(file);						// ������� folder � ������������� ������
	}
	catch (Exception e)
	{
		return NULL;
	}
}
Storage::~Storage()
{
	delete file;
}
