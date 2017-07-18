#include "parser.h"

bool printRange(char * statement, int index, int * startQuote, int * endQuote) {
    if (statement != NULL) {
        if (*statement == '\0') {
            return false;
        } else if ((*statement=='\"') && (*startQuote == -1)) {
            *startQuote = index;
            return printRange(statement+1, index+1, startQuote, endQuote);
        } else if ((*statement=='\"') && (*endQuote == -1)) {
            *endQuote = index;
            return true;
        } else {
            return printRange(statement+1, index+1, startQuote, endQuote);
        }
    }
    return false;
}

bool isAlphaNumeric(char character) {
    bool isUpperCaseAlpha = ((character >= 'A') && (character <= 'Z'));
    bool isLowerCaseAlpha = ((character >= 'a') && (character <= 'z'));
    bool isDigit = ((character >= '0') && (character <= '9'));
    
    return (isUpperCaseAlpha==true) || (isLowerCaseAlpha==true) || (isDigit==true);
}

int occurences(char * statement, char * find) {
    char * subString = NULL;
    int count = 0;
    int startPos = 0;
    int endPos = 0;
    
    while (((subString = strstr(statement, find)) != NULL)) {
        startPos = subString - statement;
        endPos = startPos+strlen(find)-1;
        statement += endPos+1;
        count++;
    }
    return count;
}

char * shiftLeft(char * statement, char indent, int noOfTimes) {
    int shiftCount = strlen(statement);
    int count = 0;
    
    while ((*statement != '\0') && (((*statement == ' ') || (*statement == '\t')) && (*statement == indent) && (noOfTimes>0))) {
        for (count=0; count<shiftCount; count++) {
            *(statement+count) = *(statement+count+1);
        }
        noOfTimes--;
    }
    return statement;
}

void leftTrim(char * statement) {
    int shiftCount = strlen(statement);
    int count = 0;
    
    while ((*statement != '\0') && ((*statement == ' ') || (*statement == '\t') || (*statement == '\n'))) {
        for (count=0; count<shiftCount; count++) {
            *(statement+count) = *(statement+count+1);
        }
    }
}

bool isBlankLine(char * statement)
{
    if ((*statement == '\0') || (*statement == '\n') || (*statement == '\r')) {
        return true;
    } else if (*statement == ' ') {
        return isBlankLine(statement+1);
    } else {
        return false;
    }
}

char * substitute(char * statement, char * find, char * replace, bool matchWholeWord, int times) {
    bool proceed = true;
    char * subString = NULL;
    char * remaining = NULL;
    char * newLine = NULL;
    int maximum = occurences(statement, find);
    int offset = 0;
    int startPos = 0;
    int endPos = 0;
    int PreviousPos = 0;
    int NextPos = 0;
    int startQuote = -1;
    int endQuote = -1;
    bool hasPrintRange = false;
    
    if (statement!=NULL) {
        if ((times < 0) || (times >= maximum)) { 
            newLine = malloc(strlen(statement)+maximum*(strlen(replace)-strlen(find))+1);
        } else {
            newLine = malloc(strlen(statement)+times*(strlen(replace)-strlen(find))+1);
        }
        strcpy(newLine, statement);
        while (((subString = strstr(newLine+offset, find)) != NULL) && (times!=0)) {
            startQuote = -1;
            endQuote = -1;
            hasPrintRange = printRange(newLine, 0, &startQuote, &endQuote);
            startPos = subString - newLine;
            endPos = startPos+strlen(find)-1;
            PreviousPos = startPos-1;
            NextPos = endPos+1;
            if (matchWholeWord == true) {
                if ((PreviousPos>=0) && (NextPos<strlen(newLine))) {
                    proceed = !isAlphaNumeric(*(newLine+PreviousPos)) && !isAlphaNumeric(*(newLine+NextPos));
                    if (hasPrintRange) {
                        proceed = proceed && !((*(newLine+PreviousPos)=='%') && (PreviousPos>startQuote) && (PreviousPos<endQuote)) && !((startPos>startQuote) && (startPos<endQuote));
                    }
                } else if ((PreviousPos<0) && (NextPos<strlen(newLine))) {
                    proceed = !isAlphaNumeric(*(newLine+NextPos));
                } else if ((PreviousPos>=0) && (NextPos==strlen(newLine))) {
                    proceed = !isAlphaNumeric(*(newLine+PreviousPos));
                    if (hasPrintRange) {
                        proceed = proceed && !((*(newLine+PreviousPos)=='%') && (PreviousPos>startQuote) && (PreviousPos<endQuote)) && !((startPos>startQuote) && (startPos<endQuote));
                    }
                } else {
                    proceed = true;
                }
            }
            if (proceed==true) {
                remaining = malloc(strlen(newLine+endPos+1)+1);
                strcpy(remaining, newLine+endPos+1);
                *(newLine+startPos) = '\0'; 
                newLine = strcat(newLine, replace);
                newLine = strcat(newLine, remaining);
                offset = strstr(newLine, remaining)-newLine;
                free(remaining);
            } else {
                offset = endPos+1;
            }
        }
        return newLine;
    }
    return NULL;   
}

bool hasCloseBracket(char * statement, int * unmatch)
{
    bool result = false;

    if (*statement == '\0') {
        result = false;
    } else if (*statement == ')') {
        if (unmatch != NULL) *unmatch -= 1;
        result = true || hasCloseBracket(statement+1, unmatch);
    } else {
        result = hasCloseBracket(statement+1, unmatch);
    }
    return result;
}

bool hasOpenBracket(char * statement, int * unmatch)
{
    bool result = false;

    if (*statement == '\0') {
        result = false;
    } else if (*statement == '(') {
        if (unmatch != NULL) *unmatch += 1;
        result = true || hasOpenBracket(statement+1, unmatch);
    } else {
        result = hasOpenBracket(statement+1, unmatch);
    }
    return result;
}

bool hasCloseCurlyBracket(char * statement, int * unmatch) {
    bool result = false;

    if (*statement == '\0') {
        result = false;
    } else if (*statement == '}') {
        if (unmatch != NULL) *unmatch -= 1;
        result = hasCloseCurlyBracket(statement+1, unmatch) || true;
    } else {
        result = hasCloseCurlyBracket(statement+1, unmatch);
    }
    return result;
}

bool hasOpenCurlyBracket(char * statement, int * unmatch) {
    bool result = false;

    if (*statement == '\0') {
        result = false;
    } else if (*statement == '{') {
        if (unmatch != NULL) *unmatch += 1;
        result = true || hasOpenCurlyBracket(statement+1, unmatch);
    } else {
        result = hasOpenCurlyBracket(statement+1, unmatch);
    }
    return result;
}

bool hasSemiColon(char * statement) {
    return (strstr(statement, ";") != NULL);
}

bool isOneOf(char ch, char * delimiter)
{
    if (*delimiter == '\0') {
        return false;   
    } else if (ch == *delimiter) {
        return true;
    } else {
        return isOneOf(ch, delimiter+1);
    }
}

bool isInLineComment(char * statement)
{
    bool result = false;
    char * line = (char *)malloc(strlen(statement)+1);

    strcpy(line, statement);
    if (strstr(line, "//") != NULL) {
        result = true;
    } else if ((strstr(line, "/*") != NULL) && (strstr(line, "*/") != NULL)) {
        result = true;
    }
    free(line);
    return result;
}

bool isBlockCommentStarts(char * statement)
{
    bool result = false;
    char * line = (char *)malloc(strlen(statement)+1);
    strcpy(line, statement);
    leftTrim(line);

    if ((strstr(line, "/*") != NULL) && (strstr(line, "*/") == NULL)) {
        result = true;
    }
    free(line);
    return result;
}

