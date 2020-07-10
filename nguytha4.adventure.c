/* Thanh Nguyen */
/* CS344: Spring 2019 */
/* Assignment 2: Adventure */

/***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pthread.h>
#include<assert.h>

/***************************************************************/

/* Struct to represent rooms */
typedef struct Room {
	char name[12];		/* name of the room */
	char* type[12];		/* type of room; start, mid, end */
	int numOutboundConnections;				/* How many rooms are connected to a given room */
	struct Room* outboundConnections[6];	/* Array of room pointers to represent connections to other rooms */
} Room ;

/***************************************************************/

/* Create a global mutex */
pthread_mutex_t lock; 

/* Create a global variable to signal when it is safe for write_time function to overwrite the time */
int writeTimeSafe = 1;

/***************************************************************/

/* Time thread function to write current time to currentTime.txt */
void * write_time()
{
	/* if it's deemed safe to write the time to the file */
	if(writeTimeSafe == 1) {
		/* lock mutex */
		pthread_mutex_lock(&lock);
		
		/* Variables to hold the local time */
		time_t unfTime;
		struct tm * infoTime;
		char timeString[80];
		
		/* Get the local time, unformatted */
		time(&unfTime);
		
		/* Get information about the local time */
		infoTime = localtime(&unfTime);
		
		/* Format the local time and store it into our string */
		strftime(timeString,80,"%-I:%M%P, %A, %B %d, %Y", infoTime);
			
		/* Variable to hold the name of the file we are writing the time to */
		char * timeFileName = "currentTime.txt";
		
		/* Create a file using the hardcoded file names */
		FILE *fp = fopen(timeFileName, "w");
		
		/* Print the current time to the file */
		fprintf(fp, "%s\n", timeString);
		
		/* Close the file */
		fclose(fp);
		
		/* unlock mutex */
		pthread_mutex_unlock(&lock); 
	}
}

/***************************************************************/

