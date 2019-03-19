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

long  Folder::GetCountFolder()																														
{
	char *name;					// Имя
	long count = 0;				// Количество слэшей в  имени
	int i = 0;

	name = (char*)GetName();
	
	while (name[i] != 0)		// Посчитали сколько слэшей в полном имени
	{
		if (name[i++] == '/')
		{
			count++;
		}
	}
	return count - 1;			// Вернули количество вложенных папок
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

	len = H5Iget_name(group->getId(), NULL, 0);			// Узнали длину имени
	buffer = new char[len + 1];							// Выделили память
	H5Iget_name(group->getId(), buffer, len + 1);		// Узнали имя
	name = buffer;										// Положили имя в строку
	return buffer;										// Вернули полный путь
}

Folder::Folder(H5File *file_)
{
	file = file_;						// Приняли файл
	group = OpenGroup("/");				// Установить корень
}
Folder::Folder(H5File *file_, const char *groupName)
{
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