bool isBlockCommentEnds(char * statement)
{
    bool result = false;
    char * line = (char *)malloc(strlen(statement)+1);
    strcpy(line, statement);
    leftTrim(line);

    if ((strstr(line, "/*") == NULL) && (strstr(line, "*/") != NULL)) {
        result = true;
    }
    free(line);
    return result;
}

bool isClassDeclaration(char * statement)
{
    char * line = (char *)malloc(strlen(statement)+1);
    char * token = NULL;
    bool result = false;
    
    
    strcpy(line, statement);
    token = strtok(line, "\t\n ");
    if (token == NULL) {
        result = false;
    } else if (strcmp(token, "class")==0) {
        result = true;
    } else {
        result = false;
    }
    free(line);
    return result;
}

bool isFunctionDeclaration(char * statement)
{
    char * line = (char *)malloc(strlen(statement)+1);
    char * token = NULL;
    bool result = false;
    
    strcpy(line, statement);
    token = strtok(line, "\t* ");
    if (token == NULL) {
        result = false;
    } else if (strcmp(token, "int")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "bool")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "char")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "enum")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "float")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "struct")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "double")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "void")==0) {
        result = hasOpenBracket(statement, NULL);
    } else if (strcmp(token, "FILE")==0) {
        result = hasOpenBracket(statement, NULL);
    } else {
        result = false;
    }
    free(line);
    return result;
}

bool isVariableDefinition(char * statement) 
{
    char * line = (char *)malloc(strlen(statement)+1);
    char * token = NULL;
    bool result = false;

    strcpy(line, statement);
    token = strtok(line, "\t*()[]\n\r{} ");
    while ((token != NULL) && (!result)){ 
        if  ((strcmp(token, "enum")==0) || (strcmp(token, "struct")==0)) {
            result = (!hasSemiColon(statement) && 
                      (strstr(statement, "(") == NULL) && 
                      (strstr(statement, ")") == NULL));
        } else {
            result = false;
        }
        token = strtok(NULL, "\t*()[]\n\r{} ");
    }
    free(line);
    return result;
}

bool isVariableDeclaration(char * statement)
{
    char * line = (char *)malloc(strlen(statement)+1);
    char * token = NULL;
    bool result = false;
    bool done = false;
    
    strcpy(line, statement);
    token = strtok(line, "\t\n* ");
    done = (token == NULL);
    while (done == false) {
        done = true;
        if (strcmp(token, "static")==0) {
            token = strtok(NULL, "\t\n* ");
            done = (token == NULL);
        } else if (strcmp(token, "int")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "bool")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "char")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "enum")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "float")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "struct")==0) {
            result = hasSemiColon(statement);
        } else if (strcmp(token, "double")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "FILE")==0) {
            result = !hasOpenBracket(statement, NULL);
        } else if (strcmp(token, "class")==0) {
            result = hasSemiColon(statement);
        } else {
            result = false;
        }
    }
    free(line);
    return result;
}

bool isMainDeclaration(char * statement)
{
    if (statement != NULL) {
        return (strstr(statement, "main") != NULL);
    } else {
        return false;
    }
}

bool hasFunctionCall(char * statement)
{
    int startQuote = -1;
    int endQuote = -1;
    char * openBracketAt = NULL;
    char * closeBracketAt = NULL;
    bool hasPrintRange = printRange(statement, 0, &startQuote, &endQuote);
    
    openBracketAt = strstr(statement, "(");
    closeBracketAt = strstr(statement, ")");
    
    bool result = ((openBracketAt != NULL) && (closeBracketAt != NULL)) &&
                  ((hasPrintRange == false) || ((openBracketAt-statement)>endQuote));

    return result;
}

bool isFunctionOfStructCall(char * statement)
{
    bool result = ((strstr(statement, "(") != NULL) && (strstr(statement, ")") != NULL)) && 
                  ((strstr(statement, ".") != NULL) || (strstr(statement, "->") != NULL));

    return result;
}

void packOneLine(char * oneLine)
{
    bool previousIsSpace = false;
    int noOfShift = 0;
    int pos = 0;
    int index;
    
    while ((*(oneLine+pos) != '\0') && (*(oneLine+pos) == ' ')) pos++;
    while (*(oneLine+pos) != '\0') {
        if ((previousIsSpace == true) && (*(oneLine+pos) == ' ')) {
            for (index = pos; index < strlen(oneLine); index++) {
                *(oneLine+index) = *(oneLine+index+1);
            }
            noOfShift++;
        }
        previousIsSpace = (*(oneLine+pos) == ' ');
        pos++;
    }
}

char * getClassName(char * statement) {
    char * line = (char *)malloc(strlen(statement)+1);
    char * className = NULL;
    char * token = NULL;
    
    strcpy(line, statement);
    token = strtok(line, "{\t\n\r ");
    token = strtok(NULL, "{\t\n\r ");
    className = (char *)malloc(strlen(token)+1);
    strcpy(className, token);
    while (strtok(NULL, "{\t\n\r ") != NULL);
    free(line);
    return className;
}

void getDataList(struct DataType ** dataType, char * statement, char * dataTypeName, enum PRIMITIVE_TYPE dataTypeID) {
    char * line = malloc(strlen(statement)+1);
    char * replaceLine = NULL;
    char * token = NULL;
    char * equalSign = NULL;
    char * dataNameList = NULL;
    char * openSquareBracket = NULL;
    
    if (strstr(statement, "class") != NULL) {
        line = realloc(line, strlen(statement)+2);
        replaceLine = substitute(statement, "class", "struct", true, 1);
        strcpy(line, replaceLine);
        free(replaceLine);
    } else {
        strcpy(line, statement);
    }
    equalSign = strstr(line, "=");
    packOneLine(line);
    packOneLine(dataTypeName);
    dataNameList = strstr(line, dataTypeName);
    if (dataNameList != NULL) {
        dataNameList += strlen(dataTypeName);
        if (equalSign != NULL) {
            *equalSign = '\0';
        }
        token = strtok(dataNameList, "*,;\n\r ");
        while (token != NULL) {
            openSquareBracket = strstr(token, "[");
            if (openSquareBracket != NULL) {
                *openSquareBracket = '\0';
            }
            insertData(dataType, dataTypeID, dataTypeName, token);
            token = strtok(NULL, "*,;\n\r ");
        }
    }
    free(line);
}   
    
char * getIdentifier(char * statement) {
    const char delimiter[10] = "*,;\t() ";
    char * line = (char *)malloc(strlen(statement)+1);
    char * identifier = (char *)malloc(1);
    char * token = NULL;
    bool quit = false;
    
    strcpy(line, statement);
    token = strtok(line, delimiter);
    if ((strcmp(token, "struct")==0) || (strcmp(token, "enum")==0)) {
        token = strtok(NULL, delimiter);
        token = strtok(NULL, delimiter);
        identifier = realloc(identifier, strlen(token)+1);
        strcpy(identifier, token);
    } else {
        while (!quit) {
            token = strtok(NULL, delimiter);
            if (token != NULL) {
                identifier = realloc(identifier, strlen(token)+1);
                strcpy(identifier, token);
                quit = ((strcmp(token, "void") != 0) &&
                        (strcmp(token, "char") != 0) &&
                        (strcmp(token, "int") != 0) &&
                        (strcmp(token, "short") != 0) &&
                        (strcmp(token, "long") != 0) &&
                        (strcmp(token, "signed") != 0) &&
                        (strcmp(token, "unsigned") != 0) &&
                        (strcmp(token, "float") != 0) &&
                        (strcmp(token, "double") != 0));
            } else {
                quit = true;
            }
        }
    }
    while (strtok(NULL, delimiter) != NULL);
    free(line);
    return identifier;
}
char * readAllAsterix(char * line, int startAt) {
    char * asterix = malloc(strlen(line)+1);
    int pos = 0;
    while ((*(line+startAt) == '*') || (*(line+startAt) == ' ')) {
        *(asterix+pos) = *(line+startAt);
        pos++;          
        startAt++;
    }
    if (*(asterix+pos-1)=='*') {
        *(asterix+pos) = '\0';
    } else {
        *(asterix+pos-1) = '\0';
    }
    return asterix;
}

