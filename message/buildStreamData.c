#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void getLengthOfMessages(FILE * instream, FILE * outstream) {
	char line[256];
	char * lptr;
	int msglen = 0;
	int byteoffset = -1;
	
	while (!feof(instream)) {
	    lptr = fgets(line, 255, instream);
	    if (strstr(line, "Sender")!=NULL) {
			if (byteoffset>-1) {
			    printf("byteOffset = %d\n", byteoffset);
			    fprintf(outstream, "%d\n", byteoffset);
			}
	    	msglen = strlen(line);
		    byteoffset += strlen(line);
	    } else if (!feof(instream)){
	    	msglen += strlen(line);
		    byteoffset += strlen(line);
		}
	}
	if (byteoffset>-1) {
	    printf("byteOffset = %d\n", byteoffset);
        fprintf(outstream, "%d\n", byteoffset);
	}
}

int main(int argc, char** argv) {
    FILE *instream = NULL;
    FILE *outstream = NULL;
    char outfile[20];
	
    if (argc!=2) {
        printf("Usage: %s <stream file>\n", *argv);
    } else {
        strcpy(outfile, *(argv+1));
        strcat(outfile, "Data");
        instream = fopen(*(argv+1), "r");
        outstream = fopen(outfile, "w");
        if ((instream != NULL) && (outstream != NULL)) {
			while (!feof(instream)) {
                getLengthOfMessages(instream, outstream);
		    }
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
