#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("Invalid use of file_search: <file_search>" \
		" <starting directory>\n");
		return -1;
	}

	if(argv[2][0] != '/')
	{
		printf("Starting Directory must be absolute path\n");
		return -1;
	}

	if(argv[2][strlen(argv[2]) - 1] == '/')
	{
		printf("Starting Directory must be absolute path\n");
		return -1;
	}

	printf("Hello\n");
	return 0;
}
