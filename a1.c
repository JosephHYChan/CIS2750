#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

int main(int argc, char** argv) {
    FILE * infile = NULL;
    FILE * outfile = NULL;
    char outFileName[25];
    char * dotAt;
    enum ContextType context = GLOBAL;
    struct ClassType * allClasses = NULL;
    struct DataType  * allNonClassVariables = NULL;
    struct CodeFragments sourceCode = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    if (argc == 3) {
        strcpy(outFileName, *(argv+2));    
    } else if (argc == 2) {
        strcpy(outFileName, *(argv+1));    
        if ((dotAt = strstr(outFileName, ".")) != NULL) {
            *dotAt = '\0';
        }
        strcat(outFileName, ".c");
    } else {
        printf("Usage : %s <C++ file> [C file]\n", *argv);
        exit(0);
    }
    if ((infile = fopen(*(argv+1), "r")) != NULL) {
        parseSourceCode(infile, MAX_CHAR_PER_LINE, &sourceCode, 
                        &allNonClassVariables, &context, &allClasses, false);
        fclose(infile);
    } else {
        printf("%s cannot be read\n", *(argv+1));
        exit(0);
    }
    if ((outfile = fopen(outFileName, "w")) == NULL) {
        outfile = stdout;
        printBlock(outfile, sourceCode.globalBlock);
        printBlock(outfile, sourceCode.classBlock);
        printBlock(outfile, sourceCode.functionBlock);
        printBlock(outfile, sourceCode.constructorDeclarationBlock);
        printBlock(outfile, sourceCode.mainDeclarationBlock);
        printBlock(outfile, sourceCode.constructorCallBlock);
        printBlock(outfile, sourceCode.mainBodyBlock);
        printBlock(outfile, sourceCode.endBlock);
    } else {
        printBlock(outfile, sourceCode.globalBlock);
        printBlock(outfile, sourceCode.classBlock);
        printBlock(outfile, sourceCode.functionBlock);
        printBlock(outfile, sourceCode.constructorDeclarationBlock);
        printBlock(outfile, sourceCode.mainDeclarationBlock);
        printBlock(outfile, sourceCode.constructorCallBlock);
        printBlock(outfile, sourceCode.mainBodyBlock);
        printBlock(outfile, sourceCode.endBlock);
        fclose(outfile);
    }
    freeData(allNonClassVariables);
    freeClass(allClasses);
    freeBlock(sourceCode.globalBlock);
    freeBlock(sourceCode.classBlock);
    freeBlock(sourceCode.functionBlock);
    freeBlock(sourceCode.constructorDeclarationBlock);
    freeBlock(sourceCode.mainDeclarationBlock);
    freeBlock(sourceCode.constructorCallBlock);
    freeBlock(sourceCode.mainBodyBlock);
    freeBlock(sourceCode.endBlock);
    exit(1);
}

