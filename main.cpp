#include <iostream>
#include "font.h"

using namespace std;


int main(int argc, char *argv[])
{


	try
	{
		ProgramOptions po(argc, argv);

		if (po.mustPrintHelp())
			cout << po << endl;

		if (po.isVerbose())
			po.printOn(cout) << endl;

		Font f(po);

	}
	catch(const exception& exc)
	{
		cerr << exc.what() << endl;
	}

	return 0;
}
