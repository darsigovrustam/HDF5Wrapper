#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	const char *n = folder->GetName();
	IHDFFolder *folder2 = folder->GetFolder("c");
	IHDFFolder *folder3 = folder2->GetFolder("v");
	IHDFFolder *folder4 = folder->GetFolder("c/v");

	const char *nameFolder3 = folder3->GetName();
	const char *nameFolder4 = folder4->GetName();
	cout << folder4->GetCountFolder();

	

	getchar();
	return 1;
}