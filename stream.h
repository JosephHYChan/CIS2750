#ifndef H_STREAM_H
#define H_STREAM_H

struct userPost {
    char *username;
    char *streamname;
    char *postDate;
    char *messageText;
};

void updateStream(struct userPost *st);
void addUser(char *username, char *list);
void removeUser(char *username, char *list);
void freeUserPost(struct userPost * st);
bool openOneFile(FILE ** outstream, int * mode, char * path, char * streamname, char * type, bool allowCreate);

#endif
