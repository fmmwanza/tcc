#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <ctype.h>
#include <stdio.h>


#include "core.h"
#include "utils.h"


int main(int argc, char **argv)
{

	int option_index = 0;
	//printf("%s ----------------\n", argv[6]);
	if (argc != 7)
	{
		printf("%s",usage);
		return -1;
	}
	else
	if ((toupper(argv[6][0]) != 'Y') && (toupper(argv[6][0]) != 'N'))
	{
		printf("%s ",usage);
		return -1;
	}


	Core appCore(argc, argv);
	appCore.run();
	printf("Hard work done! ;)\n"); 

    return 0;
}
