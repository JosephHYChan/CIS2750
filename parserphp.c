#include "parserphp.h"
#define DEF_HEADING_SIZE 3
#define DEF_HEADING_TEXT "HEADING"
#define DEF_LINK_TEXT "link"
#define INDENT 3

void repeatPrint(FILE * phpFile, char ch, int times) {
    if (times>0) {
        fprintf( phpFile, "%c", ch);
        repeatPrint(phpFile, ch, times-1);
    }    
}

void capitalize(char * word, bool firstOnly) {
    if (*word!='\0') {
        *word = (*word)-'a'+'A';
        if (firstOnly==false) {
            capitalize(word+1,firstOnly);
        }
    }
}

bool is(char * token, char * expected) {
    return (strcmp(token, expected) == 0);
}

bool isAlphabet(char ch) {
    return ((ch >='A') && (ch <= 'Z')) || ((ch >='a') && (ch <= 'z'));
}
  
bool isAlphaNumeric(char ch) {
    return ((ch >='A') && (ch <= 'Z')) || ((ch >='a') && (ch <= 'z'));
}

bool isDigit(char ch) {
    return ((ch >='0') && (ch <= '9'));
}

bool tagFound(char * line, int * atPos) {
    static char keyTags[][5] = {"a(", "b(", "d(", "e(", "f(", "g(", "h(", "i(", 
                                "l(", "o(", "p(", "q(", "r(", "s(", "t(", "ft(",
                                "hd(", "in(", "se("};
    bool found = false;
    char tagPattern[5];
    int noOfKeys = sizeof(keyTags)/sizeof(keyTags[0]);
    int iterations = 3;
    int tries = 0;
    int pos = 0;
    int lfPos = strlen(line);

    if (strstr(line, "\n") != NULL) {
        lfPos = (strstr(line, "\n")-line);
    }
    while (!found && tries<iterations) {
        if (tries==0) {
            strcpy(tagPattern, keyTags[pos]);   
        } else if (tries==1) {
            strcpy(tagPattern, " ");
            strcat(tagPattern, keyTags[pos]);   
        } else {
            strcpy(tagPattern, ".");
            strcat(tagPattern, keyTags[pos]);   
        }
        if(strstr(line, tagPattern) != NULL) {
            *atPos = (strstr(line, tagPattern)-line);
            if (lfPos > (*atPos)) {
                if (tries==0) {
                    found = (*atPos==0);
                } else {
                    if (tries==1) *atPos += 1;
                    found = true;
                }
            }
        }
        pos++;
        if (pos == noOfKeys) {
            tries++;
            pos = 0;
        }
    }
    return found;
}

bool getFile(char * tag, char * text) {
    bool endProperly =  false;
    int pos = 0;
    skipAllSpaces(tag);
    while (*(tag+pos) != '\0' && *(tag+pos) != ')' && *(tag+pos) != ',' && *(tag+pos) != '"') {
        *(text+pos) = *(tag+pos);    
        pos++;
    }
    *(text+pos) = '\0';
    endProperly = (*(tag+pos) == ')' || *(tag+pos) == ',' || *(tag+pos) == '"');
    strcpy(tag, tag+pos);
    return endProperly;
}

bool getText(char * tag, char * text) {
    bool endProperly =  false;
    int spos = 0;
    int tpos = 0;
    skipAllSpaces(tag);
    while (*(tag+spos) != '\0' && (*(tag+spos) != '"' || (*(tag+spos) == '"' && *(tag+spos-1) == '\\'))) {
        if (*(tag+spos)!='\\') {
          *(text+tpos) = *(tag+spos);
          tpos++;
        }
        spos++;
    }
    *(text+tpos) = '\0';
    endProperly = (*(tag+spos) == '"');
    strcpy(tag, tag+spos);
    return endProperly;
}

void getNumber(char * tag, char * token) {
    bool populated =  false;
    int pos = 0;
    skipAllSpaces(tag);
    while (isDigit(*(tag+pos))) {
        *(token+pos) = *(tag+pos);    
        pos++;
        populated = true;    
    }
    if (!populated) {
        *(token+pos) = *(tag+pos);
        pos++;
    }
    *(token+pos) = '\0';
    strcpy(tag, tag+pos);
}

void getToken(char * tag, char * token) {
    bool populated =  false;
    int pos = 0;
    skipAllSpaces(tag);
    while (isAlphabet(*(tag+pos))) {
        *(token+pos) = *(tag+pos);    
        pos++;
        populated = true;    
    }
    if (!populated) {
        *(token+pos) = *(tag+pos);
        pos++;
    }
    *(token+pos) = '\0';
    strcpy(tag, tag+pos);
}

void passthrough(FILE * phpFile, char * line, int noOfChar) {
    int pos = 0;
    
    while (pos<noOfChar && *(line+pos) != '\n' && *(line+pos) != '\0'){
        printf("%c", *(line+pos));
        pos++;
    }
    if (*(line+pos) == '\n') {
        printf("%c", *(line+pos));
        pos++;
    }
    strcpy(line, (line+pos));
    return;
}

bool isDot(char ch) {
    return true;
}

