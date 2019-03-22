#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	//IHDFFolder *folder = folder->GetFolder("qwe");
	IHDFStream *stream = folder->GetStream("record");
	stream->Seek(25);
	char arr[] = "1234567890";
	stream->Write(arr, 5);
	stream->Write(arr, 10);
	stream->Write(arr, 2);

	cout << stream->GetLength();

	getchar();
	return 1;
}