enum PRIMITIVE_TYPE getDataType(char * statement, char * dataTypeName) {
    enum PRIMITIVE_TYPE dataTypeID = VOID;
    char maybepointer[20];
    char dataType[80] = "";
    char * line = (char *)malloc(strlen(statement)+1);
    char * replaceLine = NULL;
    char * token = NULL;
    char * asterix = NULL;
    int count = 0;
    char * asterixStarts;
    bool done = false;
    
    strcpy(line, statement);
    token = strtok(line, "*\t();, ");
    while (!done) {
        if (strcmp(token, "static")!=0) {
            strcat(dataType, token);
            strcat(dataType," ");
            count++;
        }
        token = strtok(NULL, "*\t();, ");
        done = ((token == NULL) || (count>1));
    }
    if (strstr(dataType, "void")!=NULL) {
        strcpy(dataTypeName, "void");
        dataTypeID = VOID;
    } else if (strstr(dataType, "unsigned char")!=NULL) {
        strcpy(dataTypeName, "unsigned char");
        dataTypeID = UNSIGNED_CHAR;
    } else if (strstr(dataType, "unsigned int")!=NULL) {
        strcpy(dataTypeName, "unsigned int");
        dataTypeID = UNSIGNED_INT;
    } else if (strstr(dataType, "unsigned long")!=NULL) {
        strcpy(dataTypeName, "unsigned long");
        dataTypeID = UNSIGNED_LONG;
    } else if (strstr(dataType, "unsigned short")!=NULL) {
        strcpy(dataTypeName, "unsigned short");
        dataTypeID = UNSIGNED_SHORT;
    } else if (strstr(dataType, "signed char")!=NULL) {
        strcpy(dataTypeName, "signed char");
        dataTypeID = SIGNED_CHAR;
    } else if (strstr(dataType, "signed int")!=NULL) {
        strcpy(dataTypeName, "signed char");
        dataTypeID = SIGNED_INT;
    } else if (strstr(dataType, "signed long")!=NULL) {
        strcpy(dataTypeName, "signed long");
        dataTypeID = SIGNED_LONG;
    } else if (strstr(dataType, "signed short")!=NULL) {
        strcpy(dataTypeName, "signed short");
        dataTypeID = SIGNED_SHORT;
    } else if (strstr(dataType, "long double")!=NULL) {
        strcpy(dataTypeName, "long double");
        dataTypeID = LONG_DOUBLE;
    } else if (strstr(dataType, "bool")!=NULL) {
        strcpy(dataTypeName, "bool");
        dataTypeID = BOOL;
    } else if (strstr(dataType, "char")!=NULL) {
        strcpy(dataTypeName, "char");
        dataTypeID = CHAR;
    } else if (strstr(dataType, "int")!=NULL) {
        strcpy(dataTypeName, "int");
        dataTypeID = INT;
    } else if (strstr(dataType, "short")!=NULL) {
        strcpy(dataTypeName, "short");
        dataTypeID = SHORT;
    } else if (strstr(dataType, "float")!=NULL) {
        strcpy(dataTypeName, "float");
        dataTypeID = FLOAT;
    } else if (strstr(dataType, "double")!=NULL) {
        strcpy(dataTypeName, "double");
        dataTypeID = DOUBLE;
    } else if (strstr(dataType, "struct")!=NULL) {
        strcpy(dataTypeName, dataType);
        dataTypeID = STRUCT;
    } else if (strstr(dataType, "class")!=NULL) {
        replaceLine = substitute(dataType, "class", "struct", true, 1);
        strcpy(dataTypeName, replaceLine);
        dataTypeID = STRUCT;
    } else {
        strcpy(dataTypeName, "void");
        dataTypeID = VOID;
    }
    while (strtok(NULL, "*\t();, ") != NULL);
    asterixStarts = strstr(statement, "*");
    if (asterixStarts != NULL) {
        asterix = readAllAsterix(line, (asterixStarts-statement));
        sprintf(maybepointer, "%s %s", dataTypeName, asterix);
        packOneLine(maybepointer);
        packOneLine(statement);
        if (strstr(statement, maybepointer) != NULL) {
            strcat(dataTypeName, " ");
            strcat(dataTypeName, asterix);
        }
        free(asterix);
    }
    free(line);
    free(replaceLine);
    return dataTypeID;
}

void getParameterList(struct DataType ** parameters, char * statement) {
    enum PRIMITIVE_TYPE dataTypeID = VOID;
    char dataTypeName[80] = "";
    char * line = (char *)malloc(strlen(statement)+1);
    char * parameter = NULL;
    char * separator = NULL;
    int paraStartPos = 0;
    int paraEndPos = 0;
    
    paraStartPos = strstr(statement, "(")-statement+1;
    paraEndPos = strstr(statement, ")")-statement-1;
    
    if (paraStartPos > paraEndPos) {
        *line = '\0';
    } else {
        strcpy(line, statement+paraStartPos);
        *(line+paraEndPos-paraStartPos+1) = '\0';
    }
    parameter = line;
    if (strlen(parameter) != 0) {
        while ((separator = strstr(parameter, ",")) != NULL) {
            *separator = '\0';
            dataTypeID = getDataType(parameter, dataTypeName);
            strcpy(parameter, parameter+strlen(dataTypeName));
            leftTrim(parameter);
            insertData(parameters, dataTypeID, dataTypeName, parameter);
            strcpy(parameter, separator+1);
            leftTrim(parameter);
        }
        dataTypeID = getDataType(parameter, dataTypeName);
        strcpy(parameter, parameter+strlen(dataTypeName));
        leftTrim(parameter);
        insertData(parameters, dataTypeID, dataTypeName, parameter);
    }
    free(line);
}

void splitLine(char * oneLine, char * marker, int * unmatch, FILE * instream) {
    char * startSplitPos = strstr(oneLine, marker);
    int noOfmarker=0;
    int index;

    if (startSplitPos != NULL) {
        hasCloseCurlyBracket(oneLine, &noOfmarker);
        *unmatch=*unmatch-(noOfmarker+1);
        for (index = strlen(oneLine)-1; index > (startSplitPos-oneLine); index--) {
            ungetc(*(oneLine+index), instream);
        }
        *(startSplitPos+1) = '\0';
    }
}

char * readOneLine(char * oneLine, int maxCharPerLine, char * indent, FILE * instream)
{
    oneLine = fgets(oneLine, maxCharPerLine, instream);
    if ((oneLine != NULL) && (*(oneLine+strlen(oneLine)-1)=='\n')) {
        packOneLine(oneLine);
        *indent = *oneLine;
    }
    return oneLine;
}

void readUntil(char * oneLine, char * marker, int maxCharPerLine, FILE * instream)
{
    char * nextLine = malloc(maxCharPerLine+1);
    char indent;

    while (strstr(oneLine, marker)==NULL) {
        nextLine = readOneLine(nextLine, maxCharPerLine, &indent, instream);
        if (strlen(oneLine)+strlen(nextLine)>maxCharPerLine) {
            oneLine = realloc(oneLine, strlen(oneLine)+maxCharPerLine+1);
        }
        strcat(oneLine, nextLine);
    }
    free(nextLine);
}

