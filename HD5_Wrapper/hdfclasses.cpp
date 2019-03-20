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

long countSubFolders;		// ������� �������������
// ����������� ����� �������� � ������
herr_t group_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	hid_t subFolderID;										// ID ���������		

	subFolderID = H5Gopen2(loc_id, name, H5P_DEFAULT);		// ��������� ��� ������
	if (subFolderID == -1)									// ���� ��� �� ������, �� ����������
	{
		return 0;
	}
	//cout << "SubFolder Name : " << name << endl;
	countSubFolders++;
	H5Literate(subFolderID, H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);	// ����� � ������� ���������

	H5Gclose(subFolderID);
	return 0;
}

long  Folder::GetCountFolder()																														
{
	countSubFolders = 0;
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);	// ��������� ���������
	return	countSubFolders;
}

long countSubStreams;
// ����������� ����� ��������� � �������
herr_t dataset_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	hid_t subObjectID;										// ID ���������� �������		

	subObjectID = H5Dopen2(loc_id, name, H5P_DEFAULT);		// ��������� ��� �������
	if (subObjectID != -1)									// ���� ��� �������, �� ������� ���������
	{
		countSubStreams++;
		H5Gclose(subObjectID);
		return 0;
	}

	subObjectID = H5Gopen2(loc_id, name, H5P_DEFAULT);		// ��������� ��� ������ 
	if (subObjectID != -1)									// ���� ��� ������, �� ���� � ���
	{
		H5Literate(subObjectID, H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);
		H5Gclose(subObjectID);
	}

	return 0;
}

long  Folder::GetCountStream()																																///////
{
	countSubStreams = 0;
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);	// ��������� ���������
	return countSubStreams;
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
	countSubFolders = 0;
	file = file_;						// ������� ����
	group = OpenGroup("/");				// ���������� ������
}
Folder::Folder(H5File *file_, const char *groupName)
{
	countSubFolders = 0;
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
