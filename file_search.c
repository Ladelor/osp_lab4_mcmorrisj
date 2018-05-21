#include <stdio.h>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("Invalid use of file_search: file_search <file_" \
			"search> <starting directory>\n");
		return -1;
	}

	printf("Hello\n");
	return 0;
}
