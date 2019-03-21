#define H5_BUILT_AS_DYNAMIC_LIB


#include "hdfclasses.h"


const char * Stream::GetName()																															
{
	size_t len;
	char *buffer;

	len = H5Iget_name(dataset->getId(), NULL, 0);		// ������ ����� �����
	buffer = new char[len + 1];							// �������� ������
	H5Iget_name(dataset->getId(), buffer, len + 1);		// ������ ���
	name = buffer;
	return buffer;										// ������� ������ ����
}
enHDFTtypes Stream::GetType()																													
{
	return htByte;
}
long Stream::GetLength()																															
{
	return 0;
}
bool Stream::Seek(long _offset)																															
{
	hsize_t size;
	getDataSetInfo(dataset, &size);		// ������ ������
	if (_offset > (long)size)					// ���� ��������� ������� �� ������� ������, �� ���������� ��� � �����
	{
		pointer = size;
		return false;
	}
	else if(_offset < 0)				// ���� ��������� ������� �� ������� �����, �� ���������� ��� � ������
	{
		pointer = 0;
		return false;
	}

	pointer = _offset;					// ���������� ��������� � ������ �����
	return true;
}
long Stream::Read(void * _dest, long _cnt)																														
{
	return 0;
}

DataSet * Stream::createDataSet(const char *datasetName, Group *group, enHDFTtypes hdfType, DataSpace *dataspace)
{
	switch (hdfType)
	{
	case(htInt):
		return new DataSet(group->createDataSet(datasetName, H5T_NATIVE_INT, *dataspace));
	case(htUInt):
		return new DataSet(group->createDataSet(datasetName, H5T_NATIVE_UINT, *dataspace));
	default:
		return new DataSet(group->createDataSet(datasetName, H5T_C_S1, *dataspace));
	}
}

void Stream::writeData(void *data, DataSet *dataset, enHDFTtypes hdfType)
{
	switch (hdfType)
	{
	case (htInt):
		dataset->write(data, H5T_NATIVE_INT);
		return;

	case (htUInt):
		dataset->write(data, H5T_NATIVE_UINT);
		return;
	}
	dataset->write(data, H5T_C_S1);
}

void Stream::initDataSet()
{
	hsize_t size = 0;	
	dataspace = new DataSpace(1, &size);					// ������� dataspace (���������� ������� ����� ����� �����)
	dataset = createDataSet(name, group, type, dataspace);	// ������� ������� ������� ����
	pointer = 0;											// ��������� � ������
	dataspace = NULL;
	delete dataset;
	delete dataspace;
}

DataSet * Stream::readDataSet(const char* name, Group *group)
{
	try
	{
		return new DataSet(group->openDataSet(name));
	}
	catch (GroupIException not_found_error)
	{
		return NULL;
	}
}

int Stream::getDataSetInfo(DataSet *dataset, hsize_t *size)
{
	auto dataClass = dataset->getTypeClass();
	if (dataClass == H5T_INTEGER)					// ������ ���� ��� int, ���� uint
	{
		*size = dataset->getStorageSize() / 4;
		return 1;
	}
	else											// ���� ��� char
	{
		*size = dataset->getStorageSize();
		return 0;
	}
}

void* Stream::readData(DataSet *dataset, hsize_t *writedDataSize)
{
	void* data;
	auto dataClass = dataset->getTypeClass();
	int type_;
	
	type_ = getDataSetInfo(dataset, writedDataSize);
	if (type_)	// �������� ��� int, ���� uint
	{
		if (writedDataSize == 0)
		{
			return NULL;
		}
		data = new int[*writedDataSize];
		dataset->read(data, H5T_NATIVE_INT);
		return data;
	}

	// ������� ��� char ��� byte
	*writedDataSize = dataset->getStorageSize();		
	if (writedDataSize == 0)
	{
		return (void*)"";
	}
	data = new char[*writedDataSize];
	dataset->read(data, H5T_C_S1);

	writedDataSize--;	// ������ ����� ������ ������ �� ���������
	return data;
}

void * Stream::addArrayToArray(void *firstArray, void *secondArray, int firstArraySize, int secondArraySize)
{
	int *intData;					// ����� �������� int
	char *charData;					// ����� �������� char
	int i;



	if (type == htInt || type == htUInt)			// ���� ��� int, �� ���������� ��� ������� � intData
	{
		intData = new int[firstArraySize + secondArraySize];
		for (i = 0; i < firstArraySize; i++)
		{
			intData[i] = *((int*)firstArray + i);
		}
		i = pointer;
		for (int j = 0; j < secondArraySize; j++, i++)
		{
			intData[i] = *((int*)secondArray + j);
		}
		return intData;
	}
	else											// ���������� � charData
	{
		charData = new char[firstArraySize + secondArraySize];	
		for (i = 0; i < firstArraySize; i++)
		{
			charData[i] = *((char*)firstArray + i);
		}
		i = pointer;
		for (int j = 0; j < secondArraySize; j++, i++)
		{
			charData[i] = *((char*)secondArray + j);
		}
		return charData;
	}

}

