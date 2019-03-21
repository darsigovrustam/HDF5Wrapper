#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	//IHDFFolder *folder = folder->GetFolder("qwe");
	IHDFStream *stream = folder->GetStream("record");

	char *p;
	stream->Seek(0);
	long size = stream->Read((void**)&p, 5);
	cout << p << endl;
	delete p;
	size = stream->Read((void**)&p, 5);

	getchar();
	return 1;
}