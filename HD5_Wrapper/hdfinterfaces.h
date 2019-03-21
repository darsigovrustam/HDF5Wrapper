/*
	Ознакомиться с библиотекой с открытым исходным кодом HDF5 (https://www.hdfgroup.org/)
	Написать классы СHDFStorage СHDFFolder СHDFStream реализующие интерфейсы IHDFStorage IHDFFolder IHDFStream соответственно.
	Классы должны создавать/писать/читать файлы формата HDF5 и являться обертками для библиотеки HDF5Lib
	Разработка в Visual Studio 
	Приложение в котором будут использоваться эти класы может быть любым по Вашему желанию: консольное, winform, wpf
*/

#include "MainHeader.h"



namespace HDF5Wrapper
{
    enum enHDFTtypes {htByte, htChar, htUInt, htInt};

    class IHDFStream
    {
        public: 
        virtual const char * GetName() = NULL; //возвращает имя
        virtual enHDFTtypes GetType() = NULL; //возвращает тип
        virtual long GetLength() = NULL; //возвращает длинну (количество записанных данных )
        virtual bool Seek(long _offset) = NULL; //Перемещает указатель чтения/записи в позицию (вернет фальш если вне диапазона)
        virtual long Read(void ** _dest, long _cnt ) = NULL; //читает _cnt  элементов в _dest, с текущей позиции, возвращает реальное количество прочитанных элементов
        virtual void Write(void * _src, long _cnt ) = NULL; //пишет _cnt  элементов из _src, с текущей позиции

    };
    class IHDFFolder
    {
        public: 
        virtual IHDFFolder *  GetFolder(const char * _name) = NULL;//должен создать если нет, или вернуть существующий
        virtual IHDFFolder *  GetFolder(long _index) = NULL;//возвращает существующую папку по индексу
        virtual IHDFFolder *  GetParent() = NULL;//возвращает родительскую папку

        virtual IHDFStream *  CreateStream(const char * _name, enHDFTtypes _type) = NULL;//если уже существует, должен вернуть NULL 
        virtual IHDFStream *  GetStream(const char * _name) = NULL;//если нет, должен вернуть NULL
        virtual IHDFStream *  GetStream(long _index) = NULL;//возвращает существующий поток по индексу
        
        virtual long GetCountFolder() = NULL; //возвращает количество вложенных папок
        virtual long GetCountStream() = NULL; //возвращает количество вложенных потоков
        virtual const char * GetName() = NULL; //возвращает имя


    };
    class IHDFStorage
    {
        public: 
        virtual IHDFFolder * Create(const char * _path) = NULL; //создает новую папку (корневая папка являющаяся файлом на диске с путем _path)
        virtual IHDFFolder * Open(const char * _path) = NULL;
    };
};