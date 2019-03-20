#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	IHDFFolder *folder2 = folder->GetFolder("1/2");

	cout << folder2->GetCountFolder();

	

	getchar();
	return 1;
}