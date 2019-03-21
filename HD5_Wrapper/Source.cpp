#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("test.h5");
	IHDFStream *stream = folder->GetStream("char");
	
	char c[] = "321";
	char x[] = "123";
	char hp[] = "hello world";
	stream->Write(hp, strlen(hp));
	
	
	return 1;
}