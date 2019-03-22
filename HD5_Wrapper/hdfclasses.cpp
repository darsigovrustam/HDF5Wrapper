#define H5_BUILT_AS_DYNAMIC_LIB


#include "hdfclasses.h"

// ������� �������������
long countSubFolders;

// ������� ��������� ���������
long countSubStreams;


Stream::~Stream()
{
	delete dataset;
	delete dataspace;
}

int Stream::getDataSetInfo(DataSet *dataset, hsize_t *size)
{
	auto dataClass = dataset->getTypeClass();

	// ������ ���� ��� int, ���� uint
	if (dataClass == H5T_INTEGER)					
	{
		*size = dataset->getStorageSize() / 4;
		return 1;
	}
	// ���� ��� char
	else											
	{
		*size = dataset->getStorageSize();
		return 0;
	}
}

void* Stream::readData(DataSet *dataset, hsize_t *writedDataSize)
{
	void* data;
	int type_;

	// ������ ���, ���� ��� int/uint
	if (getDataSetInfo(dataset, writedDataSize))	
	{
		if (writedDataSize == 0)
		{
			return NULL;
		}
		data = new int[*writedDataSize];
		dataset->read(data, H5T_NATIVE_INT);
		return data;
	}

	// ���� ��� char/byte
	*writedDataSize = dataset->getStorageSize();	
	if (writedDataSize == 0)
	{
		return (void*)"";
	}
	data = new char[*writedDataSize];
	dataset->read(data, H5T_C_S1);

	// ������ ����� ������ ������ �� ���������
	writedDataSize--;								
	return data;
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

DataSet * Stream::readDataSet(const char* name, Group *group)
{
	try
	{
		return new DataSet(group->openDataSet(name));
	}
	// ���� ������ �������� ���
	catch (GroupIException not_found_error)
	{
		// ������� NULL
		return NULL;
	}
}

void Stream::initDataSet()
{
	hsize_t size = 0;

	// ������� dataspace (���������� ������� ����� ����� �����)
	dataspace = new DataSpace(1, &size);

	// ������� ������� ������� ����
	dataset = createDataSet(name, group, type, dataspace);

	// ��������� ��������� � ������
	pointer = 0;

	dataspace = NULL;
	delete dataset;
	delete dataspace;
}

void * Stream::addArrayToArray(void *firstArray, void *secondArray, int firstArraySize, int secondArraySize)
{
	// ����� �������� int
	int *intData;				

	// ����� �������� char
	char *charData;					
	int i;

	// ���� ��� int, �� ���������� ��� ������� � intData
	if (type == htInt || type == htUInt)			
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
	// ���������� � charData
	else											
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

Stream::Stream(const char * _name, enHDFTtypes _type, Group* _group, bool init)
{
	name = _name;
	type = _type;
	group = _group;
	writedDataCount = 0;

	// ���� ���������� ���� init
	if (init)				
	{	
		// ������� ������ �������
		initDataSet();
	}
	// ���� ���� init �� ����������
	else					
	{
		// ������� ������� �� �����
		dataset = readDataSet(name, group);		

		// ���������� pointer � ����� �����
		getDataSetInfo(dataset, &pointer);		
	}
}

const char * Stream::GetName()																															
{
	size_t len;
	char *buffer;

	// ������ ����� �����
	len = H5Iget_name(dataset->getId(), NULL, 0);		

	// �������� ������
	buffer = new char[len + 1];							

	// ������ ���
	H5Iget_name(dataset->getId(), buffer, len + 1);		
	name = buffer;

	// ������� ������ ����
	return buffer;										
}

enHDFTtypes Stream::GetType()																													
{
	return type;
}

long Stream::GetLength()																															
{
	return writedDataCount;
}

bool Stream::Seek(long _offset)																															
{
	hsize_t size;

	// ������ ������ ��������
	getDataSetInfo(dataset, &size);		

	// ���� ��������� ������� �� ������� ������
	if (_offset > (long)size)			
	{								
		// �� ���������� ��� � �����
		pointer = size;
		return false;
	}
	// ���� ��������� ������� �� ������� �����, 
	if(_offset < 0)						
	{							
		// �� ���������� ��� � ������
		pointer = 0;
		return false;
	}

	// ���������� ��������� � ������ �����
	pointer = _offset;					
	return true;
}

long Stream::Read(void ** _dest, long _cnt)																														
{
	void *data;
	int *intData;
	char *charData;

	// ������ ��������
	hsize_t size;								

	// ������� ������ �� ��������
	data = readData(dataset, &size);			
	
	if (long(size - pointer) < _cnt)
	{
		_cnt = size - pointer;
	}

	// ���� int/uint - ��������� ������ � intData
	if (type == htInt || type == htUInt)		
	{
		*_dest = new int[_cnt];
		intData = (int*)*_dest;
		for (int i = 0, j = pointer; j < _cnt + pointer; i++, j++)
		{
			intData[i] = *((int*)data + j);
		}
	}
	else										
	{
		// Char/byte - � charData
		*_dest = new char[_cnt];	
		charData = (char*)*_dest;
		for (int i = 0, j = pointer; j < _cnt + pointer; i++, j++)
		{
			charData[i] = *((char*)data + j);
		}
	}

	// �������� ���������
	pointer += _cnt;							
	return _cnt;
}

void Stream::Write(void * _src, long _cnt)																														
{		
	// ������ ������, ������� ��� ���� � ��������
	hsize_t writedDataSize;		

	// ������ ��������� ������ (writedDataSize + dataSize)
	hsize_t size;													

	// ������, ������� ��� ���� � ��������
	void *writedData = NULL;			

	// ��������� ������ (������ + �����), ������� ����� �������� � �������
	void *data = NULL;												

	// ������� ������, ������� ��� ���� � �������� (�� writedDataSize �������� ������)
	writedData = readData(dataset, &writedDataSize);			

	// ������� ������ �������
	H5Ldelete(group->getId(), GetName(), H5P_DEFAULT);				

	// ���������� � data = (writedData + _src)
	data = addArrayToArray(writedData, _src, writedDataSize, _cnt);	
	
	size = pointer + _cnt;
	if (size < writedDataSize)
	{
		size = writedDataSize;
	}

	// ������� dataspace (���������� ������� ����� ����� �����)
	dataspace = new DataSpace(1, &size);		

	// ������� ������� ������� ����
	dataset = createDataSet(name, group, type, dataspace);			

	// ����� ������
	writeData(data, dataset, type);									

	// �������� ���������
	pointer += _cnt;

	// ��������� ����� ���������� ����������
	// � ������� ����� ������ ������
	writedDataCount += _cnt;

	return;
}



Folder::~Folder()
{
	delete group;
}

Group * Folder::OpenGroup(const char * groupName)
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

Folder::Folder(H5File *file_)
{
	countSubFolders = 0;

	// ������� ����
	file = file_;					

	// ���������� ������
	group = OpenGroup("/");				
}

Folder::Folder(H5File *file_, const char *_group)
{
	countSubFolders = 0;

	// ������� ����
	file = file_;

	// ���������� ������ ������
	group = OpenGroup(_group);
}

IHDFFolder* Folder::GetFolder(const char * _name)
{
	char* fullname;
	fullname = (char*)GetName();
	if (strcmp(fullname, "/") != 0)
	{
		strcat(fullname, "/");
	}

	// ������� ����� ������ ����
	strcat(fullname, _name);					

	// ������� folder ����� ������
	return new Folder(file, fullname);			
}

IHDFFolder *   Folder::GetFolder(long _index)
{
	return NULL;
}

IHDFFolder *  Folder::GetParent()																															
{
	int i;
	char* fullParentName;

	// �������� ���� ������ ���
	fullParentName = (char*)GetName();			
	i = strlen(fullParentName) - 1;

	// ���� ������ ������ - ������� NULL 
	if (!i)										
	{
		return NULL;
	}

	// ������� ���� ��� �� ������
	while (i > 0)								
	{
		if (fullParentName[i] == '/')
		{
			fullParentName[i] = 0;
			break;
		}
		fullParentName[i--] = 0;
	}

	// ������� folder � ���������
	return new Folder(file, fullParentName);	
}

IHDFStream *   Folder::GetStream(const char * _name)																																
{
	DataSet *tmp;
	enHDFTtypes type;
	try
	{
		// ��������� ���� �� ����� �������
		tmp = new DataSet(group->openDataSet(_name));	
	}	
	// ���� ������ ��� ��� - ������� NULL
	catch (GroupIException not_found_error)				
	{
		return NULL;
	}

	// ���������� ���
	auto dataClass = tmp->getTypeClass();				
	type = (dataClass == H5T_INTEGER) ? htInt : htChar;

	delete tmp;
	return new Stream(_name, type, group, 0);				
}

IHDFStream *   Folder::GetStream(long _index)																							
{
	return NULL;
}

IHDFStream * Folder::CreateStream(const char * _name, enHDFTtypes _type)
{
	// ������ ��� ������
	char* fullName;

	try
	{
		// ��������� �������
		DataSet(group->openDataSet(_name));				
	}
	// ���� ������ �������� ��� ��� - �������
	catch (GroupIException not_found_error)				
	{													
		// ���� �� ������� ����������
		fullName = (char*)GetName();

		// ���� ��� �� ������ ������, 
		if (strcmp(fullName, "/") != 0)
		{												
			strcat(fullName, "/");
			strcat(fullName, _name);

			// �� ������� stream � ������ ����� � ������
			return new Stream(fullName, _type, group, 1);
		}

		// ������� stream � ������
		return new Stream(_name, _type, group, 1);		
	}
	// ���� ����� ������� ��� ���� - ������� NULL
	return NULL;
}		

// ����������� ����� �������� � ������
herr_t group_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	// ID ���������	
	hid_t subFolderID;											

	// ��������� ��� ������
	subFolderID = H5Gopen2(loc_id, name, H5P_DEFAULT);		

	// ���� ��� �� ������, �� ����������
	if (subFolderID == -1)									
	{
		return 0;
	}

	// ������� �������� �������������
	countSubFolders++;

	// ����� � ������� ���������
	H5Literate(subFolderID, H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);	

	H5Gclose(subFolderID);
	return 0;
}

long  Folder::GetCountFolder()																														
{
	countSubFolders = 0;
	
	// ��������� ���������
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);
	return	countSubFolders;
}

