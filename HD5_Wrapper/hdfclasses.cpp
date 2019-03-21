#define H5_BUILT_AS_DYNAMIC_LIB


#include "hdfclasses.h"


const char * Stream::GetName()																															
{
	size_t len;
	char *buffer;

	len = H5Iget_name(dataset->getId(), NULL, 0);		// Узнали длину имени
	buffer = new char[len + 1];							// Выделили память
	H5Iget_name(dataset->getId(), buffer, len + 1);		// Узнали имя
	name = buffer;
	return buffer;										// Вернули полный путь
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
	getDataSetInfo(dataset, &size);		// Узнали размер
	if (_offset > (long)size)					// Если указатель выходит за границу справа, то установить его в конец
	{
		pointer = size;
		return false;
	}
	else if(_offset < 0)				// Если указатель выходит за границу слева, то установить его в начало
	{
		pointer = 0;
		return false;
	}

	pointer = _offset;					// Установили указатель в нужное место
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
	dataspace = new DataSpace(1, &size);					// Создали dataspace (определили сколько нужно будет места)
	dataset = createDataSet(name, group, type, dataspace);	// Создает датасет нужного типа
	pointer = 0;											// Указатель в начало
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
	if (dataClass == H5T_INTEGER)					// Читаем если это int, либо uint
	{
		*size = dataset->getStorageSize() / 4;
		return 1;
	}
	else											// Если это char
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
	if (type_)	// Прочесть как int, либо uint
	{
		if (writedDataSize == 0)
		{
			return NULL;
		}
		data = new int[*writedDataSize];
		dataset->read(data, H5T_NATIVE_INT);
		return data;
	}

	// Прочеть как char или byte
	*writedDataSize = dataset->getStorageSize();		
	if (writedDataSize == 0)
	{
		return (void*)"";
	}
	data = new char[*writedDataSize];
	dataset->read(data, H5T_C_S1);

	writedDataSize--;	// Символ конца первой строки не требуется
	return data;
}

void * Stream::addArrayToArray(void *firstArray, void *secondArray, int firstArraySize, int secondArraySize)
{
	int *intData;					// Сумма массивов int
	char *charData;					// Сумма массивов char
	int i;



	if (type == htInt || type == htUInt)			// Если тип int, то записываем оба массива в intData
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
	else											// Аналогично с charData
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

	hsize_t writedDataSize;									// Размер данных, которые уже были в датасете
	hsize_t size;											// Размер суммарных данных (writedDataSize + dataSize)

	void *writedData = NULL;								// Данные, которые уже были в датасете
	void *data = NULL;										// Суммарные данные (старые + новые), которые нужно записать в датасет

	writedData = readData(dataset, &writedDataSize);		// Считали данные, которые уже были в датасете (во writedDataSize кладется размер)
	H5Ldelete(group->getId(), GetName(), H5P_DEFAULT);		// Удалили старый датасет


	data = addArrayToArray(writedData, _src, writedDataSize, _cnt);
	
	//size = _cnt - writedDataSize;
	size = pointer + _cnt;
	if (size < writedDataSize)
	{
		size = writedDataSize;
	}

	dataspace = new DataSpace(1, &size);					// Создали dataspace (определили сколько нужно будет места)
	dataset = createDataSet(name, group, type, dataspace);	// Создает датасет нужного типа
	writeData(data, dataset, type);							// Пишет данные

	pointer += _cnt;										// Сместили указатель
	return;
}
Stream::Stream(const char * _name, enHDFTtypes _type, Group* _group, bool init)
{
	name = _name;
	type = _type;
	group = _group;
	if (init)				// Создать пустой датасет, если установлен флаг init
	{
		initDataSet();
	}
	else					// Если флаг init в ноль, то 
	{
		dataset = readDataSet(name, group);		// Считать датасет
		getDataSetInfo(dataset, &pointer);		// Установить pointer в конец файла
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
	strcat(fullname, _name);					// Создали новый полный путь

	return new Folder(file, fullname);						// Вернули folder новой группы
}

Group * Folder::OpenGroup(const char * groupName)		// Возвращает существующую группу, либо создает ее
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
		DataSet(group->openDataSet(_name));				// Прочитать датасет
	}
	catch (GroupIException not_found_error)				// Если такого еще нет, то создать. Если такой есть, то вернуть NULL
	{
		char* fullname;
		fullname = (char*)GetName();					// Путь до текущей директории
		if (strcmp(fullname, "/") != 0)					// Если это не просто корень, 
		{												// то вернуть stream с полным путем и именем
			strcat(fullname, "/");	
			strcat(fullname, _name);					
			return new Stream(fullname, _type, group, 1);	
		}
		return new Stream(_name, _type, group, 1);			// Вернули stream с именем
	}
	return NULL;										
}
IHDFStream *   Folder::GetStream(const char * _name)																																
{
	DataSet *tmp;
	enHDFTtypes type;
	try
	{
		tmp = new DataSet(group->openDataSet(_name));		// Проверить есть ли такой датасет
	}	
	catch (GroupIException not_found_error)				// Если такого еще нет, то вернуть NULL
	{
		return NULL;
	}


	auto dataClass = tmp->getTypeClass();					// Определили тип
	type = (dataClass == H5T_INTEGER) ? htInt : htChar;

	delete tmp;
	return new Stream(_name, type, group, 0);					// Вернули
}
IHDFStream *   Folder::GetStream(long _index)																							
{
	return NULL;
}

