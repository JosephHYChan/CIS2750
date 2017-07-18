#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stream.h"

char * readStream(char * tag);
char * readText(char * tag);

struct PostEntry {
    char * stream;
    char * text; 
    char date[81];
    
   void (*PostEntryreadInput)(struct PostEntry *);
    
   void (*PostEntryformatEntrystc)(struct PostEntry *, struct userPost  *, char *);
    
   void (*PostEntrygetTimeDate)(struct PostEntry *);

   void (*PostEntrysubmitPostc)(struct PostEntry *, char *);
};


 void PostEntryreadInput(struct PostEntry * PostEntryReference) {
     PostEntryReference->stream = readStream("stream");
     PostEntryReference->text = readText("enter text");
}
   
 void PostEntryformatEntrystc(struct PostEntry * PostEntryReference, struct userPost * st, char * username) {
     st->username = (char *)malloc(strlen(username)+1);
     st->streamname = (char *)malloc(strlen(PostEntryReference->stream)+1);
     st->postDate = (char *)malloc(strlen(PostEntryReference->date)+1);
     st->messageText = (char *)malloc(strlen(PostEntryReference->text)+1);
     strcpy(st->username, username);
     strcpy(st->streamname, PostEntryReference->stream);
     strcpy(st->postDate, PostEntryReference->date);
     strcpy(st->messageText, PostEntryReference->text);
}
   
 void PostEntrygetTimeDate(struct PostEntry * PostEntryReference) {
     char month[12][5] = {"Jan.", "Feb.", "Mar.", "Apr.", "May", "Jun.", "Jul.", "Aug.", "Sep.", "Oct.", "Nov.", "Dec."}; 
     time_t t = time(NULL);
     struct tm * tm = localtime(&t);
     sprintf(PostEntryReference->date, "%-4s %02d, %d %02d:%02d %s", month[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, (tm->tm_hour>11)?"PM":"AM");
}
   
 void PostEntrysubmitPostc(struct PostEntry * PostEntryReference, char * username) {
     struct userPost * st = malloc(sizeof(struct userPost));
     
     PostEntryReference->PostEntryreadInput(PostEntryReference);
     PostEntryReference->PostEntrygetTimeDate(PostEntryReference);
     PostEntryReference->PostEntryformatEntrystc(PostEntryReference, st, username);
     updateStream(st);
     freeUserPost(st);
     free(st);
     free(PostEntryReference->stream);
     free(PostEntryReference->text);
}
   
char * readStream(char * tag) {
    char * stream;
    int pos = 0;
    int ch;

    stream = malloc(81);
    printf("%s: ", tag);
    while ((ch= getc(stdin)) != '\n') {
        if ((ch>=32) && (ch<=254)) {
            stream[pos] = ch;
            pos++;
        }
    }
    stream[pos] = '\0';
    return stream;
}

char * readText(char * tag) {
    char * text;
    int linecount = 1;
    int pos = 0;
    int ch;
    int chCount = 1;

    printf("%s: ", tag);
    text = malloc(100);
    while ((((ch = getc(stdin)) != -1) && (ch != 4)) || (chCount>1)) {
        if (((ch>=32) && (ch<=254)) || (ch == '\n')) {
            if (ch == '\n') {
                printf("- ");
                linecount++;
                text = realloc(text, linecount * 100);
            }
            text[pos] = ch;
            pos++;
        }
        chCount = (ch == '\n')?1:chCount+1;
    }
    text[pos] = '\0';
    printf("\n");
    return text;
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

void constructorPostEntry(struct PostEntry * PostEntryReference) {
   PostEntryReference->PostEntryreadInput = &PostEntryreadInput;
   PostEntryReference->PostEntryformatEntrystc = &PostEntryformatEntrystc;
   PostEntryReference->PostEntrygetTimeDate = &PostEntrygetTimeDate;
   PostEntryReference->PostEntrysubmitPostc = &PostEntrysubmitPostc;
}

int main(int argc, char ** argv) {
    struct PostEntry postEntry;
    char username[80] = "";

   constructorPostEntry(&postEntry);
    if (argc < 2) {
        printf("Usage: %s %s\n", *argv, "<author-id>");
    } else {
        getName(username, 1, argc-1, argv);
        postEntry.PostEntrysubmitPostc(&postEntry, username);
    }
    return 1;
}
