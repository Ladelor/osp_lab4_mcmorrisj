//Needed to remove gcc compiler warning for the tryjoin function
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>

#include <pthread.h>

//Pre-pro macro for the number of threads to use
#define THREADS 4

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

	//Create timeval structs to keep track of how long the program takes
	struct timeval startTime, endTime;
	//Function to get current time
	//Params are reference to time val struct (startTime)
	//And some timezone thing that just takes NULL
	gettimeofday(&startTime, NULL);
	//directory details struct pointer for getting dir names
	struct dirent* dirDetails;

	//We have to check the first directory because my function is bad
	//Should have fixed the problem but instead bandaids everywhere
	if(strstr(argv[2], argv[1]) != NULL)
		printf("%s:\n", argv[2]);

	//Going to store each filePath found in our starting directory
	//Our threads will run through the list later
	char* files[500];
	//Int to keep track of how many files we have to go through
	int fileCount = 0;

	//Read through our starting directory and store details of each file found
	//in dirDetails
	while((dirDetails = readdir(dir)))
	{
		//If we found . or .., we don't want to search through them so ignore
		if(strcmp(dirDetails->d_name, ".") != 0 &&
			strcmp(dirDetails->d_name, "..") != 0)
		{
			//Copy the string so we can modify and have what we need
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

	int threadOpen[THREADS];
	pthread_t threads[THREADS];
	for(int i = 0; i < THREADS; i++)
	{
		threadOpen[i] = 1;
	}

	int filesLeft = fileCount;

	struct functionInput** fI = malloc(sizeof(struct functionInput*)
		* THREADS);
	for(int i = 0; i < THREADS; i++)
	{
		fI[i] = (struct functionInput*)malloc(sizeof(struct functionInput));
	}

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
					fI[x]->filePath = files[i];
					fI[x]->searchString = argv[1];
					pthread_create(&threads[x], NULL,
						fileSearch, fI[x]);
					threadOpen[x] = 0;
					files[i] = NULL;
				}
			}
		}
	}

	for(int i = 0; i < THREADS; i ++)
	{
		pthread_join(threads[i], NULL);
		free(fI[i]);
	}
	free(fI);

	for(int i = 0; i < fileCount; i++)
	{
		free(files[i]);
	}
	gettimeofday(&endTime, NULL);
	printf("Time: %ld\n", (endTime.tv_usec) - (startTime.tv_usec));
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