long countSubFolders;		// Счетчик поддиректорий
// Рекурсивный поиск подгрупп в группе
herr_t group_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	hid_t subFolderID;										// ID подгруппы		

	subFolderID = H5Gopen2(loc_id, name, H5P_DEFAULT);		// Открываем как группу
	if (subFolderID == -1)									// Если это не группа, то пропускаем
	{
		return 0;
	}
	//cout << "SubFolder Name : " << name << endl;
	countSubFolders++;
	H5Literate(subFolderID, H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);	// Поиск в текущей подгруппе

	H5Gclose(subFolderID);
	return 0;
}
long  Folder::GetCountFolder()																														
{
	countSubFolders = 0;
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);	// Посчитали сабгруппы
	return	countSubFolders;
}

long countSubStreams;		// Счетчик вложенных датасетов
// Рекурсивный поиск датасетов в группах
herr_t dataset_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	hid_t subObjectID;										// ID вложенного объекта		

	subObjectID = H5Dopen2(loc_id, name, H5P_DEFAULT);		// Открываем как датасет
	if (subObjectID != -1)									// Если это датасет, то счетчик увеличить
	{
		countSubStreams++;
		H5Gclose(subObjectID);
		return 0;
	}

	subObjectID = H5Gopen2(loc_id, name, H5P_DEFAULT);		// Открываем как группу 
	if (subObjectID != -1)									// Если это группа, то ищем в ней
	{
		H5Literate(subObjectID, H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);
		H5Gclose(subObjectID);
	}

	return 0;
}
long  Folder::GetCountStream()																																
{
	countSubStreams = 0;
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);	// Посчитали сабгруппы
	return countSubStreams;
}

const char *  Folder::GetName()																																
{
	size_t len;
	char *buffer;

	len = H5Iget_name(group->getId(), NULL, 0);			// Узнали длину имени
	buffer = new char[len + 1];							// Выделили память
	H5Iget_name(group->getId(), buffer, len + 1);		// Узнали имя
	return buffer;										// Вернули полный путь
}

Folder::Folder(H5File *file_)
{
	countSubFolders = 0;
	file = file_;						// Приняли файл
	group = OpenGroup("/");				// Установить корень
}
Folder::Folder(H5File *file_, const char *_group)
{
	countSubFolders = 0;
	file = file_;						// Приняли файл
	group = OpenGroup(_group);		// Установили нужную группу
}

Folder::~Folder()
{
	delete group;
}



IHDFFolder* Storage::Create(const char * _path)
{
	file = new H5File(_path, H5F_ACC_TRUNC);		// Создали файл
	return new Folder(file);						// Вернули folder с установленным корнем
}

IHDFFolder * Storage::Open(const char * _path)
{
	try													// Если такой файл существует
	{
		file = new H5File(_path, H5F_ACC_RDWR);			// Открыли файл
		return new Folder(file);						// Вернули folder с установленным корнем
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
