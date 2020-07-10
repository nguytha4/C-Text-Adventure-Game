/* Thanh Nguyen */
/* CS344: Spring 2019 */
/* Assignment 2: Buildrooms */

/***************************************************************/

#include<stdio.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<unistd.h>

/***************************************************************/

/* Struct to represent rooms */
typedef struct Room {
	char* name;		/* name of the room */
	char* type;		/* type of room; start, mid, end */
	int numOutboundConnections;				/* How many rooms are connected to a given room */
	struct Room* outboundConnections[6];	/* Array of room pointers to represent connections to other rooms */
} Room ;

/***************************************************************/

/* Returns true if all rooms have greater than 3 outbound connections, false otherwise */
int IsGraphFull(Room * rm_arr) {
	
	/* for loop iterator */
	int i = 0;
	
	/* for loop to loop through all 7 rooms */
	for (i = 0; i < 7; i ++) {
		/* if the number of outbound connections for a room is less than 3 */
		if (rm_arr[i].numOutboundConnections < 3)
			return 0;  /* return false */
	}
	
	/* Return true because all rooms have more than 3 outbound connections*/
	return 1;
}

/***************************************************************/

/* Returns a random Room, does NOT validate if connection can be added */
Room * GetRandomRoom(Room * rm_arr) {
	
	/* Get a random number between 0 to 6 */
	int randNum = rand() % 7;
	
	/* Return a random room using the random number as the index for the array of rooms */
	return &rm_arr[randNum];
}

/***************************************************************/

/* Returns true if a connection can be added from Room randRm (< 6 outbound connections), false otherwise */
int CanAddConnectionFrom(Room * randRm) {
	
	/* if the number of outbound connections for the room is less than 6 */
	if (randRm->numOutboundConnections < 6)
		return 1;  /* return true */
	else	/* else */
		return 0;  /* return false */
}

/***************************************************************/

/* Returns true if Rooms rm1 and rm2 are the same Room, false otherwise */
int IsSameRoom(Room * rm1, Room * rm2) {
	
	/* if the name of Room rm1 is equal to Room rm2 */
	if (strcmp(rm1->name, rm2->name) == 0)
		return 1;   /* return true */
	else	/* else */
		return 0;  /* return false */
}

/***************************************************************/

/* Returns true if a connection from Room rm1 to Room rm2 already exists, false otherwise */
int ConnectionAlreadyExists(Room * rm1, Room * rm2) {
	
	/* Get the number of outbound connections for rm1 */
	int numConnect = rm1->numOutboundConnections;
	
	/* for loop counter */
	int i = 0;
	
	/* for loop to iterate through all the connections for room 1 */
	for (i = 0; i < numConnect; i++) {
		/* if a room connected to rm1 has the same name as rm2 */
		if (strcmp(rm1->outboundConnections[i]->name,rm2->name) == 0)
			return 1;	/* return true */
	}
	
	return 0;	/* return false because rm1 doesn't have any connections that match rm2's name */
}

/***************************************************************/
 
/* Connects Rooms rm1 and rm2 together, does not check if this connection is valid */
void ConnectRoom(Room * rm1, Room * rm2) {
	
	/* Get the number of outbound connections for rm1 */
	int numConnect = rm1->numOutboundConnections;
	
	/* Connect rm1 to rm2 using the number of connections as the next available index for the connections array */
	rm1->outboundConnections[numConnect] = rm2;
	
	/* Increment the number of connections for room 1 */
	rm1->numOutboundConnections++;
}

/***************************************************************/

 /* main function */
