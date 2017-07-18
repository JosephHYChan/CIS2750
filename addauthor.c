#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"

bool readStream(char * tag, char * stream) {
    int pos = 0;
    int ch;
    
    printf("%s: ", tag);
    while ((ch= getc(stdin)) != '\n') {
        if ((ch>=32) && (ch<=254)) {
            stream[pos] = ch;
            pos++;
        }
    }
    stream[pos] = '\0';
    return true;
}

bool find(FILE * outstream, char * username) {
    char * entry = malloc(81);
    char * marker = NULL;
    bool result = false;

    fseek(outstream, 0, SEEK_SET);
    marker = fgets(entry, 80, outstream);
    while ((marker != NULL) && (result==false)) {
        result = (strstr(entry, username) != NULL);
        marker = fgets(entry, 80, outstream);
    }
    free(entry);
    return result;
}

void addUser(char * username, char * stream) {
    FILE * outstream = NULL;
    char path[20] = "./message";
    int mode = 0;

    openOneFile(&outstream, &mode, path, stream, "Users", true);
    if (mode == 0) {
        fprintf(outstream, "%s %d\n", username, 0);
    } else if (!find(outstream, username)) {
        fseek(outstream, 0, SEEK_END);
        fprintf(outstream, "%s %d\n", username, 0);
    }
    fclose(outstream);
}

void removeUser(char * username, char * stream) {
    FILE * outstream = NULL;
    FILE * tempstream = NULL;
    char * entry = malloc(81);
    char * marker = NULL;
    char path[20] = "./message";
    char filename[80] = "";
    bool removed =  false;
    int mode = 0;

    strcpy(filename, path);
    strcat(filename, "/");
    strcat(filename, stream);
    strcat(filename, "Users");
    if (openOneFile(&outstream, &mode, path, stream, "Users", false) == true) {
        if ((mode == 1) && find(outstream, username)) {
            removed = true;
            fseek(outstream, 0, SEEK_SET);
            tempstream = fopen("./message/swapFile", "w");
            marker = fgets(entry, 80, outstream);
            while (marker != NULL) {
                if (strstr(entry, username) == NULL) {
                    fprintf(tempstream, "%s", entry);    
                }
                marker = fgets(entry, 80, outstream);
            }
            free(entry);
            fclose(tempstream);
        }
        fclose(outstream);
        if (removed) {
            remove(filename);
            rename("./message/swapFile", filename);
        }
	}
}

void getName(char * username, int offset, int size, char ** passIn) {
    int pos = 0;
    
    while (pos<size) {
        strcat(username, *(passIn+offset+pos));
        strcat(username, " ");
        pos++;
    }
    username[strlen(username)-1] = '\0';
}

int main(int argc, char ** argv) {
    char * token;
    char username[80] = "";
    char streams[132];
    bool addTo = true;

    if (argc < 2) {
        printf("Usage: %s [%s] %s\n", *argv, "-r", "<author-id>");
        exit(0);
    } else if (argc > 2) {
        if (strcmp("-r", *(argv+1))==0) {
            addTo = false;
            getName(username, 2, argc-2, argv);
        } else {
            getName(username, 1, argc-1, argv);
        }
    } else {
        strcpy(username, *(argv+1));
    }
    readStream("list streams", streams);
    token = strtok(streams, ", ");
    while (token != NULL) {
        if (addTo) {
             addUser(username, token);
        } else {
             removeUser(username, token);
        }
        token = strtok(NULL, ", ");
    }
    return 1;
}
