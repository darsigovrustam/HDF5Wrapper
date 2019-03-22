#define H5_BUILT_AS_DYNAMIC_LIB


#include "hdfclasses.h"

// Счетчик поддиректорий
long countSubFolders;

// Счетчик вложенных датасетов
long countSubStreams;


Stream::~Stream()
{
	delete dataset;
	delete dataspace;
}

int Stream::getDataSetInfo(DataSet *dataset, hsize_t *size)
{
	auto dataClass = dataset->getTypeClass();

	// Читаем если это int, либо uint
	if (dataClass == H5T_INTEGER)					
	{
		*size = dataset->getStorageSize() / 4;
		return 1;
	}
	// Если это char
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

	// Узнали тип, если это int/uint
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

	// Если это char/byte
	*writedDataSize = dataset->getStorageSize();	
	if (writedDataSize == 0)
	{
		return (void*)"";
	}
	data = new char[*writedDataSize];
	dataset->read(data, H5T_C_S1);

	// Символ конца первой строки не требуется
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
	// Если такого датасета нет
	catch (GroupIException not_found_error)
	{
		// Вернуть NULL
		return NULL;
	}
}

void Stream::initDataSet()
{
	hsize_t size = 0;

	// Создали dataspace (определили сколько нужно будет места)
	dataspace = new DataSpace(1, &size);

	// Создает датасет нужного типа
	dataset = createDataSet(name, group, type, dataspace);

	// Указатель выставили в начало
	pointer = 0;

	dataspace = NULL;
	delete dataset;
	delete dataspace;
}

void * Stream::addArrayToArray(void *firstArray, void *secondArray, int firstArraySize, int secondArraySize)
{
	// Сумма массивов int
	int *intData;				

	// Сумма массивов char
	char *charData;					
	int i;

	// Если тип int, то записываем оба массива в intData
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
	// Аналогично с charData
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

	// Если установлен флаг init
	if (init)				
	{	
		// Создать пустой датасет
		initDataSet();
	}
	// Если флаг init не установлен
	else					
	{
		// Считать датасет из файла
		dataset = readDataSet(name, group);		

		// Установить pointer в конец файла
		getDataSetInfo(dataset, &pointer);		
	}
}

const char * Stream::GetName()																															
{
	size_t len;
	char *buffer;

	// Узнали длину имени
	len = H5Iget_name(dataset->getId(), NULL, 0);		

	// Выделили память
	buffer = new char[len + 1];							

	// Узнали имя
	H5Iget_name(dataset->getId(), buffer, len + 1);		
	name = buffer;

	// Вернули полный путь
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

	// Узнали размер датасета
	getDataSetInfo(dataset, &size);		

	// Если указатель выходит за границу справа
	if (_offset > (long)size)			
	{								
		// то установить его в конец
		pointer = size;
		return false;
	}
	// Если указатель выходит за границу слева, 
	if(_offset < 0)						
	{							
		// то установить его в начало
		pointer = 0;
		return false;
	}

	// Установили указатель в нужное место
	pointer = _offset;					
	return true;
}

long Stream::Read(void ** _dest, long _cnt)																														
{
	void *data;
	int *intData;
	char *charData;

	// Размер датасета
	hsize_t size;								

	// Считали данные из датасета
	data = readData(dataset, &size);			
	
	if (long(size - pointer) < _cnt)
	{
		_cnt = size - pointer;
	}

	// Если int/uint - поместили данные в intData
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
		// Char/byte - в charData
		*_dest = new char[_cnt];	
		charData = (char*)*_dest;
		for (int i = 0, j = pointer; j < _cnt + pointer; i++, j++)
		{
			charData[i] = *((char*)data + j);
		}
	}

	// Сместили указатель
	pointer += _cnt;							
	return _cnt;
}

void Stream::Write(void * _src, long _cnt)																														
{		
	// Размер данных, которые уже были в датасете
	hsize_t writedDataSize;		

	// Размер суммарных данных (writedDataSize + dataSize)
	hsize_t size;													

	// Данные, которые уже были в датасете
	void *writedData = NULL;			

	// Суммарные данные (старые + новые), которые нужно записать в датасет
	void *data = NULL;												

	// Считали данные, которые уже были в датасете (во writedDataSize кладется размер)
	writedData = readData(dataset, &writedDataSize);			

	// Удалили старый датасет
	H5Ldelete(group->getId(), GetName(), H5P_DEFAULT);				

	// Складываем в data = (writedData + _src)
	data = addArrayToArray(writedData, _src, writedDataSize, _cnt);	
	
	size = pointer + _cnt;
	if (size < writedDataSize)
	{
		size = writedDataSize;
	}

	// Создали dataspace (определили сколько нужно будет места)
	dataspace = new DataSpace(1, &size);		

	// Создает датасет нужного типа
	dataset = createDataSet(name, group, type, dataspace);			

	// Пишет данные
	writeData(data, dataset, type);									

	// Сместили указатель
	pointer += _cnt;

	// Посчитали общее количество записанных
	// с помощью этого стрима данных
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

	// Приняли файл
	file = file_;					

	// Установить корень
	group = OpenGroup("/");				
}

