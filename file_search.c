#include <stdio.h>
#include <string.h>
#include <dirent.h>

int fileSearch(char*, char*);

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

	//This makes it not accept only / as an entry
	//You said in response to Robert you didn't care
	if(argv[2][strlen(argv[2]) - 1] == '/')
	{
		printf("Starting Directory must be absolute path\n");
		return -1;
	}

	DIR* dir;
	dir = opendir(argv[2]);
	if(dir == NULL)
	{
		printf("Failed to open starting directory\n");
		return -1;
	}

	fileSearch(argv[2], argv[1]);

	return 0;
}

int fileSearch(char* filePath, char* searchString)
{
	DIR* dir = opendir(filePath);
	if(dir != NULL)
	{
		struct dirent *dirDetails;
		char nameCopy[256];
		while((dirDetails = readdir(dir)))
		{
			if(strcmp(dirDetails->d_name, ".") != 0 &&
				strcmp(dirDetails->d_name, "..") != 0)
			{
				//char nameCopy[256];
				strcpy(nameCopy, filePath);
				strcat(nameCopy, "/");
				strcat(nameCopy, dirDetails->d_name);
				if(strstr(dirDetails->d_name, searchString)
					 != NULL)
				{
					if(opendir(nameCopy) != NULL)
						printf("%s:\n", nameCopy);
					else
						printf("%s\n", nameCopy);
				}
				fileSearch(nameCopy, searchString);
			}
		}
	}
	return 0;
}
