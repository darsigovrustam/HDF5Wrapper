#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	IHDFStream *stream = folder->GetStream("int");
	
	int arr[] = { 0, 0, 0, 0 };
	char arr2[] = "qqqqq";
	stream->Write(arr, 2);
	
	
	return 1;
}