Folder::Folder(H5File *file_, const char *_group)
{
	countSubFolders = 0;

	// Приняли файл
	file = file_;

	// Установили нужную группу
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

	// Создали новый полный путь
	strcat(fullname, _name);					

	// Вернули folder новой группы
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

	// Получили свое полное имя
	fullParentName = (char*)GetName();			
	i = strlen(fullParentName) - 1;

	// Если открыт корень - вернуть NULL 
	if (!i)										
	{
		return NULL;
	}

	// Убираем свое имя до сплэша
	while (i > 0)								
	{
		if (fullParentName[i] == '/')
		{
			fullParentName[i] = 0;
			break;
		}
		fullParentName[i--] = 0;
	}

	// Вернуть folder с родителем
	return new Folder(file, fullParentName);	
}

IHDFStream *   Folder::GetStream(const char * _name)																																
{
	DataSet *tmp;
	enHDFTtypes type;
	try
	{
		// Проверить есть ли такой датасет
		tmp = new DataSet(group->openDataSet(_name));	
	}	
	// Если такого еще нет - вернуть NULL
	catch (GroupIException not_found_error)				
	{
		return NULL;
	}

	// Определили тип
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
	// Полное имя стрима
	char* fullName;

	try
	{
		// Прочитать датасет
		DataSet(group->openDataSet(_name));				
	}
	// Если такого датасета еще нет - создать
	catch (GroupIException not_found_error)				
	{													
		// Путь до текущей директории
		fullName = (char*)GetName();

		// Если это не просто корень, 
		if (strcmp(fullName, "/") != 0)
		{												
			strcat(fullName, "/");
			strcat(fullName, _name);

			// то вернуть stream с полным путем и именем
			return new Stream(fullName, _type, group, 1);
		}

		// Вернули stream с именем
		return new Stream(_name, _type, group, 1);		
	}
	// Если такой датасет уже есть - вернуть NULL
	return NULL;
}		

// Рекурсивный поиск подгрупп в группе
herr_t group_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	// ID подгруппы	
	hid_t subFolderID;											

	// Открываем как группу
	subFolderID = H5Gopen2(loc_id, name, H5P_DEFAULT);		

	// Если это не группа, то пропускаем
	if (subFolderID == -1)									
	{
		return 0;
	}

	// Счетчик подгрупп увеличивается
	countSubFolders++;

	// Поиск в текущей подгруппе
	H5Literate(subFolderID, H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);	

	H5Gclose(subFolderID);
	return 0;
}

long  Folder::GetCountFolder()																														
{
	countSubFolders = 0;
	
	// Посчитали сабгруппы
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, group_info, NULL);
	return	countSubFolders;
}

// Рекурсивный поиск датасетов в группах
herr_t dataset_info(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
{
	// ID вложенного объекта
	hid_t subObjectID;												

	// Открываем как датасет
	subObjectID = H5Dopen2(loc_id, name, H5P_DEFAULT);		

	// Если это датасет, то счетчик увеличить
	if (subObjectID != -1)									
	{
		countSubStreams++;
		H5Gclose(subObjectID);
		return 0;
	}

	// Открываем как группу 
	subObjectID = H5Gopen2(loc_id, name, H5P_DEFAULT);		

	// Если это группа, то ищем в ней
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

	// Посчитали сабгруппы
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);	
	return countSubStreams;
}

const char * Folder::GetName()																																
{
	size_t len;
	char *buffer;

	// Узнали длину имени
	len = H5Iget_name(group->getId(), NULL, 0);			

	// Выделили память
	buffer = new char[len + 1];							

	// Узнали имя
	H5Iget_name(group->getId(), buffer, len + 1);		

	// Вернули полный путь
	return buffer;										
}



Storage::~Storage()
{
	delete file;
}

IHDFFolder* Storage::Create(const char * _path)
{
	// Создали файл
	file = new H5File(_path, H5F_ACC_TRUNC);	

	// Вернули folder с установленным корнем
	return new Folder(file);						
}

IHDFFolder * Storage::Open(const char * _path)
{
	// Если открываемый файл существует
	try												
	{
		// Открыли файл
		file = new H5File(_path, H5F_ACC_RDWR);		

		// Вернули folder с установленным корнем
		return new Folder(file);					
	}
	// Если открываемый файл не существует 
	catch (Exception e)
	{
		// Вернуть NULL
		return NULL;
	}
}

