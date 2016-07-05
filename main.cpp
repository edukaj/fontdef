#include <iostream>
#include "font.h"

using namespace std;


int main(int argc, char *argv[])
{
	try
	{
		ProgramOptions po(argc, argv);
		Font{po};
	}
	catch(const exception& exc)
	{
		cerr << exc.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
