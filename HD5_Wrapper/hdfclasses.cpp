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

long countSubStreams;
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

long  Folder::GetCountStream()																																///////
{
	countSubStreams = 0;
	H5Literate(group->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL, dataset_info, NULL);	// Посчитали сабгруппы
	return countSubStreams;
}
const char *  Folder::GetName()																																
{
	size_t len;
	char *buffer;
	std::string name;

	len = H5Iget_name(group->getId(), NULL, 0);			// Узнали длину имени
	buffer = new char[len + 1];							// Выделили память
	H5Iget_name(group->getId(), buffer, len + 1);		// Узнали имя
	name = buffer;										// Положили имя в строку
	return buffer;										// Вернули полный путь
}

Folder::Folder(H5File *file_)
{
	countSubFolders = 0;
	file = file_;						// Приняли файл
	group = OpenGroup("/");				// Установить корень
}
Folder::Folder(H5File *file_, const char *groupName)
{
	countSubFolders = 0;
	file = file_;						// Приняли файл
	group = OpenGroup(groupName);		// Установили нужную группу
}

Folder::~Folder()
{
	delete group;
	delete file;
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