int main() {
	
	/* try to initialize the mutex */
    if (pthread_mutex_init(&lock, NULL) != 0) 
    {
		/* Notify user if mutex init fails */
        printf("\n mutex init has failed\n");

		/* Exit from from program with code of 1 */
        return 1; 
    } 
	
	/* lock mutex */
	pthread_mutex_lock(&lock);
	
	/* thread variable */
	pthread_t thr;
	
	/* variable to hold result of create a thread */
	int thread_result;
	
	/* create time thread */
	thread_result = pthread_create(&thr, NULL, write_time, NULL); 
	
	/***************************************************************/
	
	/* Variable to hold directory with the latest time */
	int latestDirTime = -1;

	/* Variable to hold the string of the directory prefix we'll be using to search */
	char dirPrefix[30] = "nguytha4.rooms.";
	
	/* Variable to hold the resulting directory determined to be the latest */
	char latestDir[256];
	memset(latestDir, '\0', sizeof(latestDir));
	
	/* Variable to hold the current, starting directory */
	DIR * currDir;
	
	/* Variable to hold subdirectory of starting directory */
	struct dirent * subDir;
	
	/* Variable to hold information about subdirectory */
	struct stat dirStats;
	
	/* Open up the current working directory */
	currDir = opendir(".");
	
	/* Check to see if the current directory could be opened */
	if (currDir > 0) {
		
		/* while we still have subdirectories */
		while((subDir = readdir(currDir)) != NULL) {
			
			/* If a match for the prefix is found for the subdirectory */
			if(strstr(subDir->d_name, dirPrefix) != NULL) {
				
				/* Get information about the subdirectory */
				stat(subDir->d_name, &dirStats);
				
				/* If this subdirectory has a later time */
				if((int)dirStats.st_mtime > latestDirTime) {
					
					/* Update the latest directory time */
					latestDirTime = (int)dirStats.st_mtime;
					
					/* Clear out the string for the latest directory name */
					memset(latestDir, '\0', sizeof(latestDir));
					
					/* Copy the name of the directory we found to the latest directory variable */
					strcpy(latestDir, subDir->d_name);
				}	
			}
		}
	}
	
	/* Close the opened directory */
	closedir(currDir);
	
	/***************************************************************/
	
	/* Variable to hold the result of mkdir and chdir */
	int result = 0;
	
	/* Change the working directory to the one just created */
	result = chdir(latestDir);
	
	/***************************************************************/
	
	/* Open the current directory variable to the directory to the latest directory we just found */
	currDir = opendir(".");
	
	/* Create an array of 7 Rooms */
	Room arr_room[7];
	
	/* Variable to hold the index of the room we're working with */
	int roomCtr = 0;
	
	/* Variable to keep track of a struct Room's connection index */
	int connectCtr = 0;
	
	/* Variable to hold the Room file prefix to get only get room files and not directories */
	char roomPrefix[10] = "Room";
	
	/* Variable to hold the name of the Room file */
	char roomFileName[10];
	memset(roomFileName, '\0', sizeof(roomFileName));
	
	/* Variable to hold file from directory */
	struct dirent * fileInDir;
	
	/***************************************************************/
	
	/* Create a variable to hold the strings that we will be reading in from files */
	char fileLine[64];
	memset(fileLine, '\0', sizeof(fileLine));
	
	/* Variable to hold the number of lines a file has */
	int numLine = 0;
	
	/* Variable to hold the number of connections a room has */
	int numConnect = 0;
	
	/* Variable to hold the substrings as we split the read line */
	char * token;
	
	/* Create a file object to read in objects */
	FILE * fp;
	
	/* Create a variable to hold the index of the starting room for later */
	int startIdx = -1;
	
	/* Create a variable to hold the index of the ending room for later */
	int endIdx = -1;
	
	/***************************************************************/
	
	/* Check to see if the current directory could be opened */
	if (currDir > 0) {
		
		/* while we still have room files to read in */
		while((fileInDir = readdir(currDir)) != NULL) {
			
			/* If a match for the prefix is found for the Room file */
			if(strstr(fileInDir->d_name, roomPrefix) != NULL) {
				
				/***************************************************************/
				
				/* Open the Room file for reading */
				fp = fopen(fileInDir->d_name, "r");
				
				/***************************************************************/
				
				/* reset the line counter */
				numLine = 0;
				
				/* while we haven't reached the end of the file */
				while(fgets(fileLine, 64, fp)) {
					/* Increment the line counter */
					numLine = numLine + 1;
				}
				
				/* Subtract 2 from the number of lines to get the number of connections */
				numConnect = numLine - 2;
				
				/* Set the number of connections for the current room */
				arr_room[roomCtr].numOutboundConnections = numConnect;
				
				/***************************************************************/
				
				/* Get the a line of the Room file */
				fgets(fileLine, 64, fp);
				
				/* Run strtok three times, splitting on the space character, to get the room type */
				token = strtok(fileLine, " \n");
				token = strtok(NULL, " \n");
				token = strtok(NULL, " \n");
				
				/* Put the room type into the Room struct */
				stpcpy(arr_room[roomCtr].type, token);
				
				/* if this is the start room, save the roomCtr index as the starting index */
				if(strcmp("start_room", token) == 0) {
					startIdx = roomCtr;
				}
				
				/* if this is the end room, save the roomCtr index as the ending index */
				if(strcmp("end_room", token) == 0) {
					endIdx = roomCtr;
				}	
				
				/***************************************************************/
				
				/* Reset the file counter */
				rewind(fp);
				
				/* Get the a line of the Room file */
				fgets(fileLine, 64, fp);
				
				/* Run strtok three times, splitting on the space character, to get the room name */
				token = strtok(fileLine, " \n");
				token = strtok(NULL, " \n");
				token = strtok(NULL, " \n");
				
				/* Put the room name into the Room struct */
				stpcpy(arr_room[roomCtr].name, token);
				
				/* Close the file we opened for reading */
				fclose(fp);
				
				/***************************************************************/
				
				/* Increment the room counter */
				roomCtr++;
			}
		}
	}
	
	/* Close the opened directory */
	closedir(currDir);
	
	/***************************************************************/
	
	/* Variable to hold the index of the Room whose file we're currently working with */
	int roomIdx = 0;
	
	/* Variable to hold index of room to connect to */
	int connectIdx = 0;
	
	/* Open the current directory again */
	currDir = opendir(".");
	
	/* Check to see if the current directory could be opened */
	if (currDir > 0) {
		
		/* while we still have room files to read in */
		while((fileInDir = readdir(currDir)) != NULL) {
			
			/* If a match for the prefix is found for the Room file */
			if(strstr(fileInDir->d_name, roomPrefix) != NULL) {
				
				/* Open the Room file for reading */
				fp = fopen(fileInDir->d_name, "r");
				
				/***************************************************************/
				
				/* reset the line counter */
				numLine = 0;
				
				/* while we haven't reached the end of the file */
				while(fgets(fileLine, 64, fp)) {
					/* Increment the line counter */
					numLine = numLine + 1;
				}
				
				/* Subtract 2 from the number of lines to get the number of connections */
				numConnect = numLine - 2;
				
				/* Set the number of connections for the current Room file */
				arr_room[roomCtr].numOutboundConnections = numConnect;
				
				/***************************************************************/
				
				/* Reset the file counter */
				rewind(fp);
				
				/* Get the a line of the Room file */
				fgets(fileLine, 64, fp);
				
				/* Run strtok three times, splitting on the space character, to get the room name */
				token = strtok(fileLine, " \n");
				token = strtok(NULL, " \n");
				token = strtok(NULL, " \n");
				
				/* Find the corresponding Room in the array of Rooms */
				for (roomIdx = 0; roomIdx < 7; roomIdx++) {
					if(strcmp(arr_room[roomIdx].name, token) == 0) {
						break;
					}
				}
				
				/***************************************************************/
				
				/* Reset the connection ctr to 0 */
				connectCtr = 0;
				
				/* while we still have connections to process */
				while(numConnect > 0) {
					
					/* Get the a line of the Room file */
					fgets(fileLine, 64, fp);
					
					/* Run strtok three times, splitting on the space character, to get the room name */
					token = strtok(fileLine, " \n");
					token = strtok(NULL, " \n");
					token = strtok(NULL, " \n");
					
					/* Find the corresponding room in the array */
					for (connectIdx = 0; connectIdx < 7; connectIdx++) {
						if(strcmp(arr_room[connectIdx].name, token) == 0)
							break;
					}
					
					/* Add the room as a connection to the main room we're working on */
					arr_room[roomIdx].outboundConnections[connectCtr] = &arr_room[connectIdx];
					
					/* Increment the connection counter */
					connectCtr++;
					
					/* Decrement the number of connections left */
					numConnect--;
				}
				
				/* Close the file we opened for reading */
				fclose(fp);
			}
		}
	}
	
	/* Close the opened directory */
	closedir(currDir);
	
	/***************************************************************/
	
	/* change back to parent directory */
	chdir(".."); 
	
	/* Variable to signal when ending room is found */
	int gameover = 0;
	
	/* Variable to signal when a user inputted a valid room to traverse to */
	int valid = 0;
	
	/* create an array of ints to store the path */
	int pathArr[100];
	
	/* Variable to hold how many steps we're taking */
	int pathCtr = 0;
	
	/* create a string to hold the name of the ending room */
	char * endRoom = arr_room[endIdx].name;
	
	/* variable to hold the index of the current room we are in, initialized to the starting room */
	int currIdx = startIdx;
	
	/* variable to hold the string entered by the user */
	char * lineEntered = NULL;
	
	/* Variables for getline */
	size_t buffersize = 32;
	size_t characters = 0;
	
	/* For loop counters; outer and inner */
	int i = 0;
	int j = 0;
	
	/***************************************************************/
	
	/* while the user's input is not the ending room */
	while(gameover == 0) {
		
		/* Print out the current location to the user */
		printf("CURRENT LOCATION: %s\n", arr_room[currIdx].name);
		
		/* Print out the possible connections to the current location */
		printf("POSSIBLE CONNECTIONS: ");
		for (i = 0; i < arr_room[currIdx].numOutboundConnections; i++) {
			if(i == arr_room[currIdx].numOutboundConnections - 1)
				printf("%s.\n", arr_room[currIdx].outboundConnections[i]->name);
			else
				printf("%s, ", arr_room[currIdx].outboundConnections[i]->name);
		}
		
		/* Ask the user which room they want to go to next */
		printf("WHERE TO? >");
		
		/* Get the user's input and replace the newline with a null terminator */
		characters = getline(&lineEntered, &buffersize, stdin);
		lineEntered[characters - 1] = '\0';
		
		/* print a newline */
		printf("\n");
		
		/***************************************************************/
		
		/* Reset the valid flag to false */
		valid = 0;
		
		/* Check to see if the user asked for the time */
		if(strcmp(lineEntered, "time") == 0) {
			
			/* unlock mutex */
			pthread_mutex_unlock(&lock);
			
			/* join the time thread */
			thread_result = pthread_join(thr, NULL);
			
			/* Open the time file for reading */
			fp = fopen("currentTime.txt", "r");
			
			/* Get the a line of the time file */
			fgets(fileLine, 64, fp);
			
			/* Print the time */
			printf(" %s\n", fileLine);
			
			/* close the time file */
			fclose(fp);
			
			/* lock mutex */
			pthread_mutex_lock(&lock); 
			
			/* create time thread */
			thread_result = pthread_create(&thr, NULL, write_time, NULL);
			
			/* Set the valid flag to true */
			valid = 1;
		}
		/* Proceed with checking to see if the user entered a valid room */
		else {
		
			/* Check to see if the user's input matches the name of a connected room to the current room */
			for (i = 0; i < arr_room[currIdx].numOutboundConnections; i++) {
				
				/* if the selection matches a connected room */
				if(strcmp(lineEntered, arr_room[currIdx].outboundConnections[i]->name) == 0) {
					
					/* Find the room's index in the array of Rooms */
					for (j = 0; j < 7; j++) {
						if(strcmp(lineEntered, arr_room[j].name) == 0) {
							currIdx = j;
							break;
						}
					}
					
					/* If the index matches the index of the ending room, set the gameover flag to true */
					if (currIdx == endIdx)
						gameover = 1;
					
					/* set the valid flag to true */
					valid = 1;
					
					/* Store the index of the room into our int path array */
					pathArr[pathCtr] = currIdx;
					
					/* Increment the path counter */
					pathCtr++;
					
					/* Break out of for loop with new currIdx */ 
					break;
				}
			}
		}
		
		/* If the user's input was deemed invalid */
		if(valid == 0) {
			/* inform the user that their selection was invalid */
			printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			printf("\n");
		}
				
		/***************************************************************/
		
	}
	
	/* Free the memory used by the variable holding the string entered by the user */
	free(lineEntered);
	
	/* Print the congratulatory message to the user and print the path they look */
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", pathCtr);
	
	/* Print all of the rooms that the user went into */
	for (i = 0; i < pathCtr; i++) {
		printf("%s\n", arr_room[pathArr[i]].name);
	}
		
	/***************************************************************/
	
	/* Set the writeTimeSafe variable to false so we don't overwrite the time as this point */
	writeTimeSafe = 0;
	
	/* unlock mutex */
	pthread_mutex_unlock(&lock);
	
	/* join the time thread */
	thread_result = pthread_join(thr, NULL);
	
	/* destroy the mutex */
	pthread_mutex_destroy(&lock); 
	
	/* signal to operating system program ran fine */
    return 0;
}