char * getPostFix(struct DataType * parameters) {
    char * paraTypeList = malloc(strlen("void")+1);
   
    *paraTypeList = '\0';
    while (parameters != NULL) {
        switch (parameters->dataType) {
            case VOID :
                break;
            case BOOL :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "b");
                break;
            case CHAR :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "c");
                break;
            case INT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "i");
                break;
            case LONG :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "l");
                break;
            case SHORT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "s");
                break;
            case FLOAT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "f");
                break;
            case DOUBLE :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+2); 
                strcat(paraTypeList, "d");
                break;
            case SIGNED_CHAR :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "sc");
                break;
            case SIGNED_INT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "si");
                break;
            case SIGNED_LONG :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "sl");
                break;
            case SIGNED_SHORT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "ss");
                break;
            case UNSIGNED_CHAR :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "uc");
                break;
            case UNSIGNED_INT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "ui");
                break;
            case UNSIGNED_LONG :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "ul");
                break;
            case UNSIGNED_SHORT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "us");
                break;
            case LONG_DOUBLE :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "ld");
                break;
            case STRUCT :
                paraTypeList = realloc(paraTypeList, strlen(paraTypeList)+3); 
                strcat(paraTypeList, "st");
                break;
            default:
                strcat(paraTypeList, "void");
        }
        parameters =parameters->next;
    }
    return paraTypeList;
}

void createConstructorCalls(struct ClassType * allClasses, struct BlockType ** constructorCalls, struct StructType ** identifierToClassMap, char * statement) {
    char * line = (char *)malloc(strlen(statement)+1);
    char * className = getClassName(statement);
    char * constructorDeclaration = malloc(strlen(className)+strlen("constructor(&);\n")+1);
    struct ClassType * currentClass = locateClass(allClasses, className);
    char * variable = NULL;
    char * separator = NULL;
    char indent = ' ';
    int paraStartPos = 0;
    int paraEndPos = 0;
    
    if ((currentClass != NULL) && (currentClass->functions!=NULL)) {
        paraStartPos = strstr(statement, className)-statement+strlen(className);
        paraEndPos = strstr(statement, ";")-statement-1;
    
        if (paraStartPos > paraEndPos) {
            *line = '\0';;
        } else {
            strcpy(line, statement+paraStartPos);
            *(line+paraEndPos-paraStartPos+1) = '\0';
        }
        variable = line;
        if (strlen(variable)>0) {
            while ((separator = strstr(variable, ",")) != NULL) {
                *separator = '\0';
                constructorDeclaration = realloc(constructorDeclaration, strlen(className)+strlen("constructor(&);\n")+strlen(variable)+1);
                strcpy(constructorDeclaration, "constructor");
                strcat(constructorDeclaration, className);
                strcat(constructorDeclaration, "(&");
                leftTrim(variable);
                strcat(constructorDeclaration, variable);
                strcat(constructorDeclaration, ");\n");
                insertBlock(constructorCalls, constructorDeclaration, 1, indent);
                insertStruct(identifierToClassMap, variable, currentClass);
                strcpy(variable, separator+1);
                leftTrim(variable);
            }
            constructorDeclaration = realloc(constructorDeclaration, strlen(className)+strlen("constructor(&);\n")+strlen(variable)+1);
            strcpy(constructorDeclaration, "constructor");
            strcat(constructorDeclaration, className);
            strcat(constructorDeclaration, "(&");
            leftTrim(variable);
            strcat(constructorDeclaration, variable);
            strcat(constructorDeclaration, ");\n");
            insertBlock(constructorCalls, constructorDeclaration, 1, indent);
            insertStruct(identifierToClassMap, variable, currentClass);
        }
    }
    free(line);
    free(className);
    free(constructorDeclaration);
}

void updateSiblingFunctionCall(char * statement, char ** replaceLine, struct FunctionType * allFunctions, struct DataType * allData, char * className, char * myName)
{
    struct FunctionType * current = allFunctions;
    struct DataType * parameters = NULL;
    struct DataType * data = NULL;
    enum PRIMITIVE_TYPE dataTypeID = VOID;
    char dataTypeName[80];
    char modifyName[80];
    char find[80];
    char replace[80];
    char paraSet[80];
    char * markLine = NULL;
    char * arguments = NULL;
    char * parameter = NULL;
    char * separator = NULL;
    char * postFix = NULL;
    char * OpenBracketAt = NULL;
    char * CloseBracketAt = NULL; 
    char * findFunction = NULL;
    bool needComma = false;
    
    *replaceLine = malloc(strlen(statement)+1);
    strcpy(*replaceLine, statement);
    while (current != NULL) {
        findFunction = strstr(statement, current->originalName);
        while (findFunction != NULL) {
            OpenBracketAt = strstr(findFunction, "(");
            CloseBracketAt = strstr(findFunction, ")");
            strcpy(paraSet, OpenBracketAt+1);
            paraSet[CloseBracketAt-OpenBracketAt-1] = '\0';
            if (OpenBracketAt<CloseBracketAt-1) {
                arguments = malloc(CloseBracketAt-OpenBracketAt);
                strncpy(arguments, OpenBracketAt+1, CloseBracketAt-OpenBracketAt-1);
                *(arguments+(CloseBracketAt-OpenBracketAt-1)) = '\0';
                parameter = arguments;
                while ((separator = strstr(parameter, ",")) != NULL) {
                    *separator = '\0';
                    data = locateData(locateFunction(allFunctions,myName)->parameters, parameter);
                    if (data == NULL) {
                        data = locateData(locateFunction(allFunctions,myName)->variables, parameter);
                    }
                    if (data == NULL) {
                        data = locateData(allData, parameter);
                    }
                    if (data != NULL) {
                        dataTypeID = data->dataType;
                        strcpy(dataTypeName, data->dataTypeName);
                        leftTrim(parameter);
                        insertData(&parameters, dataTypeID, dataTypeName, parameter);
                        strcpy(parameter, separator+1);
                        leftTrim(parameter);
                    }
                }
                data = locateData(locateFunction(allFunctions,myName)->parameters, parameter);
                if (data == NULL) {
                    data = locateData(locateFunction(allFunctions,myName)->variables, parameter);
                }
                if (data == NULL) {
                    data = locateData(allData, parameter);
                }
                if (data != NULL) {
                    dataTypeID = data->dataType;
                    strcpy(dataTypeName, data->dataTypeName);
                    leftTrim(parameter);
                    insertData(&parameters, dataTypeID, dataTypeName, parameter);
                }
                free(arguments);
            }
            postFix = getPostFix(parameters);
            strcpy(modifyName, className);
            strcat(modifyName, current->originalName);
            strcat(modifyName, postFix);
            if(strlen(postFix)>0) {
                needComma = true;
            }
            free(postFix);
            freeData(parameters);
            if (strcmp(modifyName, current->identifier) == 0) {
                strcpy(replace, className);
                strcat(replace, "Reference->");
                strcat(replace, modifyName);
                strcat(replace, "(");
                strcat(replace, className);
                if (needComma) {
                    strcat(replace, "Reference, ");
                } else {
                    strcat(replace, "Reference");
                }
                strcpy(find, current->originalName);
                strcat(find, "(");
                markLine = *replaceLine;
                *replaceLine = substitute(*replaceLine, find, replace, false, -1);
                free(markLine);
            }
            statement = CloseBracketAt+1;
            findFunction = strstr(statement, current->originalName);
        }
        current = current->next;
    }
}