void Stream::Write(void * _src, long _cnt)																														
{		

	hsize_t writedDataSize;									// ������ ������, ������� ��� ���� � ��������
	hsize_t size;											// ������ ��������� ������ (writedDataSize + dataSize)

	void *writedData = NULL;								// ������, ������� ��� ���� � ��������
	void *data = NULL;										// ��������� ������ (������ + �����), ������� ����� �������� � �������

	writedData = readData(dataset, &writedDataSize);		// ������� ������, ������� ��� ���� � �������� (�� writedDataSize �������� ������)
	H5Ldelete(group->getId(), GetName(), H5P_DEFAULT);		// ������� ������ �������


	data = addArrayToArray(writedData, _src, writedDataSize, _cnt);
	
	//size = _cnt - writedDataSize;
	size = pointer + _cnt;
	if (size < writedDataSize)
	{
		size = writedDataSize;
	}

	dataspace = new DataSpace(1, &size);					// ������� dataspace (���������� ������� ����� ����� �����)
	dataset = createDataSet(name, group, type, dataspace);	// ������� ������� ������� ����
	writeData(data, dataset, type);							// ����� ������

	pointer += _cnt;										// �������� ���������
	return;
}
Stream::Stream(const char * _name, enHDFTtypes _type, Group* _group, bool init)
{
	name = _name;
	type = _type;
	group = _group;
	if (init)				// ������� ������ �������, ���� ���������� ���� init
	{
		initDataSet();
	}
	else					// ���� ���� init � ����, �� 
	{
		dataset = readDataSet(name, group);		// ������� �������
		getDataSetInfo(dataset, &pointer);		// ���������� pointer � ����� �����
	}

}
Stream::~Stream()
{
	delete dataset;
	delete dataspace;
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

IHDFFolder *   Folder::GetFolder(long _index)																															
{
	return NULL;
}
IHDFFolder *  Folder::GetParent()																															
{
	return NULL;
}

IHDFStream *   Folder::CreateStream(const char * _name, enHDFTtypes _type)									
{
	try
	{
		DataSet(group->openDataSet(_name));				// ��������� �������
	}
	catch (GroupIException not_found_error)				// ���� ������ ��� ���, �� �������. ���� ����� ����, �� ������� NULL
	{
		char* fullname;
		fullname = (char*)GetName();					// ���� �� ������� ����������
		if (strcmp(fullname, "/") != 0)					// ���� ��� �� ������ ������, 
		{												// �� ������� stream � ������ ����� � ������
			strcat(fullname, "/");	
			strcat(fullname, _name);					
			return new Stream(fullname, _type, group, 1);	
		}
		return new Stream(_name, _type, group, 1);			// ������� stream � ������
	}
	return NULL;										
}
IHDFStream *   Folder::GetStream(const char * _name)																																
{
	DataSet *tmp;
	enHDFTtypes type;
	try
	{
		tmp = new DataSet(group->openDataSet(_name));		// ��������� ���� �� ����� �������
	}	
	catch (GroupIException not_found_error)				// ���� ������ ��� ���, �� ������� NULL
	{
		return NULL;
	}


	auto dataClass = tmp->getTypeClass();					// ���������� ���
	type = (dataClass == H5T_INTEGER) ? htInt : htChar;

	delete tmp;
	return new Stream(_name, type, group, 0);					// �������
}
IHDFStream *   Folder::GetStream(long _index)																							
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

long countSubStreams;		// ������� ��������� ���������
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
long  Folder::GetCountStream()																																
{
	countSubStreams = 0;
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);	// ��������� ���������
	return countSubStreams;
}

const char *  Folder::GetName()																																
{
	size_t len;
	char *buffer;

	len = H5Iget_name(group->getId(), NULL, 0);			// ������ ����� �����
	buffer = new char[len + 1];							// �������� ������
	H5Iget_name(group->getId(), buffer, len + 1);		// ������ ���
	return buffer;										// ������� ������ ����
}

Folder::Folder(H5File *file_)
{
	countSubFolders = 0;
	file = file_;						// ������� ����
	group = OpenGroup("/");				// ���������� ������
}
Folder::Folder(H5File *file_, const char *_group)
{
	countSubFolders = 0;
	file = file_;						// ������� ����
	group = OpenGroup(_group);		// ���������� ������ ������
}

Folder::~Folder()
{
	delete group;
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
