#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>

#define _GNU_SOURCE
#include <pthread.h>

#define THREADS 4

void* fileSearchWrapper(void*);
void* fileSearch(void*);
struct functionInput
{
	char* filePath;
	char* searchString;
};

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

	struct timeval startTime, endTime;
	gettimeofday(&startTime, NULL);
	struct dirent* dirDetails;

	int threadOpen[THREADS];
	pthread_t threads[THREADS];
	for(int i = 0; i < THREADS; i++)
	{
		threadOpen[i] = 1;
	}
	if(strstr(argv[2], argv[1]) != NULL)
		printf("%s:\n", argv[2]);
	char* files[500];
	int fileCount = 0;
	while((dirDetails = readdir(dir)))
	{
		if(strcmp(dirDetails->d_name, ".") != 0 &&
			strcmp(dirDetails->d_name, "..") != 0)
		{
			char stringCopy[256];
			strcpy(stringCopy, argv[2]);
			strcat(stringCopy, "/");
			strcat(stringCopy, dirDetails->d_name);
			files[fileCount] = strdup(stringCopy);
			fileCount++;
			if(strstr(dirDetails->d_name, argv[1]) != NULL)
			{
				if(opendir(stringCopy) != NULL)
					printf("%s:\n", stringCopy);
				else
					printf("%s\n", stringCopy);

			}
		}
	}

	int filesLeft = fileCount;
	while(filesLeft > 0)
	{
		for(int i = 0; i < fileCount; i++)
		{
			for(int x = 0; x < THREADS; x++)
			{
				if(threadOpen[x] == 0)
				{
					if(pthread_tryjoin_np(threads[x],
						NULL) == 0)
					{
						filesLeft--;
						threadOpen[x] = 1;
					}
					continue;
				}
				else if(files[i] != NULL)
				{
					struct functionInput* fI = malloc(
						sizeof(struct functionInput));
					fI->filePath = files[i];
					fI->searchString = argv[1];
					pthread_create(&threads[x], NULL,
						fileSearch, fI);
					threadOpen[x] = 0;
					files[i] = NULL;
				}
			}
		}
	}

	for(int i = 0; i > fileCount; i++)
	{
		printf("%s\n", files[i]);
	}

	for(int i = 0; i < THREADS; i ++)
	{
		pthread_join(threads[i], NULL);
	}


	/*
	struct functionInput* fI = malloc(sizeof(struct functionInput));
	fI->filePath = argv[2];
	fI->searchString = argv[1];

	//Start the recursive function
	pthread_t p1;
	pthread_create(&p1, NULL, fileSearchWrapper, fI);
	//fileSearch(argv[2], argv[1]);
	pthread_join(p1, NULL);
//	free(fI);
	*/
	gettimeofday(&endTime, NULL);
	printf("Time: %ld\n", (endTime.tv_usec) - (startTime.tv_usec));
	return 0;
}

void* fileSearchWrapper(void* arg)
{
	struct functionInput* fI = arg;
	if(strstr(fI->filePath, fI->searchString) != NULL)
	{
		printf("%s:\n", fI->filePath);
	}
	fileSearch(arg);
	return 0;
}
//Function to recursively search a directory for a string
//First param is path to start file
//Second param is string to search for
//Return is not really used
void* fileSearch(void* arg)
{
	struct functionInput* fI = arg;
	char* filePath = fI->filePath;
	char* searchString = fI->searchString;
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
				struct functionInput* fI = malloc(sizeof(struct functionInput));
				fI->searchString = searchString;
				fI->filePath = nameCopy;
				fileSearch(fI);
				free(fI);
			}
		}
	}
	return 0;
}