int main()
{
	
	/* for loop counter */
	int i = 0;
	
	/* Hardcode room names to an array of string pointers */
	char *rmNames[] = {
						"Library",
						"Bedroom",
						"Kitchen",
						"Ballroom",
						"Bathroom",
						"Office",
						"Basement",
						"Attic",
						"Cellar",
						"Dining"
					};
	
	/* Hardcode room types to an array of string pointers */
	char *rmTypes[] = {
						"start_room",
						"mid_room",
						"end_room"
	};
	
	/* Hardcode file room names to an array of string pointers */
	char *fileRmNames[] = {
						"Room1",
						"Room2",
						"Room3",
						"Room4",
						"Room5",
						"Room6",
						"Room7"
					};
					
	/***************************************************************/
		
	/* Create an array to numbers 0-9 that will be shuffled around	*/
	int array[10];
	
	/* fill array with numbers 0-9 */
	for (i = 0; i < 10; i++) { 
		array[i] = i;
	}
	
	/* randomize seed */
	srand(time(NULL));
	
	/* shuffle array */
	for (i = 0; i < 10; i++) {    
		int temp = array[i];				/* create a temp variable to hold the current value of the array that the given index */
		int randomIndex = rand() % 10;		/* get a random number between 0-9 */
		
		array[i] = array[randomIndex];		/* set the value of the array at the current index to the random number */
		array[randomIndex] = temp;			/* swap the previous value with another value in the array using the random number as the index */
	}
	
	/***************************************************************/
	
	/* Create an array of pointers to strings to hold the actual room names used for the project out of the 10 possible */
	char* actRmNames[7];
	
	/* for loop to assign first seven random room names using the shuffled array */
	for (i = 0; i < 7; i++) {
		actRmNames[i] = rmNames[array[i]];
	}
		
	/* Create an array of 7 Rooms */
	Room arr_room[7];
	
	/* Set the name for each room as well as number of connections to 0 */
	for (i = 0; i < 7; i++) {
		arr_room[i].name = actRmNames[i];
		arr_room[i].numOutboundConnections = 0;
	}
	
	/* Set the first two rooms to the start and ending room respectively */
	arr_room[0].type = rmTypes[0];
	arr_room[1].type = rmTypes[2];
	
	/* Set the other 5 rooms to midrooms */
	for (i = 2; i < 7; i++) {
		arr_room[i].type = rmTypes[1];
	}
	
	/***************************************************************/
		
	/* Create all connections in graph */
	while (IsGraphFull(arr_room) == 0) { 
	
		Room * A;	/* Create room pointers to represent randomly picked rooms */
		Room * B;	/* Create room pointers to represent randomly picked rooms */
	
		/* while always true; loop to find a random room that has less than 6 connections */
		while(1 == 1) {
			
			/* Get a random room */
			A = GetRandomRoom(arr_room); 
			
			/* if the room has less than 6 outbound connections, break out of the loop */
			if (CanAddConnectionFrom(A) == 1) {
				break;
			}
		}
		
		/* do-while loop; get a random second room */
		do {
			B = GetRandomRoom(arr_room);
		}
		/* keep going while we get an incompatible room; > 6 connections, A and B are the same room, A and B already connected */
		while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);
		
		ConnectRoom(A, B);	/* Add room B as a connection to room A */
		ConnectRoom(B, A);	/* Add room A as a connection to room B */
	}

	/**********************************************************************************/
	
	/* Create a variable to hold the final name of the directory */
	char dirNameFull[256];
	memset(dirNameFull, '\0', sizeof(dirNameFull));
	
	/* Hardcode the first part of the directory name to a char pointer */
	char * dirName = "nguytha4.rooms.";
	
	/* Get the process id */
	pid_t process_id = getpid();
	
	/* Create the directory name string */
	sprintf(dirNameFull, "%s%d", dirName, process_id);
	
	/* Variable to hold the result of mkdir and chdir */
	int result = 0;
	
	/* Make a directory using the string created for the directory name */
	result = mkdir(dirNameFull, 0755); 
	
	/* Change the working directory to the one just created */
	result = chdir(dirNameFull);
	
	/**********************************************************************************/
	
	/* Variable to hold the number of connections for a room */
	int numConnect = 0;
	
	/* Variable to hold line item counter that actually goes on file; +1 since C is 0-index */
	int lineNum = 0;
	
	/* Inner for loop variable */
	int j = 0;
	
	/* Create 7 Room files to hold information for each room */
	for (i = 0; i < 7; i++) { 
		
		/* Create a file using the hardcoded file names */
		FILE *fp = fopen(fileRmNames[i], "w");
	
		/* Print the "ROOM NAME:" header line with the Room's name */
		fprintf(fp, "ROOM NAME: %s\n", arr_room[i].name);
		
		/* Get the number of connections for the current room */
		numConnect = arr_room[i].numOutboundConnections;
		
		/* Print all of the Room's connections */
		for (j = 0; j < numConnect; j++) {
			lineNum = j + 1;
			fprintf(fp, "CONNECTION %d: %s\n", lineNum, arr_room[i].outboundConnections[j]->name);
		}
		
		/* Print the "ROOM TYPE:" header line with the Room's type */
		fprintf(fp, "ROOM TYPE: %s\n", arr_room[i].type);
	
		/* Close the file */
		fclose(fp);	
		
	}
	
	/**********************************************************************************/
	
	/* signal to operating system program ran fine */
    return 0;
}