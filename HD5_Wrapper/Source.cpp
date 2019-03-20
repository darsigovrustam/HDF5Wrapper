#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	IHDFFolder *folder2 = folder->GetFolder("qwe");

	IHDFStream *stream = folder2->CreateStream("111", htInt);
	IHDFStream *stream2 = folder->CreateStream("12", htInt);
	cout << stream2->GetName() << endl;
	cout << stream->GetName() << endl;

	//cout << stream->GetName();

	getchar();
	return 1;
}