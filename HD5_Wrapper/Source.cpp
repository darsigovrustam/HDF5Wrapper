#include "hdfclasses.h"

int main()
{
	H5::Exception::dontPrint();
	Storage *storage = new Storage();
	IHDFFolder *folder = storage->Open("qwe.h5");
	IHDFFolder *folder2 = folder->GetFolder("/");

	cout << folder2->GetCountStream();

	

	getchar();
	return 1;
}