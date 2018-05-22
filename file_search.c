#include <stdio.h>
#include <string.h>
#include <dirent.h>

int fileSearch(char*, char*);

int main(int argc, char* argv[])
{
	//Make sure there are the right number of args
	if(argc != 3)
	{
		printf("Invalid use of file_search: <file_search>" \
		" <starting directory>\n");
		return -1;
	}

	//Make sure initial directory starts with a /
	if(argv[2][0] != '/')
	{
		printf("Starting Directory must be absolute path\n");
		return -1;
	}

	//This makes it not accept only / as an entry
	//You said in response to Robert you didn't care
	//Also this ensures file does not end with /
	if(argv[2][strlen(argv[2]) - 1] == '/')
	{
		printf("Starting Directory must be absolute path\n");
		return -1;
	}

	//Check to make sure initial filePath is a directory
	DIR* dir;
	//opendir attempts to open a directory from a cstring
	//arg is user inputted filepath
	//returns pointer to stream for the directory into dir
	dir = opendir(argv[2]);
	//This means we failed to open filePath as a directory
	if(dir == NULL)
	{
		printf("Failed to open starting directory\n");
		return -1;
	}

	//Start the recursive function
	fileSearch(argv[2], argv[1]);

	return 0;
}

//Function to recursively search a directory for a string
//First param is path to start file
//Second param is string to search for
//Return is not really used
int fileSearch(char* filePath, char* searchString)
{
	//Attempt to open the file as a directory
	//If we fail, it is some other file type
	//Printing is done by the root directory
	//Sort of bad implementation but ohh well
	DIR* dir = opendir(filePath);
	if(dir != NULL)
	{
		//struct to store diretory info as per man page
		struct dirent *dirDetails;
		//char array to manipulate the file names
		char nameCopy[256];
		//Takes a directory stream and returns directory details
		//Goes until there are no more files and it returns null
		while((dirDetails = readdir(dir)))
		{
			//If it is . or .., we don't want it
			if(strcmp(dirDetails->d_name, ".") != 0 &&
				strcmp(dirDetails->d_name, "..") != 0)
			{
				//Modifying the filePath into what I need
				strcpy(nameCopy, filePath);
				strcat(nameCopy, "/");
				strcat(nameCopy, dirDetails->d_name);
				//Check to see if the file name (not path)
				//Contains the searchString
				if(strstr(dirDetails->d_name, searchString)
					 != NULL)
				{
					//Another kinda bad implementation
					//Just try opening the file here too
					//If its a dir, add the :
					if(opendir(nameCopy) != NULL)
						printf("%s:\n", nameCopy);
					else
						printf("%s\n", nameCopy);
				}
				//Recursive call on each file found
				fileSearch(nameCopy, searchString);
			}
		}
	}
	return 0;
}
