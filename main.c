#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parserphp.h"

int getByteSize(FILE * configFile) {
    int wordCount = 0;

    fgetc(configFile);
    while (!feof(configFile)) {
        wordCount++;
        fgetc(configFile);
    } 
    fseek(configFile, 0, SEEK_SET);
    return wordCount;
}

void readIntoLine(FILE * configFile, char * line) {
    int pos = 0;

    fseek(configFile, 0, SEEK_SET);
    while (!feof(configFile)) {
        *(line+pos) = fgetc(configFile);
        if (!feof(configFile)) {
            pos++;
        }
    } 
    *(line+pos) = '\0';
}

void generateHTML(FILE * configFile, FILE * phpFile) {
    char * line = NULL;
    char tag[MAX_TAG_LENGTH];
    int tagPos;
    int lineLength = getByteSize(configFile)+1;
    
    line = malloc(lineLength);
    
    readIntoLine(configFile, line);
    while ((strlen(line)>0)) {
        if (tagFound(line, &tagPos)==true) {
            passthrough(stdout, line, tagPos);
            getTag(line, tag);
            parse(phpFile, tag, tagPos);
        } else {
            passthrough(stdout, line, strlen(line));
        }
    }
}

int main(int argc, char** argv) {
    FILE *instream = NULL;
    FILE *outstream = NULL;
    char outfile[20];
    char * dot;
    
    if (argc!=2) {
        printf("Usage: %s <configuartion file>\n", *argv);
    } else {
        strcpy(outfile, *(argv+1));
        dot = strstr(outfile,".");
        if (dot != NULL) {
            *dot = '\0';
        }
        strcat(outfile, ".php");
        instream = fopen(*(argv+1), "r");
        outstream = fopen(outfile, "w");
        if ((instream != NULL) && (outstream != NULL)) {
            generateHTML(instream, outstream);
            fclose(instream);
            fclose(outstream);
        } else if (instream != NULL) {
            fclose(instream);
        } else if (outstream != NULL) {
            fclose(outstream);
        } else {
            printf("Error: %s cannot be opened\n", *(argv+1));
        }

    }
    return (EXIT_SUCCESS);
}