char * convertFunctionCall(char * statement, struct DataType * allVariables, struct StructType * identifierToClassMap) {
    struct FunctionCall * Keyword = NULL;
    struct DataType * parameters = NULL;
    enum PRIMITIVE_TYPE dataTypeID = VOID;
    char dataTypeName[80] = "";
    char * line = NULL;
    char * remain = NULL;
    char * postFix = NULL;
    char * replaceLine = NULL;
    char * returnLine = NULL; 
    char * KeywordAt = NULL;
    char * arguments = NULL;
    char * parameter = NULL;
    char * separator = NULL;
    char * replaceCall = NULL;
    char * OpenBracketAt; 
    char * CloseBracketAt; 
    char * PeriodAt;
    char findKey[80];
    char replaceKey[80];
    char structName[80];
    char paraSet[80];
    
    returnLine = malloc(strlen(statement)+1);
    strcpy(returnLine, statement);
    while (identifierToClassMap != NULL) {
        Keyword =  identifierToClassMap->keys;   
        while (Keyword != NULL) {
            line = malloc(strlen(returnLine)+1);
            strcpy(line, returnLine);
            remain = line;
            while ((KeywordAt = strstr(remain, Keyword->functionCall)) != NULL) {
                OpenBracketAt = strstr(KeywordAt, "(");
                CloseBracketAt = strstr(KeywordAt, ")");
                PeriodAt = strstr(Keyword->functionCall, ".");
                strcpy(structName, Keyword->functionCall);
                structName[PeriodAt-Keyword->functionCall] = '\0';
                strcpy(paraSet, OpenBracketAt+1);
                paraSet[CloseBracketAt-OpenBracketAt-1] = '\0';
                if (OpenBracketAt<CloseBracketAt-1) {
                    arguments = malloc(CloseBracketAt-OpenBracketAt);
                    strncpy(arguments, OpenBracketAt+1, CloseBracketAt-OpenBracketAt-1);
                    *(arguments+(CloseBracketAt-OpenBracketAt-1)) = '\0';
                    parameter = arguments;
                    while ((separator = strstr(parameter, ",")) != NULL) {
                        *separator = '\0';
                        dataTypeID = locateData(allVariables, parameter)->dataType;
                        strcpy(dataTypeName, locateData(allVariables, parameter)->dataTypeName);
                        leftTrim(parameter);
                        insertData(&parameters, dataTypeID, dataTypeName, parameter);
                        strcpy(parameter, separator+1);
                        leftTrim(parameter);
                    }
                    dataTypeID = locateData(allVariables, parameter)->dataType;
                    strcpy(dataTypeName, locateData(allVariables, parameter)->dataTypeName);
                    leftTrim(parameter);
                    insertData(&parameters, dataTypeID, dataTypeName, parameter);
                    postFix = getPostFix(parameters);
                    replaceCall = malloc(strlen(Keyword->replaceCall)+strlen(postFix)+1);
                    strcpy(replaceCall, Keyword->replaceCall);
                    strcat(replaceCall, postFix);
                    strcpy(findKey, Keyword->functionCall);
                    strcat(findKey, "(");
                    strcat(findKey, paraSet);
                    strcat(findKey, ")");
                    strcpy(replaceKey, replaceCall);
                    strcat(replaceKey, "(&");     
                    strcat(replaceKey, structName);
                    strcat(replaceKey, ", ");
                    strcat(replaceKey, paraSet);
                    strcat(replaceKey, ")");
                    free(postFix);
                    free(arguments);
                    freeData(parameters);
                } else {
                    replaceCall = malloc(strlen(Keyword->replaceCall)+1);
                    strcpy(replaceCall, Keyword->replaceCall);
                    strcpy(findKey, Keyword->functionCall);
                    strcat(findKey, "()");
                    strcpy(replaceKey, replaceCall);
                    strcat(replaceKey, "(&");     
                    strcat(replaceKey, structName);
                    strcat(replaceKey, ")");
                }
                replaceLine = substitute(returnLine, findKey, replaceKey, true, 1);
                returnLine = realloc(returnLine,strlen(replaceLine)+1);
                strcpy(returnLine, replaceLine);
                free(replaceCall);
                free(replaceLine);
                remain = CloseBracketAt+1;
            }
            free(line);
            Keyword = Keyword->next;
        }
        identifierToClassMap = identifierToClassMap->next;
    }
    return returnLine;
}

void modifyParameters(char * statement, char ** replaceLine, char *className) {
    char * structPointer = malloc(strlen("struct * ") + strlen(className) + strlen(" * ") + strlen(className) + strlen("Reference"));
    char * line = NULL;
    char * parameters = NULL;
    char * startOpenBracket = NULL;
    char * endOpenBracket = NULL;
    char * currentPos = NULL;
    char * find = NULL;
    char * replace = NULL;
    bool isBlank = true;

    strcpy(structPointer, "struct ");
    strcat(structPointer, className);
    strcat(structPointer, " * ");
    strcat(structPointer, className);
    strcat(structPointer, "Reference");

    startOpenBracket = strstr(statement, "(");
    endOpenBracket = strstr(statement, ")");

    find = malloc(endOpenBracket-startOpenBracket+1+1);
    strncpy(find, startOpenBracket, endOpenBracket-startOpenBracket+1);
    *(find+(endOpenBracket-startOpenBracket)+1) = '\0';

    if ((endOpenBracket-startOpenBracket)>1) {
        for (currentPos = startOpenBracket+1; currentPos < endOpenBracket; currentPos++) {
            isBlank = isBlank && ((*currentPos == ' ') || (*currentPos == '\t'));
        }
        parameters = malloc((endOpenBracket-startOpenBracket));
        strncpy(parameters, startOpenBracket+1, (int)(endOpenBracket-startOpenBracket)-1);
        *(parameters+(endOpenBracket-startOpenBracket)-1) = '\0';
    }
    if (isBlank) {
        replace = malloc(strlen(structPointer)+strlen("()")+1);
        strcpy(replace, "(");
        strcat(replace, structPointer);
        strcat(replace, ")");
    } else {
        replace = malloc(strlen(structPointer)+strlen("(, )")+strlen(parameters)+1);
        strcpy(replace, "(");
        strcat(replace, structPointer);
        strcat(replace, ", ");
        strcat(replace, parameters);
        strcat(replace, ")");
    }
    *replaceLine = realloc(*replaceLine, strlen(*replaceLine)+strlen(replace)-strlen(find)+1);
    line = substitute(*replaceLine, find, replace, false, 1);
    strcpy(*replaceLine, line);
    free(line);
    free(find);
    free(replace);
    free(parameters);
    free(structPointer);
}     

void addStructReference(char * statement, char ** replaceLine, struct DataType * allData, struct FunctionType * allFunctions, char * className, char * functionName) {
    struct FunctionType * function = locateFunction(allFunctions, functionName);
    char * line = NULL;
    char * refName = NULL;
    
    *replaceLine = malloc(strlen(statement)+1);
    strcpy(*replaceLine, statement);
    while (allData != NULL) {
        if (locateData(function->parameters, allData->identifier) == NULL) {
            if (locateData(function->variables, allData->identifier) == NULL) {
                refName = malloc(strlen(className)+strlen("Reference->")+strlen(allData->identifier)+1);
                strcpy(refName, className);
                strcat(refName, "Reference->");
                strcat(refName, allData->identifier);
            
                line = substitute(*replaceLine, allData->identifier, refName, true, -1);
                *replaceLine = realloc(*replaceLine, strlen(line)+1);
                strcpy(*replaceLine, line);
                free(refName);
                free(line);
            }
        }
        allData = allData->next;
    }
}

void parseComplexVariableDef(FILE * instream, int maxCharPerLine, 
                             struct CodeFragments * sourceCode, enum ContextType context) {
    char * oneLine = (char *)malloc(maxCharPerLine+1);
    char indent;

    if (readOneLine(oneLine, maxCharPerLine, &indent, instream) != NULL) {
        switch (context) {
            case CLASS:
                insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
                break;
            case FUNCTION:
                insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                break;
            case MAINDECLARATION:
                insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
                break;
            default:
                insertBlock((struct BlockType **)&(sourceCode->globalBlock), oneLine, 0, indent);
        }
        if (!hasCloseCurlyBracket(oneLine, NULL)) {
            parseComplexVariableDef(instream, maxCharPerLine, sourceCode, context);
        }
    } 
    free(oneLine);   
}

