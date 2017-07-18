#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"


void getFileName(char * path, char * streamname, char * type, char * filename) {
    strcpy(filename, path);
    strcat(filename, "/");
    strcat(filename, streamname);
    strcat(filename, type);
}

bool fileExist(char * filename) {
    bool result = (access(filename, F_OK)==0);
    return result;    
}

int getLastByte(FILE * streamData) {
    int endByte;
    
    while (!feof(streamData)) {
        fscanf(streamData, "%d", &endByte);
    }
    return endByte;
}

bool openOneFile(FILE ** outstream, int * mode, char * path, char * streamname, char * type, bool allowCreate) {
    char filename[80];
    
    getFileName(path, streamname, type, filename);
    if (!fileExist(filename)) {
	    if (allowCreate) {
            *outstream = fopen(filename, "w");
            *mode = 0;
		} else {
		    return false;
		}
    } else {
        *outstream = fopen(filename, "r+");
        *mode = 1;
    }           
	return true;
}

void closeAllFiles(FILE * outstream[], bool opened[]) {
    int id = 0;

    for (id=0; id<3; id++) {
	    if (opened[id]==true) {
            fclose(outstream[id]);
		}
    }
}

bool openAllFiles(FILE * outstream[], int mode[], char * path, char * streamname, char type[][5], bool allowCreate[]) {
	bool status = true;
	bool opened[3] = {false, false, false};
    int id = 0;

    for (id=0; id<3; id++) {
        opened[id] = openOneFile(&outstream[id], &mode[id], path, streamname, type[id], allowCreate[id]);
		status = status && opened[id];
    }
	if (status==false) {
        closeAllFiles(outstream, opened);
	}
	return status;
}

bool checkPermission(FILE * usersStream, char * username) {
    char * line = malloc(80);
	char * marker;
	bool found = false;
	
	fseek(usersStream, 0, SEEK_SET);
	while ((!found) && (!feof(usersStream))) {
	    marker = fgets(line, 80, usersStream);
		if (marker != NULL) {
		    found = (strstr(line, username) != NULL);
		}
	}	
	free(line);
    return found;
}

void updateStreamFile(FILE * outstream, int mode, struct userPost * st, char * senderTag, char * dateTag) {
    if (mode == 1) {
        fseek(outstream, 0, SEEK_END);
    }
    fprintf(outstream, "%s%s\n", senderTag, st->username);
    fprintf(outstream, "%s%s\n", dateTag, st->postDate);
    fprintf(outstream, "%s", st->messageText);
}

void updateStreamDataFile(FILE * outstream, int mode, int postSize) {
    int lastByte = -1;

    if (mode == 1) {
        lastByte = getLastByte(outstream);
        fseek(outstream, 0, SEEK_END);
    }
    fprintf(outstream, "%d\n", lastByte+postSize);
}

void updateStream(struct userPost *st) {
    FILE * outstream[3];
    char type[3][5] = {"Users", "", "Data"};
	bool allowCreate[3] = {false, true, true};
	bool opened[3] = {true, true, true};
    char path[20] = "./message";
    char senderTag[10] = "Sender: ";
    char dateTag[10] = "Date: ";
    int mode[3];
    int postSize = 0;
    
    postSize += strlen(senderTag)+strlen(st->username)+1;
    postSize += strlen(dateTag)+strlen(st->postDate)+1;
    postSize += strlen(st->messageText);
    if (openAllFiles(outstream, mode, path, st->streamname, type, allowCreate) == true) {
	    if (checkPermission(outstream[0], st->username)==true) {
            updateStreamFile(outstream[1], mode[1], st, senderTag, dateTag);
            updateStreamDataFile(outstream[2], mode[2], postSize);
		} else {
	        printf("Error: Author %s not found in %sUsers and he/she is not authorized to post\n", st->username, st->streamname);
		}
        closeAllFiles(outstream, opened);
	} else {
	    printf("Error: %sUsers does not exist and author cannot be authorized to post\n", st->streamname);
	}
}

void freeUserPost(struct userPost * st) {
    free(st->username);
    free(st->streamname);
    free(st->postDate);
    free(st->messageText);
}
