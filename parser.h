#ifndef H_PARSER_H
#define H_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedList.h"

#define MAX_CHAR_PER_LINE 256

enum ContextType {
    GLOBAL = 0,
    CLASS,
    COMMENT,
    FUNCTION,
    MAINDECLARATION,
    MAINBODY,
    END
};

char * shiftLeft(char * statement, char indent, int noOfTimes);
void splitLine(char * oneLine, char * marker, int * unmatch, FILE * instream);
bool hasCloseBracket(char * statement, int * unmatch);
bool hasOpenBracket(char * statement, int * unmatch);
bool hasCloseCurlyBracket(char * statement, int * unmatch);
bool hasOpenCurlyBracket(char * statement, int * unmatch);
bool isClassDeclaration(char * statement);
bool isFunctionDeclaration(char * statement);
bool isVariableDeclaration(char * statement);
void parseSourceCode(FILE * instream, int maxCharPerLine, struct CodeFragments * sourceCode, 
                     struct DataType ** allNonClassVariables, enum ContextType * context, 
                     struct ClassType ** allClasses, bool inBlockComment);
#endif