void parseFunctionBody(FILE * instream, int maxCharPerLine, struct CodeFragments * sourceCode, 
                       struct DataType * allClassData, struct FunctionType * allFunctions, 
                       char * className, char * functionName, int * unmatch, bool inBlockComment) {
    enum PRIMITIVE_TYPE dataType;
    char * oneLine = (char *)malloc(maxCharPerLine+1);
    char * replaceLine = NULL;
    char dataTypeName[80];
    char indent;
    bool openCurlyBracketFound = false;
    bool closeCurlyBracketFound = false;
    struct FunctionType * me = NULL;
    
    if (readOneLine(oneLine, maxCharPerLine, &indent, instream) != NULL) {
        oneLine = shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE);
        if (isInLineComment(oneLine)) {
            insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
            parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, false);
        } else if (isBlockCommentStarts(oneLine)) {
            insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
            parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, true);
        } else if (inBlockComment) {
            insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
            parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, !isBlockCommentEnds(oneLine));
        } else {
            openCurlyBracketFound = hasOpenCurlyBracket(oneLine, unmatch);
            closeCurlyBracketFound = hasCloseCurlyBracket(oneLine, unmatch);
            if (isVariableDefinition(oneLine)) {
                insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                parseComplexVariableDef(instream, maxCharPerLine, sourceCode, FUNCTION);
            } else if (isVariableDeclaration(oneLine)) {
                if (strstr(oneLine, ";") == NULL) readUntil(oneLine, ";", maxCharPerLine, instream);
                dataType = getDataType(oneLine, dataTypeName);
                me = locateFunction(allFunctions, functionName);
                if (me!=NULL) {
                    getDataList(&(me->variables), oneLine, dataTypeName, dataType);
                }
                if (strstr(oneLine, "class") != NULL) {
                    replaceLine = substitute(oneLine, "class", "struct", false, 1);
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), replaceLine, 0, indent);
                    free(replaceLine);
                } else {
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                }
                parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, false);
            } else if (openCurlyBracketFound==true) {
                insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, false);
            } else if (closeCurlyBracketFound==false) {
                if (hasFunctionCall(oneLine)) {
                    updateSiblingFunctionCall(oneLine, &replaceLine, allFunctions, allClassData, className, functionName);
                    oneLine = realloc(oneLine, strlen(replaceLine)+1);
                    strcpy(oneLine, replaceLine);
                    free(replaceLine);
                }
                addStructReference(oneLine, &replaceLine, allClassData, allFunctions, className, functionName);
                insertBlock((struct BlockType **)&(sourceCode->functionBlock), replaceLine, 0, indent);
                free(replaceLine);
                parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, false);
            } else {
                if (*unmatch == 0){
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), "\n", 1, indent);
                } else {
                    splitLine(oneLine, "}", unmatch, instream);
                    if (*unmatch == 0){
                        if (hasFunctionCall(oneLine)) {
                            updateSiblingFunctionCall(oneLine, &replaceLine, allFunctions, allClassData, className, functionName);
                            strcpy(oneLine, replaceLine);
                            free(replaceLine);
                        }
                        addStructReference(oneLine, &replaceLine, allClassData, allFunctions, className, functionName);
                        insertBlock((struct BlockType **)&(sourceCode->functionBlock), replaceLine, 1, indent);
                        insertBlock((struct BlockType **)&(sourceCode->functionBlock), "\n\n", 1, indent);
                        free(replaceLine);
                    } else {
                        insertBlock((struct BlockType **)&(sourceCode->functionBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                        parseFunctionBody(instream, maxCharPerLine, sourceCode, allClassData, allFunctions, className, functionName, unmatch, false);
                    }
                }
            }
        }
    }
    free(oneLine);
}

void parseMainBody(FILE * instream, int maxCharPerLine, struct CodeFragments * sourceCode, 
                   struct ClassType * allClasses, struct StructType ** identifierToClassMap, 
                   struct DataType ** allVariables, int * unmatch, enum ContextType *context,
                   bool declaration, bool inBlockComment) {
    enum PRIMITIVE_TYPE dataType;
    char * oneLine = (char *)malloc(maxCharPerLine+1);
    char * replaceLine = NULL;
    char * identifier = NULL;
    char * functionName = NULL;
    char * parameters = NULL;
    char dataTypeName[80] = "";
    char indent;
    bool openCurlyBracketFound = false;
    bool closeCurlyBracketFound = false;
    int startPos;
    int startQuote = -1;
    int endQuote = -1;
    
    if (readOneLine(oneLine, maxCharPerLine, &indent, instream) != NULL) {
        if (isInLineComment(oneLine)) {
            if (declaration) {
                insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
            } else {
                insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
            }
            parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, false);
        } else if (isBlockCommentStarts(oneLine)) {
            if (declaration) {
                insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
            } else {
                insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
            }
            parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, true);
        } else if (inBlockComment) {
            if (declaration) {
                insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
            } else {
                insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
            }
            parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, !isBlockCommentEnds(oneLine));
        } else {
            openCurlyBracketFound = hasOpenCurlyBracket(oneLine, unmatch);
            closeCurlyBracketFound = hasCloseCurlyBracket(oneLine, unmatch);
            if (isVariableDefinition(oneLine)) {
                insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
                parseComplexVariableDef(instream, maxCharPerLine, sourceCode, MAINDECLARATION); 
            } else if (isVariableDeclaration(oneLine)) { 
                if (strstr(oneLine, "class") != NULL) {
                    startPos = strstr(oneLine, "class") - oneLine;
                    startQuote = -1;
                    endQuote = -1;
                    if (printRange(oneLine, 0, &startQuote, &endQuote)) {
                        if (!((startPos>startQuote) && (startPos<endQuote))) {
                            createConstructorCalls(allClasses, &(sourceCode->constructorCallBlock), identifierToClassMap, oneLine);
                            replaceLine = substitute(shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), "class", "struct", false, 1);
                            insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), replaceLine, 1, indent);
                            free(replaceLine);
                        } else {
                            insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
                        }          
                    } else {
                        createConstructorCalls(allClasses, &(sourceCode->constructorCallBlock), identifierToClassMap, oneLine);
                        replaceLine = substitute(shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), "class", "struct", false, 1);
                        insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), replaceLine, 1, indent);
                        free(replaceLine);
                    }
                    parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, false);
                } else {
                    dataType = getDataType(oneLine, dataTypeName);
                    getDataList(allVariables, oneLine, dataTypeName, dataType);
                    insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
                    parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, false);
                }
            } else if (openCurlyBracketFound) {
                declaration = declaration?isBlankLine(oneLine):false;
                if (declaration) {
                    insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 1, indent);
                } else {
                    insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
                }
                parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, false);
            } else if (closeCurlyBracketFound) {
                insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
            } else {
                declaration = declaration?isBlankLine(oneLine):false;
                if (declaration) {
                    insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 1, indent);
                } else {
                    *context = MAINBODY;
                    if (isFunctionOfStructCall(oneLine)) {
                        replaceLine = convertFunctionCall(oneLine, *allVariables, *identifierToClassMap);
                        insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), replaceLine, 0, indent);
                        free(identifier);
                        free(functionName);
                        free(parameters);
                        free(replaceLine);
                    } else {
                        insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
                    }
                }
                parseMainBody(instream, maxCharPerLine, sourceCode, allClasses, identifierToClassMap, allVariables, unmatch, context, declaration, false);
            }
        }
    }
    free(oneLine);
}

