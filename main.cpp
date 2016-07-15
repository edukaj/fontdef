#include <iostream>
#include "fontdefgenerator.h"

using namespace std;


int main(int argc, char *argv[])
{
	try
	{
		ProgramOptions po(argc, argv);

		FontdefGenerator fontdefGen{po};

		if (po.showOnlyUsage())
			cout << po << endl;
		else
			fontdefGen.generate();
	}
	catch(const exception& exc)
	{
		cerr << exc.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
