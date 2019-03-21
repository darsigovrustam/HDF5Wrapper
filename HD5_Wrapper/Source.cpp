#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	IHDFStream *stream = folder->GetStream("int");
	stream->Seek(2);

	cout << stream->Seek(17) << endl;	//can do it
	cout << stream->Seek(-2) << endl;	// will be 0
	cout << stream->Seek(18) << endl;	// will ne 17

	getchar();
	return 1;
}