void parseClassBody(FILE * instream, int maxCharPerLine, struct CodeFragments * sourceCode, 
                    char * className, struct DataType ** allData, struct FunctionType ** allFunctions, 
                    bool * methodEncountered, bool inBlockComment) {
    enum PRIMITIVE_TYPE dataType;
    struct DataType * parameters = NULL;
    struct DataType * onePara = NULL;
    char * oneLine = (char *)malloc(maxCharPerLine+1);
    char * line = NULL;
    char * replaceLine = NULL;
    char * identifier = NULL;
    char * originalFunctionName = NULL;
    char * modifiedFunctionName = NULL;
    char * postFix = NULL;
    char * constructorDeclaration = NULL;
    char * functionPtr = NULL;
    char * functionPtrAssignment = NULL;
    char paraList[80] = "";
    char dataInitLine[80] = "";
    char dataTypeName[80] = "";
    char indentation[80];
    char indent;
    int count = 0;
    int noOfShift = 0;
    int unmatch = 0;
    
    if (readOneLine(oneLine, maxCharPerLine, &indent, instream) != NULL) {
        if (isInLineComment(oneLine)) {
            insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
            parseClassBody(instream, maxCharPerLine, sourceCode, className, allData, allFunctions, methodEncountered, false);
        } else if (isBlockCommentStarts(oneLine)) {
            insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
            parseClassBody(instream, maxCharPerLine, sourceCode, className, allData, allFunctions, methodEncountered, true);
        } else if (inBlockComment) {
            insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
            parseClassBody(instream, maxCharPerLine, sourceCode, className, allData, allFunctions, methodEncountered, !isBlockCommentEnds(oneLine));
        } else {
            if (isVariableDefinition(oneLine)) {
                insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
                parseComplexVariableDef(instream, maxCharPerLine, sourceCode, CLASS); 
            } else if (isVariableDeclaration(oneLine)) {
                if (strstr(oneLine, ";") == NULL) readUntil(oneLine, ";", maxCharPerLine, instream);
                dataType = getDataType(oneLine, dataTypeName);
                getDataList(allData, oneLine, dataTypeName, dataType);
                if (strstr(oneLine, "=") != NULL) {
                    strcpy(dataInitLine, className);
                    strcat(dataInitLine, "Reference->");
                    line =  malloc(strlen(oneLine)+1);
                    strcpy(line, strstr(oneLine, dataTypeName)+strlen(dataTypeName));
                    leftTrim(line);
                    strcat(dataInitLine, line);
                    if (*methodEncountered == false) {
                    constructorDeclaration = malloc(strlen(className)+strlen(className)+strlen(className)+strlen("void constructor() {\n")+strlen("Reference")+strlen("struct ")+ strlen(" * ")+1);
                    strcpy(constructorDeclaration, "void constructor");
                    strcat(constructorDeclaration, className);
                    strcat(constructorDeclaration, "(struct ");
                    strcat(constructorDeclaration, className);
                    strcat(constructorDeclaration, " * ");
                    strcat(constructorDeclaration, className);
                    strcat(constructorDeclaration, "Reference) {\n");
                        insertBlock((struct BlockType **)&(sourceCode->constructorDeclarationBlock),  shiftLeft(constructorDeclaration, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                        free(constructorDeclaration);
                        *methodEncountered = true;
                    }
                    line = realloc(line, strlen(dataInitLine)+1);
                    strcpy(line, dataInitLine);
                    leftTrim(dataInitLine);
                    insertBlock((struct BlockType **)&(sourceCode->constructorDeclarationBlock), dataInitLine, 1, indent);
                    *strstr(oneLine, "=") = '\0';
                    strcat(oneLine, ";");
                    free(line);
                }
                insertBlock((struct BlockType **)&(sourceCode->classBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                parseClassBody(instream, maxCharPerLine, sourceCode, className, allData, allFunctions, methodEncountered, false);
            } else if (isFunctionDeclaration(oneLine)) {
                if (*methodEncountered == false) {
                    constructorDeclaration = malloc(strlen(className)+strlen(className)+strlen(className)+strlen("void constructor() {\n")+strlen("Reference")+strlen("struct ")+ strlen(" * ")+1);
                    strcpy(constructorDeclaration, "void constructor");
                    strcat(constructorDeclaration, className);
                    strcat(constructorDeclaration, "(struct ");
                    strcat(constructorDeclaration, className);
                    strcat(constructorDeclaration, " * ");
                    strcat(constructorDeclaration, className);
                    strcat(constructorDeclaration, "Reference) {\n");
                    insertBlock((struct BlockType **)&(sourceCode->constructorDeclarationBlock),  shiftLeft(constructorDeclaration, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                    free(constructorDeclaration);
                    *methodEncountered = true;
                }
                identifier = getIdentifier(oneLine);
                dataType = getDataType(oneLine, dataTypeName);
                getParameterList(&parameters, oneLine);
                postFix = getPostFix(parameters);
                onePara = parameters;
                while (onePara != NULL) {
                    strcat(paraList, ", ");
                    strcat(paraList, onePara->dataTypeName);
                    if (strstr(onePara->identifier, "[") != NULL) {
                        strcat(paraList, " *");
                    }
                    onePara = onePara->next;
                }
                originalFunctionName = malloc(strlen(className)+strlen(identifier)+1);
                modifiedFunctionName = malloc(strlen(className)+strlen(identifier)+strlen(postFix)+1);
                strcpy(originalFunctionName, identifier);
                strcpy(modifiedFunctionName, className);
                strcat(modifiedFunctionName, identifier);
                strcat(modifiedFunctionName, postFix);
                free(postFix);
                replaceLine = substitute(oneLine, identifier, modifiedFunctionName, false, 1);
                insertFunction(allFunctions, dataType, modifiedFunctionName, originalFunctionName, parameters);
                free(identifier);
                functionPtr = malloc(strlen(dataTypeName)+strlen(modifiedFunctionName)+strlen(className)+strlen(" (*)(struct  *, );\n")+strlen(paraList)+1);
                
                strcpy(functionPtr, dataTypeName);
                strcat(functionPtr, " (*");
                strcat(functionPtr, modifiedFunctionName);
                strcat(functionPtr, ")(struct ");
                strcat(functionPtr, className);
                strcat(functionPtr, " *");
                strcat(functionPtr, paraList);
                strcat(functionPtr, ");\n");
                functionPtrAssignment = malloc(strlen("    ")+strlen(className)+strlen("Reference")+strlen("->")+strlen(modifiedFunctionName)+strlen(" = &")+strlen(modifiedFunctionName)+strlen(";\n")+1);
                count = 0; 
                noOfShift = (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE;
                while (count<noOfShift) {
                    *(indentation+count) = indent;
                    count++;
                }
                *(indentation+count) = '\0';
                strcpy(functionPtrAssignment,indentation);
                strcat(functionPtrAssignment,className);
                strcat(functionPtrAssignment,"Reference->");
                strcat(functionPtrAssignment,modifiedFunctionName);
                strcat(functionPtrAssignment," = &");
                strcat(functionPtrAssignment, modifiedFunctionName);
                strcat(functionPtrAssignment,";\n");
                oneLine = realloc(oneLine, strlen(replaceLine)+1);
                strcpy(oneLine, replaceLine);
                modifyParameters(oneLine, &replaceLine, className);
                insertBlock((struct BlockType **)&(sourceCode->classBlock), shiftLeft(functionPtr, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                insertBlock((struct BlockType **)&(sourceCode->functionBlock), shiftLeft(replaceLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                if (*methodEncountered) {
                    insertBlock((struct BlockType **)&(sourceCode->constructorDeclarationBlock),  shiftLeft(functionPtrAssignment, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                }
                unmatch = 0;
                hasOpenCurlyBracket(replaceLine, &unmatch);
                free(replaceLine);
                free(functionPtr);
                free(functionPtrAssignment);
                parseFunctionBody(instream, maxCharPerLine, sourceCode, *allData, *allFunctions, className, modifiedFunctionName, &unmatch, false);
                free(originalFunctionName);
                free(modifiedFunctionName);
                parseClassBody(instream, maxCharPerLine, sourceCode, className, allData, allFunctions, methodEncountered, false);
            } else {
                if (hasCloseCurlyBracket(oneLine, &unmatch)==false) {
                    insertBlock((struct BlockType **)&(sourceCode->classBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                    parseClassBody(instream, maxCharPerLine, sourceCode, className, allData, allFunctions, methodEncountered, false);
                } else {
                    insertBlock((struct BlockType **)&(sourceCode->classBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                    insertBlock((struct BlockType **)&(sourceCode->classBlock), "\n", 0, indent);
                    if (*methodEncountered) {
                        insertBlock((struct BlockType **)&(sourceCode->constructorDeclarationBlock), "}\n\n", 0, indent);
                    }
                }
            }
        }
    }
    freeData(parameters);
    free(oneLine);
}
void parseSourceCode(FILE * instream, int maxCharPerLine, struct CodeFragments * sourceCode, 
                     struct DataType ** allNonClassVariables, enum ContextType * context, 
                     struct ClassType ** allClasses, bool inBlockComment) {
    enum PRIMITIVE_TYPE dataType;
    char dataTypeName[80] = "";
    struct BlockType * prototype = NULL;
    struct StructType * identifierToClassMap = NULL;
    struct ClassType * currentClass = NULL;
    char * oneLine = (char *)malloc(maxCharPerLine+1);
    char * replaceLine = NULL;
    char * className = NULL;
    char indent;
    bool openCurlyBracketFound = false;
    bool methodEncountered = false;
    int unmatch = 0;
    
    if (readOneLine(oneLine, maxCharPerLine, &indent, instream) != NULL) {
        if (isInLineComment(oneLine)) {
            if (*context == GLOBAL) {
                insertBlock((struct BlockType **)&(sourceCode->globalBlock), oneLine, 0, indent);
            } else if (*context == CLASS) {
                insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
            }
            parseSourceCode(instream, maxCharPerLine, sourceCode, allNonClassVariables, context, allClasses, false);
        } else if (isBlockCommentStarts(oneLine)) {
            if (*context == GLOBAL) {
                insertBlock((struct BlockType **)&(sourceCode->globalBlock), oneLine, 0, indent);
            } else if (*context == CLASS) {
                insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
            }
            parseSourceCode(instream, maxCharPerLine, sourceCode, allNonClassVariables, context, allClasses, true);
        } else if (inBlockComment) {
            if (*context == GLOBAL) {
                insertBlock((struct BlockType **)&(sourceCode->globalBlock), oneLine, 0, indent);
            } else if (*context == CLASS) {
                insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
            }
            parseSourceCode(instream, maxCharPerLine, sourceCode, allNonClassVariables, context, allClasses, !isBlockCommentEnds(oneLine));
        } else {
            if (isVariableDefinition(oneLine)) {
                insertBlock((struct BlockType **)&(sourceCode->globalBlock), oneLine, 0, indent);
                parseComplexVariableDef(instream, maxCharPerLine, sourceCode, GLOBAL); 
            } else if (isVariableDeclaration(oneLine)) { 
                if (strstr(oneLine, "class") != NULL) {
                    replaceLine = substitute(oneLine, "class", "struct", false, 1);
                    insertBlock((struct BlockType **)&(sourceCode->prototypeBlock), oneLine, 1, indent);
                    insertBlock((struct BlockType **)&(sourceCode->classBlock), replaceLine, 0, indent);
                    free(replaceLine);
                } else {
                    if (*context == GLOBAL) {
                        insertBlock((struct BlockType **)&(sourceCode->globalBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                        dataType = getDataType(oneLine, dataTypeName);
                        getDataList(allNonClassVariables, oneLine, dataTypeName, dataType);
                    } else if (*context == FUNCTION) {
                        insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                    } else if (*context == CLASS) {
                        insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
                    } else if (*context == MAINDECLARATION) {
                        insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
                    } else if (*context == MAINBODY) {
                        insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
                    } else if (*context == FUNCTION) {
                        insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                    } else {
                        insertBlock((struct BlockType **)&(sourceCode->endBlock), oneLine, 0, indent);
                    }
                }
            } else if (isClassDeclaration(oneLine)) {
                *context = CLASS;
                unmatch = 0;
                openCurlyBracketFound = hasOpenCurlyBracket(oneLine, &unmatch);
                if (!openCurlyBracketFound) readUntil(oneLine, "{", maxCharPerLine, instream);
                className = getClassName(oneLine);
                insertClass(allClasses, className);
                currentClass = locateClass(*allClasses, className);
                replaceLine = substitute(shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), "class", "struct", false, 1);
                insertBlock((struct BlockType **)&(sourceCode->classBlock), replaceLine, 0, indent);
                free(replaceLine);
                if (currentClass != NULL) {
                    parseClassBody(instream, maxCharPerLine, sourceCode, className, &(currentClass->variables), &(currentClass->functions), &methodEncountered, false);
                }
                free(className);
            } else if (isMainDeclaration(oneLine)) {
                *context = MAINDECLARATION;
                insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 0, indent);
                prototype = sourceCode->prototypeBlock;
                while (prototype != NULL) {
                    createConstructorCalls(*allClasses, &(sourceCode->constructorCallBlock), &identifierToClassMap, prototype->statement);
                    prototype = prototype->next;
                }
                unmatch = 0;
                openCurlyBracketFound = hasOpenCurlyBracket(oneLine, &unmatch);
                parseMainBody(instream, maxCharPerLine, sourceCode, *allClasses, &identifierToClassMap, allNonClassVariables, &unmatch, context, true, false);
                freeStruct(identifierToClassMap);
                *context = END;
            } else if (isFunctionDeclaration(oneLine)) {
                if (*context == CLASS) {
                    *context = FUNCTION;
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                } else if (*context == GLOBAL) {
                    insertBlock((struct BlockType **)&(sourceCode->globalBlock), oneLine, 0, indent);
                } else if (*context == FUNCTION) {
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                } else {
                    insertBlock((struct BlockType **)&(sourceCode->endBlock), oneLine, 0, indent);
                }
            } else {
                if (*context == GLOBAL) {
                   insertBlock((struct BlockType **)&(sourceCode->globalBlock), shiftLeft(oneLine, indent, (indent==' ')?SPACE_INDENT_SIZE:TAB_INDENT_SIZE), 1, indent);
                } else if (*context == FUNCTION) {
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                } else if (*context == CLASS) {
                    insertBlock((struct BlockType **)&(sourceCode->classBlock), oneLine, 0, indent);
                } else if (*context == MAINDECLARATION) {
                    insertBlock((struct BlockType **)&(sourceCode->mainDeclarationBlock), oneLine, 0, indent);
                } else if (*context == MAINBODY) {
                    insertBlock((struct BlockType **)&(sourceCode->mainBodyBlock), oneLine, 0, indent);
                } else if (*context == FUNCTION) {
                    insertBlock((struct BlockType **)&(sourceCode->functionBlock), oneLine, 0, indent);
                } else {
                    insertBlock((struct BlockType **)&(sourceCode->endBlock), oneLine, 0, indent);
                }
            }
            parseSourceCode(instream, maxCharPerLine, sourceCode, allNonClassVariables, context, allClasses, false);
        }
    }
    free(oneLine);
}