// ����������� ����� ��������� � �������
herr_t dataset_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	// ID ���������� �������
	hid_t subObjectID;												

	// ��������� ��� �������
	subObjectID = H5Dopen2(loc_id, name, H5P_DEFAULT);		

	// ���� ��� �������, �� ������� ���������
	if (subObjectID != -1)									
	{
		countSubStreams++;
		H5Gclose(subObjectID);
		return 0;
	}

	// ��������� ��� ������ 
	subObjectID = H5Gopen2(loc_id, name, H5P_DEFAULT);		

	// ���� ��� ������, �� ���� � ���
	if (subObjectID != -1)									
	{
		H5Literate(subObjectID, H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);
		H5Gclose(subObjectID);
	}
	
	return 0;
}

long  Folder::GetCountStream()																																
{
	countSubStreams = 0;

	// ��������� ���������
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);	
	return countSubStreams;
}

const char * Folder::GetName()																																
{
	size_t len;
	char *buffer;

	// ������ ����� �����
	len = H5Iget_name(group->getId(), NULL, 0);			

	// �������� ������
	buffer = new char[len + 1];							

	// ������ ���
	H5Iget_name(group->getId(), buffer, len + 1);		

	// ������� ������ ����
	return buffer;										
}



Storage::~Storage()
{
	delete file;
}

IHDFFolder* Storage::Create(const char * _path)
{
	// ������� ����
	file = new H5File(_path, H5F_ACC_TRUNC);	

	// ������� folder � ������������� ������
	return new Folder(file);						
}

IHDFFolder * Storage::Open(const char * _path)
{
	// ���� ����������� ���� ����������
	try												
	{
		// ������� ����
		file = new H5File(_path, H5F_ACC_RDWR);		

		// ������� folder � ������������� ������
		return new Folder(file);					
	}
	// ���� ����������� ���� �� ���������� 
	catch (Exception e)
	{
		// ������� NULL
		return NULL;
	}
}