bool isActUpon(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char type[80] = "";
    char text[80] = "";
    char name[80] = "";
    char value[80] = "";
    char exe[80] = "";
    char para[80] = "";
    char result[80] = "";
    int layout = 0;
    int linefeed = 0;
    int count = 0;

    getToken(tag, token);
    if (is(token, ".")) {
        return isActUpon(tag, attribute);
    } else if (is(token, "a")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "condition")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->action = malloc(strlen(token)+1);
                    strcpy(attribute->action, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "exe")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                if (getFile(tag, token)) {
                    attribute->exe = malloc(strlen(token)+1);
                    strcpy(attribute->exe, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "func")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    strcpy(name, token);
                } else {
                    perfect =  false; 
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
                count=2;
            } else if (is(token, "pname")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    strcpy(name, token);
                } else {
                    perfect =  false; 
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
                count++;
            } else if (is(token, "pvalue")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    strcpy(value, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
                count++;
            } else if (is(token, "output")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->value = malloc(strlen(token)+1);
                    strcpy(attribute->value, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
            if (count == 2) {
                insertInput(&(attribute->inputs), layout, linefeed, type, text, name, value, exe, para, result);
                count = 0;
                layout = 0;
                linefeed = 0;
            }
        }
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isButton(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isButton(tag, attribute);
    } else if (is(token, "b")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "name")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                attribute->name = malloc(strlen(token)+1);
                strcpy(attribute->name, token);
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "link")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->link = malloc(strlen(token)+1);
                    strcpy(attribute->link, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isDraw(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isDraw(tag, attribute);
    } else if (is(token, "d")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isExecutable(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isExecutable(tag, attribute);
    } else if (is(token, "e")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "exe")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getFile(tag, token)) {
                    attribute->exe = malloc(strlen(token)+1);
                    strcpy(attribute->exe, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "para")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->text = malloc(strlen(token)+1);
                    strcpy(attribute->text, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "output")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->value = malloc(strlen(token)+1);
                    strcpy(attribute->value, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isFoot(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isFoot(tag, attribute);
    } else if (is(token, "ft")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isForm(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char type[80] = "";
    char text[80] = "";
    char name[80] = "";
    char value[80] = "";
    char exe[80] = "";
    char para[80] = "";
    char result[80] = "";
    int layout = 0;
    int linefeed = 0;
    int count = 0;
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isForm(tag, attribute);
    } else if (is(token, "f")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        if (is(token, "action")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            if (getText(tag, token)) {
                attribute->action = malloc(strlen(token)+1);
                strcpy(attribute->action, token);
            } else {
                perfect =  false;    
            }
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ",");
            while (more) {
                getToken(tag, token);
                if (is(token, "align")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    attribute->align = atoi(token);
                    getToken(tag, token);
                    more = is(token, ",");
                } else if (is(token, "layout")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    layout = atoi(token);
                    getToken(tag, token);
                    more = is(token, ",");
                } else if (is(token, "linefeed")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    linefeed = atoi(token);
                    getToken(tag, token);
                    more = is(token, ",");
                } else if (is(token, "type")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    perfect = perfect && (is(token, "radio") || is(token, "submit") || is(token, "hidden") || is(token, "areain") || is(token, "areaout"));
                    strcpy(type, token);
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    count++;
                } else if (is(token, "text")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(text, token);
                        strcpy(type, "text");
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    count++;
                } else if (is(token, "name")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    strcpy(name, token);
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    count++;
                } else if (is(token, "value")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(value, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    count++;
                } else if (is(token, "exe")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getFile(tag, token)) {
                        attribute->exe = malloc(strlen(token)+1);
                        strcpy(attribute->exe, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                } else if (is(token, "para")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(para, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                } else if (is(token, "output")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(result, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                } else {
                    more = false;
                }
                if (count == 3) {
                    insertInput(&(attribute->inputs), layout, linefeed, type, text, name, value, exe, para, result);
                    strcpy(type, "");
                    strcpy(text, "");
                    strcpy(name, "");
                    strcpy(value, "");
                    strcpy(exe, "");
                    strcpy(para, "");
                    strcpy(result, "");
                    count = 0;
                    layout = 0;
                    linefeed = 0;
                }
            }
            perfect = perfect && mustbe(token, ")");
        } else {
            perfect = false;
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isHead(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isHead(tag, attribute);
    } else if (is(token, "hd")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isHeading(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isHeading(tag, attribute);
    } else if (is(token, "h")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "text")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->text = malloc(strlen(token)+1);
                    strcpy(attribute->text, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "align")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getNumber(tag, token);
                attribute->align = atoi(token);
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "size")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getNumber(tag, token);
                attribute->size = atoi(token);
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
        if (attribute->size == 0) attribute->size = DEF_HEADING_SIZE;
        if (attribute->text == NULL) {
            attribute->text = malloc(strlen(DEF_HEADING_TEXT)+1);
            strcpy(attribute->text, DEF_HEADING_TEXT);
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isInit(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char type[80] = "";
    char text[80] = "";
    char name[80] = "";
    char value[80] = "";
    char exe[80] = "";
    char para[80] = "";
    char result[80] = "";
    int layout = 0;
    int linefeed = 0;
    int count = 0;
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isInit(tag, attribute);
    } else if (is(token, "in")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "name")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    strcpy(name, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
                count++;
            } else if (is(token, "value")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    strcpy(value, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
                count++;
            } else {
                more = false;
            }
            if (count == 2) {
                insertInput(&(attribute->inputs), layout, linefeed, type, text, name, value, exe, para, result);
                count = 0;
                layout = 0;
                linefeed = 0;
            }
        }
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isInput(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char type[80] = "";
    char text[80] = "";
    char name[80] = "";
    char value[80] = "";
    char exe[80] = "";
    char para[80] = "";
    char result[80] = "";
    int layout = 0;
    int linefeed = 0;
    int order[3] = {0,0,0};
    int count = 0;
    int cumulative = 0;
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isInput(tag, attribute);
    } else if (is(token, "i")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        if (is(token, "action")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            if (getText(tag, token)) {
                attribute->action = malloc(strlen(token)+1);
                strcpy(attribute->action, token);
            } else {
                perfect =  false;    
            }
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ",");
            while (more) {
                getToken(tag, token);
                if (is(token, "text")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(text, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    order[0]++;
                    count++;
                } else if (is(token, "name")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    strcpy(name, token);
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    order[1]++;
                    count++;
                } else if (is(token, "value")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(value, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    order[2]++;
                    count++;
                } else {
                    more = false;
                }
                if (count == 3) {
                    perfect = (order[0]==1) && (order[1]==1) && (order[2]==1);
                    if (perfect) {
                        insertInput(&(attribute->inputs), layout, linefeed, type, text, name, value, exe, para, result);
                        count = 0;
                        layout = 0;
                        linefeed = 0;
                        order[0]=0;
                        order[1]=0;
                        order[2]=0;
                        cumulative++;
                    }
                } else if (count==1) {
                    perfect = (order[0]==1) && (order[1]==0) && (order[2]==0);
                } else if (count==2) {
                    perfect = (order[0]==1) && (order[1]==1) && (order[2]==0);
                }
                more = more && perfect;
            }
            perfect = perfect && mustbe(token, ")");
            perfect = perfect && (order[0]==0) && (order[1]==0) && (order[2]==0);
            perfect = perfect && (cumulative>0);
        } else {
            perfect = false;
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isLink(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isLink(tag, attribute);
    } else if (is(token, "l")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "text")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->text = malloc(strlen(token)+1);
                    strcpy(attribute->text, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "link")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->link = malloc(strlen(token)+1);
                    strcpy(attribute->link, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
        if (attribute->text == NULL) {
            attribute->text = malloc(strlen(DEF_LINK_TEXT)+1);
            strcpy(attribute->text, DEF_LINK_TEXT);
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isOpenProc(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char name[80] = "";
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isOpenProc(tag, attribute);
    } else if (is(token, "o")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        if (is(token, "proc")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            if (getText(tag, token)) {
                attribute->exe = malloc(strlen(token)+1);
                strcpy(attribute->exe, token);
            } else {
                perfect =  false;    
            }
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ",");
            while (more) {
                getToken(tag, token);
                if (is(token, "stdin")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(name, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    insertInput(&(attribute->inputs), 0, 0, "", "", name, "", "", "", "");
                } else if (is(token, "para")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        attribute->text = malloc(strlen(token)+1);
                        strcpy(attribute->text, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                } else if (is(token, "stdout")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        attribute->value = malloc(strlen(token)+1);
                        strcpy(attribute->value, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                } else {
                    more = false;
                }
            }
            perfect = perfect && mustbe(token, ")");
        } else {
            perfect = false;
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isPicture(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isPicture(tag, attribute);
    } else if (is(token, "p")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "image")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->image = malloc(strlen(token)+1);
                    strcpy(attribute->image, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "size")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getNumber(tag, token);
                attribute->width = atoi(token);
                getToken(tag, token);
                perfect = perfect && mustbe(token, "x");
                getNumber(tag, token);
                attribute->height = atoi(token);
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
        if (attribute->width == 0) attribute->width = 100;
        if (attribute->height == 0) attribute->height = 100;
    } else {
        perfect = false;
    }
    return perfect;
}

bool isQuery(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char name[80] = "";
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isQuery(tag, attribute);
    } else if (is(token, "q")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        if (is(token, "array")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            attribute->name = malloc(strlen(token)+1);
            strcpy(attribute->name, token);
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ",");
            while (more) {
                getToken(tag, token);
                if (is(token, "key")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    strcpy(name, token);
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    insertInput(&(attribute->inputs), 0, 0, "", "", name, "", "", "", "");
                } else if (is(token, "return")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    attribute->link = malloc(strlen(token)+1);
                    strcpy(attribute->link, token);
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                } else {
                    more = false;
                }
            }
            perfect = perfect && mustbe(token, ")");
        } else {
            perfect = false;
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isRadio(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    int count=0;

    getToken(tag, token);
    if (is(token, ".")) {
        return isRadio(tag, attribute);
    } else if (is(token, "r")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "action")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->action = malloc(strlen(token)+1);
                    strcpy(attribute->action, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "name")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                attribute->name = malloc(strlen(token)+1);
                strcpy(attribute->name, token);
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "value")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    insertButton(&(attribute->buttons), token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
                count++;
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
        perfect = perfect && (count>0);
   } else {
        perfect = false;
    }
    return perfect;
}

bool isSelect(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    char type[80] = "";
    char text[80] = "";
    char name[80] = "";
    char value[80] = "";
    char exe[80] = "";
    char para[80] = "";
    char result[80] = "";
    int layout = 0;
    int linefeed = 0;
    int count = 0;
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isSelect(tag, attribute);
    } else if (is(token, "se")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        if (is(token, "name")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            if (getText(tag, token)) {
                attribute->name = malloc(strlen(token)+1);
                strcpy(attribute->name, token);
            } else {
                perfect =  false;    
            }
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ",");
            while (more) {
                getToken(tag, token);
                if (is(token, "condition")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(text, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    count++;
                } else if (is(token, "value")) {
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "=");
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    if (getText(tag, token)) {
                        strcpy(value, token);
                    } else {
                        perfect =  false;    
                    }
                    getToken(tag, token);
                    perfect = perfect && mustbe(token, "\"");
                    getToken(tag, token);
                    more = is(token, ",");
                    count++;
                } else {
                    more = false;
                }
                if (count == 2) {
                    insertInput(&(attribute->inputs), layout, linefeed, type, text, name, value, exe, para, result);
                    count = 0;
                    layout = 0;
                    linefeed = 0;
                }
            }
            perfect = perfect && mustbe(token, ")");
        } else {
            perfect = false;
        }
    } else {
        perfect = false;
    }
    return perfect;
}

bool isStatement(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    bool more = true;
    char token[80];
    
    getToken(tag, token);
    if (is(token, ".")) {
        return isStatement(tag, attribute);
    } else if (is(token, "s")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        while (more) {
            getToken(tag, token);
            if (is(token, "code")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                if (getText(tag, token)) {
                    attribute->text = malloc(strlen(token)+1);
                    strcpy(attribute->text, token);
                } else {
                    perfect =  false;    
                }
                getToken(tag, token);
                perfect = perfect && mustbe(token, "\"");
                getToken(tag, token);
                more = is(token, ",");
            } else if (is(token, "indent")) {
                getToken(tag, token);
                perfect = perfect && mustbe(token, "=");
                getNumber(tag, token);
                attribute->indent = atoi(token);
                getToken(tag, token);
                more = is(token, ",");
            } else {
                more = false;
            }
        }
        perfect = perfect && mustbe(token, ")");
    } else {
        perfect = false;
    }
    return perfect;
}

bool isText(char * tag, TagAttribute * attribute) {
    bool perfect = true;
    char token[80];

    getToken(tag, token);
    if (is(token, ".")) {
        return isText(tag, attribute);
    } else if (is(token, "t")) {
        getToken(tag, token);
        perfect = perfect && mustbe(token, "(");
        getToken(tag, token);
        if (is(token, "text")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            if (getText(tag, token)) {
                attribute->text = malloc(strlen(token)+1);
                strcpy(attribute->text, token);
            } else {
                perfect =  false;    
            }
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ")");
        } else if (is(token, "file")) {
            getToken(tag, token);
            perfect = perfect && mustbe(token, "=");
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            if (getText(tag, token)) {
                attribute->file = malloc(strlen(token)+1);
                strcpy(attribute->file, token);
            } else {
                perfect =  false;    
            }
            getToken(tag, token);
            perfect = perfect && mustbe(token, "\"");
            getToken(tag, token);
            perfect = perfect && mustbe(token, ")");
        } else {
            perfect =  false;    
        }
    } else {
        perfect =  false;  
    }
    return perfect;
}

bool isActUponTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isActUpon(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isButtonTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isButton(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isDrawTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isDraw(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isExecutableTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isExecutable(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isFootTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isFoot(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isFormTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isForm(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isHeadTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isHead(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isHeadingTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isHeading(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isInitTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isInit(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isInputTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isInput(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isLinkTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isLink(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isPictureTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isPicture(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isOpenProcTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isOpenProc(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isQueryTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isQuery(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isRadioTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isRadio(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isSelectTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isSelect(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isStatementTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isStatement(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool isTextTag(char * tag, TagAttribute * attribute) {
    bool result = false;
    char marker[MAX_TAG_LENGTH];
    strcpy(marker, tag);
    result = isText(tag, attribute);
    strcpy(tag, marker);
    return result;
}

bool mustbe(char * token, char * expected) {
    if (strcmp(token, expected) != 0) {
        fprintf(stderr, "Encounter syntax error, parsing aborts\n");
        return false;
    }
    return true;    
}

void skipAllSpaces(char * line) {
    int pos = 0;
    
    while (*(line+pos)== ' ') {
        pos++;
    }
    strcpy(line, (line+pos));
}

void initialize(TagAttribute * attribute) {
    attribute->inputs = NULL;
    attribute->buttons = NULL;
    attribute->action = NULL;
    attribute->exe = NULL;
    attribute->file = NULL;
    attribute->image = NULL;
    attribute->link = NULL;
    attribute->name = NULL;
    attribute->text = NULL;
    attribute->value = NULL;
    attribute->indent = 0;
    attribute->align = 0;
    attribute->layout = 0;
    attribute->submit = 0;
    attribute->size = 0;
    attribute->width = 0;
    attribute->height = 0;
}

void freeup(TagAttribute * attribute) {
    if (attribute->inputs != NULL) freeInput(attribute->inputs);
    if (attribute->buttons != NULL) freeButton(attribute->buttons);
    if (attribute->action != NULL) free(attribute->action);
    if (attribute->exe != NULL) free(attribute->exe);
    if (attribute->file != NULL) free(attribute->file);
    if (attribute->image != NULL) free(attribute->image);
    if (attribute->link != NULL) free(attribute->link);
    if (attribute->name != NULL) free(attribute->name);
    if (attribute->text != NULL) free(attribute->text);
    if (attribute->value != NULL) free(attribute->value);
}

void getTag(char * line, char * tag) {
    int linePos = 0;
    int tagPos = 0;
    int mismatch = 0;
    while (*(line+linePos) != '\0' && (mismatch || *(line+linePos) != ' ') && (mismatch || *(line+linePos) != '\n')) {
        if (*(line+linePos) != '\n') {
            *(tag+tagPos) = *(line+linePos);
            tagPos++;
        }
        if (*(line+linePos) == '(') mismatch++;
        if (*(line+linePos) == ')') mismatch--;
        linePos++;
    }
    
    *(tag+tagPos) = '\0';
    strcpy(line, (line+linePos));
}

void insertInput(InputAttribute ** root, int layout, int linefeed, char * type, char * text, char * name, char * value, char * exe, char * para, char * result) {
    if (*root == NULL) {
        *root = malloc(sizeof(InputAttribute));
        (*root)->type = malloc(strlen(type)+1);
        (*root)->text = malloc(strlen(text)+1);
        (*root)->name = malloc(strlen(name)+1);
        (*root)->value = malloc(strlen(value)+1);
        (*root)->exe = malloc(strlen(exe)+1);
        (*root)->para = malloc(strlen(para)+1);
        (*root)->result = malloc(strlen(result)+1);
        (*root)->layout = layout;
        (*root)->linefeed = linefeed;
        strcpy((*root)->type, type);
        strcpy((*root)->text, text);
        strcpy((*root)->name, name);
        strcpy((*root)->value, value);
        strcpy((*root)->exe, exe);
        strcpy((*root)->para, para);
        strcpy((*root)->result, result);
        (*root)->next = NULL;
    } else {
        insertInput(&((*root)->next), layout, linefeed, type, text, name, value, exe, para, result);
    }
}

void insertButton (RadioButton ** root, char * value) {
    if (*root == NULL) {
        *root = malloc(sizeof(RadioButton));
        (*root)->value = malloc(strlen(value)+1);
        strcpy((*root)->value, value);
        (*root)->next = NULL;
    } else {
        insertButton(&((*root)->next), value);
    }
}

void insertTag (Tag ** root, enum tagType type, TagAttribute * attribute) {
    RadioButton * button = attribute->buttons;
    InputAttribute * input = attribute->inputs;
    
    if (*root == NULL) {
        (*root) = malloc(sizeof(Tag));
        (*root)->type = type;
        (*root)->attribute.inputs = NULL;    
        (*root)->attribute.buttons = NULL;    
        while (input != NULL) {
            insertInput(&((*root)->attribute.inputs), input->layout, input->linefeed, 
                        input->type, input->text, input->name, input->value, input->exe, 
                        input->para, input->result);
            input = input->next;
        }
        while (button != NULL) {
            insertButton(&((*root)->attribute.buttons), button->value);
            button = button->next;
        }
        if (attribute->action != NULL) {
            (*root)->attribute.action = malloc(strlen(attribute->action)+1);
            strcpy((*root)->attribute.action, attribute->action);
        } else {
            (*root)->attribute.action = NULL;
        }
        if (attribute->name != NULL) {
            (*root)->attribute.name = malloc(strlen(attribute->name)+1);
            strcpy((*root)->attribute.name, attribute->name);
        } else {
            (*root)->attribute.name = NULL;
        }
        if (attribute->text != NULL) {
            (*root)->attribute.text = malloc(strlen(attribute->text)+1);
            strcpy((*root)->attribute.text, attribute->text);
        } else {
            (*root)->attribute.text = NULL;
        }
        if (attribute->link != NULL) {
            (*root)->attribute.link = malloc(strlen(attribute->link)+1);
            strcpy((*root)->attribute.link, attribute->link);
        } else {
            (*root)->attribute.link = NULL;
        }
        if (attribute->file != NULL) {
            (*root)->attribute.file = malloc(strlen(attribute->file)+1);
            strcpy((*root)->attribute.file, attribute->file);
        } else {
            (*root)->attribute.file = NULL;
        }
        if (attribute->image != NULL) {
            (*root)->attribute.image = malloc(strlen(attribute->image)+1);
            strcpy((*root)->attribute.image, attribute->image);
        } else {
            (*root)->attribute.image = NULL;
        }
        if (attribute->value != NULL) {
            (*root)->attribute.value = malloc(strlen(attribute->value)+1);
            strcpy((*root)->attribute.value, attribute->value);
        } else {
            (*root)->attribute.value = NULL;
        }
        if (attribute->exe != NULL) {
            (*root)->attribute.exe = malloc(strlen(attribute->exe)+1);
            strcpy((*root)->attribute.exe, attribute->exe);
        } else {
            (*root)->attribute.exe = NULL;
        }
        (*root)->attribute.indent = attribute->indent;
        (*root)->attribute.align = attribute->align;
        (*root)->attribute.size = attribute->size;
        (*root)->attribute.width = attribute->width;
        (*root)->attribute.height = attribute->height;
        (*root)->next = NULL;
    } else {
        insertTag(&((*root)->next), type, attribute);
    }
}

void freeInput(InputAttribute * root){
    if (root != NULL) {
        freeInput(root->next);
        free(root->type);
        free(root->text);
        free(root->name);
        free(root->value);
        free(root->exe);
        free(root->para);
        free(root->result);
    }
}

void freeButton(RadioButton * root){
    if (root != NULL) {
        freeButton(root->next);
        free(root->value);
    }
}

void freeTag(Tag * root) {
    if (root != NULL) {
        freeTag(root->next);
        freeButton(root->attribute.buttons);
        freeInput(root->attribute.inputs);
        free(root->attribute.action);
        free(root->attribute.exe);
        free(root->attribute.file);
        free(root->attribute.image);
        free(root->attribute.link);
        free(root->attribute.name);
        free(root->attribute.text);
        free(root->attribute.value);
    }
}

void parse(FILE * phpFile, char * tag, int indent) {
    Tag * allTags = NULL;
    TagAttribute attribute;
    
    initialize(&attribute);
    if (isActUponTag(tag, &attribute)) {
        insertTag(&allTags, ACTUPON, &attribute);
        printActUponTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isButtonTag(tag, &attribute)) {
        insertTag(&allTags, BUTTON, &attribute);
        printButtonTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isDrawTag(tag, &attribute)) {
        insertTag(&allTags, DRAW, &attribute);
        printDrawTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isExecutableTag(tag, &attribute)) {
        insertTag(&allTags, EXECUTABLE, &attribute);
        printExecutableTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isFootTag(tag, &attribute)) {
        insertTag(&allTags, FORM, &attribute);
        printFootTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isFormTag(tag, &attribute)) {
        insertTag(&allTags, FORM, &attribute);
        printFormTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isHeadTag(tag, &attribute)) {
        insertTag(&allTags, HEADING, &attribute);
        printHeadTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isHeadingTag(tag, &attribute)) {
        insertTag(&allTags, HEADING, &attribute);
        printHeadingTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isInitTag(tag, &attribute)) {
        insertTag(&allTags, INIT, &attribute);
        printInitTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isInputTag(tag, &attribute)) {
        insertTag(&allTags, INPUT, &attribute);
        printInputTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isLinkTag(tag, &attribute)) {
        insertTag(&allTags, LINK, &attribute);
        printLinkTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isOpenProcTag(tag, &attribute)) {
        insertTag(&allTags, OPENPROC, &attribute);
        printOpenProcTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isPictureTag(tag, &attribute)) {
        insertTag(&allTags, PICTURE, &attribute);
        printPictureTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isQueryTag(tag, &attribute)) {
        insertTag(&allTags, QUERY, &attribute);
        printQueryTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isRadioTag(tag, &attribute)) {
        insertTag(&allTags, RADIO, &attribute);
        printRadioTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isSelectTag(tag, &attribute)) {
        insertTag(&allTags, SELECT, &attribute);
        printSelectTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isStatementTag(tag, &attribute)) {
        insertTag(&allTags, STATEMENT, &attribute);
        printStatementTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else if (isTextTag(tag, &attribute)) {
        insertTag(&allTags, TEXT, &attribute);
        printTextTag(phpFile, allTags->attribute, indent);
        freeTag(allTags);
    } else {
        attribute.text = malloc(strlen(tag)+1);
        strcpy(attribute.text, tag);
        insertTag(&allTags, UNDEFINE, &attribute);
        fprintf(phpFile, "%s\n", allTags->attribute.text);
        freeTag(allTags);
    }
}

Tag * search(Tag * allTags, enum tagType type) {
    if (allTags == NULL) {
        return NULL;
    } else if (allTags->type == type) {
        return allTags;
    } else {
        return search(allTags->next, type);
    }
}

void printActUponTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;

    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<?php\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "if (isset($_POST['%s'])) {\n", attribute.action);
    if (input!=NULL) {
        if (strlen(input->name)>0 && strlen(input->value)>0) {
            repeatPrint(phpFile, ' ', indent+shift+shift);
            fprintf(phpFile, "$para = \"\";\n");
        }
    }
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift+shift);
        if (strlen(input->name)>0 && strlen(input->value)>0) {
            if (input->next==NULL) {
                fprintf(phpFile, "$para = $para.\" %s=\".%s;\n", input->name, input->value);
            } else {
                fprintf(phpFile, "$para = $para.\" %s=\".%s.\",\";\n", input->name, input->value);
            }
        } else if (strlen(input->name)>0) {
            fprintf(phpFile, "%s;\n", input->name);
        }
        input = input->next;
    }
    if (attribute.value != NULL) {
        repeatPrint(phpFile, ' ', indent+shift+shift);
        fprintf(phpFile, "$cmd = $exe.$para;\n");
        repeatPrint(phpFile, ' ', indent+shift+shift);
        fprintf(phpFile, "$_POST['%s'] = shell_exec($cmd);\n", attribute.value);
    }
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "}\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "?>\n");
}

void printButtonTag(FILE * phpFile, TagAttribute attribute, int indent) {
    int shift = INDENT;
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<form method=\"post\" action=\"%s\">\n", attribute.link);
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "<input type=\"submit\" value=\"%s\" />\n", attribute.name);
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "</form>\n");
}

void printDrawTag(FILE * phpFile, TagAttribute attribute, int indent) {
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<hr><br>\n");
}

void printExecutableTag(FILE * phpFile, TagAttribute attribute, int indent) {
    int shift = INDENT;

    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<?php\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$exe= \"%s\";\n", attribute.exe);
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$para = %s;\n", attribute.text);
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$cmd = $exe.\" \\\"\".$para.\"\\\"\";\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "%s = shell_exec($cmd);\n", attribute.value);
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "?>\n");
}

void printFootTag(FILE * phpFile, TagAttribute attribute, int indent) {
    
    repeatPrint(phpFile, ' ', 2*indent);
    fprintf(phpFile, "</body>\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "</html>");
}

void printFormTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;
    int count;
    char lastfieldName[80]= "";
    char label[20];
    bool submitExist = false;
    
    fprintf(phpFile, "<?php\n");
    strcpy(lastfieldName, "");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "function test_input($data) {\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$data = trim($data);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$data = stripslashes($data);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$data = htmlspecialchars($data);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "return $data;\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "}\n\n");
    input = attribute.inputs;
    while (input != NULL) {
        if ((strcmp(input->type, "radio")==0) && (strcmp(input->name, lastfieldName)!=0)) {
            repeatPrint(phpFile, ' ', indent+shift);
            fprintf(phpFile, "if(isset($_POST[\"%s\"])) {\n", input->name);
            repeatPrint(phpFile, ' ', indent+shift+shift);
            fprintf(phpFile, "$%s = test_input($_POST[\"%s\"]);\n", input->name, input->name);
            if ((input->exe != NULL) && (strlen(input->exe)>0)){
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "$exe= \"%s\";\n", input->exe);
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "$para = %s;\n", input->para);
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "$cmd = $exe.\" \\\"\".$para.\"\\\"\";\n");
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "%s = shell_exec($cmd);\n", input->result);
            }
            repeatPrint(phpFile, ' ', indent+shift);
            fprintf(phpFile, "}\n");
        } else if ((strcmp(input->type, "submit")==0) || (strcmp(input->type, "text")==0) || (strcmp(input->type, "areain")==0)) {
            repeatPrint(phpFile, ' ', indent+shift);
            fprintf(phpFile, "if(isset($_POST[\"%s\"])) {\n", input->name);
            repeatPrint(phpFile, ' ', indent+shift+shift);
            fprintf(phpFile, "$%s = test_input($_POST[\"%s\"]);\n", input->name, input->name);
            if ((strcmp(input->type, "submit")==0) && (attribute.exe != NULL) && (strlen(attribute.exe)>0)){
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "$exe= \"%s\";\n", attribute.exe);
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "$para = %s;\n", input->para);
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "$cmd = $exe.\" \\\"\".$para.\"\\\"\";\n");
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "%s = shell_exec($cmd);\n", input->result);
            }
            repeatPrint(phpFile, ' ', indent+shift);
            fprintf(phpFile, "}\n");
        }
        strcpy(lastfieldName, input->name);
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "?>\n\n");
    if (attribute.align==1) {
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "<center>\n");
    }
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<form method=\"post\" action=\"%s\">\n", attribute.action);
    input = attribute.inputs;
    strcpy(lastfieldName, "");
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift);
        if (strcmp(input->type, "radio")==0) {
            fprintf(phpFile, "<input type=\"radio\" name=\"%s\" value=\"%s\" <?php if ($%s == '%s') { echo 'checked=\"checked\"'; }?> /> %s%s\n", input->name, input->value, input->name, input->value, input->value, (input->layout)?"":"<br>");
        } else if (strcmp(input->type, "areaout")==0) {
            fprintf(phpFile, "<textarea rows=\"10\" cols=\"50\"><?php echo %s;?></textarea>%s\n", input->value, (input->layout)?"":"<br>");
        } else if (strcmp(input->type, "areain")==0) {
            strcpy(label, input->name);
            capitalize(label, true);
            fprintf(phpFile, "%s:<br>\n", label);
            repeatPrint(phpFile, ' ', indent+shift);
            fprintf(phpFile, "<textarea name=\"%s\" rows=\"10\" cols=\"50\" value=\"<?php echo %s;?>\"></textarea>%s\n", input->name, input->value, (input->layout)?"":"<br>");
        } else if (strcmp(input->type, "hidden")==0) {
            fprintf(phpFile, "<input type=\"hidden\" name=\"%s\" value=\"<?php echo %s;?>\">%s\n", input->name, input->value, (input->layout)?"":"<br>");
        } else if (strcmp(input->type, "submit")==0) {
            fprintf(phpFile, "<input type=\"submit\" name=\"%s\" value=\"%s\">%s\n", input->name, input->value, (input->layout)?"":"<br>");
            if (strcmp(input->name, "submit")==0) {
                submitExist = true;
            }
        } else {
            fprintf(phpFile, "%s: <input type=\"text\" name=\"%s\" value=\"<?php echo isset($_POST['%s'])?$_POST['%s']:%s;?>\">%s\n", input->text, input->name, input->name, input->name, input->value, (input->layout)?"":"<br>");
        }
        if (input->layout==0) {
            for (count = 0; count < input->linefeed; count++) {
                repeatPrint(phpFile, ' ', indent+shift);
                fprintf(phpFile, "<br>\n");
            }
        }
        strcpy(lastfieldName, input->name);
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "<br>\n");
    if (submitExist==false) {
        repeatPrint(phpFile, ' ', indent+shift);
        fprintf(phpFile, "<input type=\"submit\" name=\"submit\" value=\"Submit\">\n");
    }
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "</form>\n");
}

void printHeadTag(FILE * phpFile, TagAttribute attribute, int indent) {

    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<!DOCTYPE HTML>\n");
    fprintf(phpFile, "<html>\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<head>\n");
    repeatPrint(phpFile, ' ', 2*indent);
    fprintf(phpFile, "<style>\n");
    repeatPrint(phpFile, ' ', 3*indent);
    fprintf(phpFile, ".error {color: #FF0000;}\n");
    repeatPrint(phpFile, ' ', 2*indent);
    fprintf(phpFile, "</style>\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "</head>\n");
    fprintf(phpFile, "\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<body> \n");
}

void printHeadingTag(FILE * phpFile, TagAttribute attribute, int indent) {
    switch (attribute.size) {
        case 1:
            fprintf(phpFile, "%s<h1>%s</h1>%s\n", (attribute.align==1)?"<center>":"", 
                    attribute.text, (attribute.align==1)?"</center>":"");
            break;
        case 2:
            fprintf(phpFile, "%s<h2>%s</h2>%s\n", (attribute.align==1)?"<center>":"", 
                    attribute.text, (attribute.align==1)?"</center>":"");
            break;
        case 3:
            fprintf(phpFile, "%s<h3>%s</h3>%s\n", (attribute.align==1)?"<center>":"", 
                    attribute.text, (attribute.align==1)?"</center>":"");
            break;
        case 4:
            fprintf(phpFile, "%s<h4>%s</h4>%s\n", (attribute.align==1)?"<center>":"", 
                    attribute.text, (attribute.align==1)?"</center>":"");
            break;
        case 5:
            fprintf(phpFile, "%s<h5>%s</h5>%s\n", (attribute.align==1)?"<center>":"", 
                    attribute.text, (attribute.align==1)?"</center>":"");
            break;
        case 6:
            fprintf(phpFile, "%s<h6>%s</h6>%s\n", (attribute.align==1)?"<center>":"", 
                    attribute.text, (attribute.align==1)?"</center>":"");
            break;
    }
}

void printInitTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;
    
    if (input != NULL) {
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "<?php\n"); 
        while (input != NULL) {
            repeatPrint(phpFile, ' ', indent+shift);
            fprintf(phpFile, "%s = %s;\n", input->name, input->value); 
            input=input->next;
        }
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "?>\n"); 
    }
}

void printInputTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;
    
    fprintf(phpFile, "<?php\n");
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift);
        fprintf(phpFile, "$%s = \"\";\n", input->name);
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "function test_input($data) {\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$data = trim($data);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$data = stripslashes($data);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$data = htmlspecialchars($data);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "return $data;\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "}\n\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "if(isset($_POST['submit'])) {\n");
    input = attribute.inputs;
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift+shift);
        fprintf(phpFile, "$%s = test_input($_POST[\"%s\"]);\n", input->name, input->name);
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "}\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "?>\n\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<center>\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "<form method=\"post\" action=\"%s\">\n", attribute.action);
    input = attribute.inputs;
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift);
        fprintf(phpFile, "%s: <input type=\"text\" name=\"%s\" value=\"<?php echo isset($_POST['%s'])?$_POST['%s']:%s;?>\"><br>\n", input->text, input->name, input->name, input->name, input->value);
        repeatPrint(phpFile, ' ', indent+shift);
        fprintf(phpFile, "<br>\n");
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "<br>\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "<input type=\"submit\" name=\"submit\" value=\"Submit\">\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "</form>\n");
}

void printLinkTag(FILE * phpFile, TagAttribute attribute, int indent) {
    fprintf(phpFile, "<a href=\"%s\">%s</a><br>\n", attribute.link, attribute.text);
}

void printOpenProcTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;
    
    fprintf(phpFile, "<?php\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$exe = \"%s\";\n", attribute.exe);
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$cmd = $exe.\" \".%s;\n", attribute.text);
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "if ($cmd !== \"\") {\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$descriptorspec = array(\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "0 => array(\"pipe\", \"r\"),\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "1 => array(\"pipe\", \"w\"),\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "2 => array(\"pipe\", \"w\"));\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "$process = proc_open($cmd, $descriptorspec, $pipes);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "stream_set_blocking($pipes[0], 0);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "stream_set_blocking($pipes[1], 0);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "stream_set_blocking($pipes[2], 0);\n\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "if (is_resource($process)) {\n");
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift+shift+shift);
        fprintf(phpFile, "fwrite($pipes[0], %s);\n", input->name);
        repeatPrint(phpFile, ' ', indent+shift+shift+shift);
        fprintf(phpFile, "fwrite($pipes[0], \"\\n\");\n");
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent+shift+shift+shift);
    fprintf(phpFile, "fclose($pipes[0]);\n");
    if (attribute.value != NULL) {
        repeatPrint(phpFile, ' ', indent+shift+shift+shift);
        fprintf(phpFile, "%s = stream_get_contents($pipes[1]);\n", attribute.value);
        repeatPrint(phpFile, ' ', indent+shift+shift+shift);
        fprintf(phpFile, "echo %s;\n", attribute.value);
    }
    repeatPrint(phpFile, ' ', indent+shift+shift+shift);
    fprintf(phpFile, "fclose($pipes[2]);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift+shift);
    fprintf(phpFile, "fclose($pipes[1]);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift+shift);
    fprintf(phpFile, "$return_value = proc_close($process);\n");
    repeatPrint(phpFile, ' ', indent+shift+shift);
    fprintf(phpFile, "}\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "}\n");
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "?>\n\n");
}

void printPictureTag(FILE * phpFile, TagAttribute attribute, int indent) {
    fprintf(phpFile, "<img src=\"%s\" height=\"%d\" width=\"%d\"><br>\n", attribute.image, attribute.height, attribute.width); 
}

void printQueryTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;
    
    fprintf(phpFile, "<?php\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$passInfo = $_SERVER['QUERY_STRING'];\n");
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "parse_str($passInfo, $%s);\n", attribute.name);
    while (input != NULL) {
        repeatPrint(phpFile, ' ', indent+shift);
        fprintf(phpFile, "$%s = $%s['%s'];\n", input->name, attribute.name, input->name);
        input = input->next;
    }
    repeatPrint(phpFile, ' ', indent+shift);
    fprintf(phpFile, "$return = \"Location: \".$%s;\n", attribute.link);
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "?>\n\n");
}

void printRadioTag(FILE * phpFile, TagAttribute attribute, int indent) {
    RadioButton * button = attribute.buttons;
    bool setDefault = false;
    
    fprintf(phpFile, "<form action=\"%s\">\n", attribute.action);
    while (button != NULL) {
        repeatPrint(phpFile, ' ', indent);
        if (!setDefault) {
            fprintf(phpFile, "  <input type=\"radio\" name=\"%s\" value=\"%s\" checked> %s<br>\n", attribute.name, button->value, button->value);
            setDefault = true;
        } else {
            fprintf(phpFile, "  <input type=\"radio\" name=\"%s\" value=\"%s\"> %s<br>\n", attribute.name, button->value, button->value);
        }
        button = button->next;
    }
    repeatPrint(phpFile, ' ', indent);
    fprintf(phpFile, "</form>\n");
}

void printSelectTag(FILE * phpFile, TagAttribute attribute, int indent) {
    InputAttribute * input = attribute.inputs;
    int shift = INDENT;
    int count = 0;
    
    if (input != NULL) {
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "<?php\n"); 
        while (input != NULL) {
            if (count==0) {
                repeatPrint(phpFile, ' ', indent+shift);
                fprintf(phpFile, "if (%s) {\n", input->text); 
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "%s = %s;\n", attribute.name, input->value); 
            } else {
                repeatPrint(phpFile, ' ', indent+shift);
                fprintf(phpFile, "} else if (%s) {\n", input->text); 
                repeatPrint(phpFile, ' ', indent+shift+shift);
                fprintf(phpFile, "%s = %s;\n", attribute.name, input->value); 
            }
            count++;
            input=input->next;
        }
        repeatPrint(phpFile, ' ', indent+shift);
        fprintf(phpFile, "}\n"); 
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "?>\n"); 
    }
}

void printStatementTag(FILE * phpFile, TagAttribute attribute, int indent) {
    repeatPrint(phpFile, ' ', attribute.indent);
    fprintf(phpFile, "%s\n", attribute.text); 
}

void printTextTag(FILE * phpFile, TagAttribute attribute, int indent) {
    if (attribute.text !=NULL) {
        fprintf(phpFile, "<p>%s</p>\n", attribute.text);
    } else {
        fprintf(phpFile, "<object data=\"/Users/Thomson/Downloads/a1.txt\" type=\"text/plain\"\n");
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "width=\"500\" style=\"height: 300px\">\n");
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "<a href=\"/Users/Thomson/Downloads/a1.txt\">Not Found</a>\n");
        repeatPrint(phpFile, ' ', indent);
        fprintf(phpFile, "</object><br><br>\n");      
    }
}
