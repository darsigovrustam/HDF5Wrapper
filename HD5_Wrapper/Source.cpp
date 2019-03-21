#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	//IHDFFolder *folder = folder->GetFolder("qwe");
	IHDFStream *stream = folder->GetStream("record");

	char p[] = "aaaaaaaaaa";

	stream->Seek(10);
	stream->Write(p, strlen(p));
	cout << p << endl;

	getchar();
	return 1;
}