#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("aaa.h5");

	IHDFStream *stream = folder->GetStream("uint");
	IHDFStream *byteStream = folder->GetStream("byte");

	char *p;
	unsigned int *i;

	byteStream->Read((void**)&p, 6);
	stream->Read((void**)&i, 3);


;

	IHDFFolder *folder2 = storage->Open("test.h5");
	IHDFStream *stream2 = folder2->GetStream("int5");
	int *intt;

	stream2->Seek(0);
	stream2->Read((void**)&intt, 2);

	return 1;
}