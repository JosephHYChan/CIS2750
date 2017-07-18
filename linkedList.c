#include "linkedList.h"

struct ClassType * locateClass(struct ClassType * classes, char * tagName) {
    if (classes == NULL) {
        return NULL;
    } else if (strcmp(classes->tagName, tagName) == 0) {
        return classes;
    } else {
        return locateClass(classes->next, tagName);
    }
}

struct FunctionType * locateFunction(struct FunctionType * functions, char * identifier) {
    if (functions == NULL) {
        return NULL;
    } else if (strcmp(functions->identifier, identifier) == 0) {
        return functions;
    } else {
        return locateFunction(functions->next, identifier);
    }
}

struct DataType * locateData(struct DataType * data, char * identifier) {
    if (data == NULL) {
        return NULL;
    } else if (strcmp(data->identifier, identifier) == 0) {
        return data;
    } else {
        return locateData(data->next, identifier);
    }
}

struct StructType * locateStruct(struct StructType * structType, char * identifier) {
    if (structType == NULL) {
        return NULL;
    } else if (strcmp(structType->identifier, identifier) == 0) {
        return structType;
    } else {
        return locateStruct(structType->next, identifier);
    }
}

void insertBlock(struct BlockType ** blocks, char * statement, int indentLevel, char indent)
{
    if (*blocks == NULL) {
        *blocks = malloc(sizeof(struct BlockType));
        (*blocks)->statement = malloc(strlen(statement)+1);
        strcpy((*blocks)->statement, statement);
        (*blocks)->indentLevel = indentLevel;
        (*blocks)->indent = indent;
        (*blocks)->next = NULL;
    } else {
        insertBlock((struct BlockType **)&((*blocks)->next), statement, indentLevel, indent);
    }
}

void insertClass(struct ClassType ** classes, char * tagName) 
{
    if (*classes == NULL) {
        *classes = malloc(sizeof(struct ClassType));
        (*classes)->tagName = malloc(strlen(tagName)+1);
        strcpy((*classes)->tagName, tagName);
        (*classes)->variables = NULL;
        (*classes)->functions = NULL;
        (*classes)->next = NULL;
    } else {
        insertClass(&((*classes)->next), tagName);
    }
}

void insertFunction(struct FunctionType ** functions, enum PRIMITIVE_TYPE returnType, char * identifier, char * originalName, struct DataType * parameters)
{
    if (*functions == NULL) {
        *functions = malloc(sizeof(struct FunctionType));
        (*functions)->returnType = returnType;
        (*functions)->identifier = malloc(strlen(identifier)+1);
        (*functions)->originalName = malloc(strlen(originalName)+1);
        (*functions)->parameters = NULL;
        (*functions)->variables = NULL;
        (*functions)->next = NULL;
        strcpy((*functions)->identifier, identifier);
        strcpy((*functions)->originalName, originalName);
        while (parameters != NULL) {
            insertData(&((*functions)->parameters), parameters->dataType, parameters->dataTypeName, parameters->identifier);
            parameters = parameters->next;
        }
    } else {
        insertFunction(&((*functions)->next), returnType, identifier, originalName, parameters);
    }
}

void insertFunctionCall(struct FunctionCall ** functionCalls, char * functionCall, char * replaceCall)
{
    if (*functionCalls == NULL) {
        *functionCalls = malloc(sizeof(struct FunctionCall));
        (*functionCalls)->functionCall = malloc(strlen(functionCall)+1);
        (*functionCalls)->replaceCall = malloc(strlen(replaceCall)+1);
        strcpy((*functionCalls)->functionCall, functionCall);
        strcpy((*functionCalls)->replaceCall, replaceCall);
        (*functionCalls)->next = NULL;
    } else {
        insertFunctionCall(&((*functionCalls)->next), functionCall, replaceCall);
    }
}

void insertData(struct DataType ** data, enum PRIMITIVE_TYPE dataType, char * dataTypeName, char * identifier)
{
    if (*data == NULL) {
        *data = malloc(sizeof(struct DataType));
        (*data)->dataType = dataType;
        (*data)->dataTypeName = malloc(strlen(dataTypeName)+1);
        (*data)->identifier = malloc(strlen(identifier)+1);
        strcpy((*data)->dataTypeName, dataTypeName);
        strcpy((*data)->identifier, identifier);
        (*data)->next = NULL;
    } else {
        insertData(&((*data)->next), dataType, dataTypeName, identifier);
    }
}
void populateKeys(struct FunctionCall ** keys, char * identifier, char * className, struct FunctionType * function) {
    char * key = NULL;
    char * replacement = NULL;
    if (function != NULL) {
        key = malloc(strlen(identifier)+strlen(function->originalName)+strlen(".")+1);
        replacement = malloc(strlen(identifier)+strlen(className)+strlen(function->originalName)+strlen(".")+1);
        strcpy(key, identifier);
        strcat(key, ".");
        strcat(key, function->originalName);
        strcpy(replacement, identifier);
        strcat(replacement, ".");
        strcat(replacement, className);
        strcat(replacement, function->originalName);
        insertFunctionCall(keys, key, replacement);
        free(key);
        free(replacement);
        populateKeys(keys, identifier, className, function->next);
    }
}

void insertStruct(struct StructType ** structType, char * identifier, struct ClassType * classType)
{
    if (*structType == NULL) {
        *structType = malloc(sizeof(struct StructType));
        (*structType)->identifier = malloc(strlen(identifier)+1);
        strcpy((*structType)->identifier, identifier);
        (*structType)->classType = classType;
        (*structType)->keys = NULL;
        populateKeys(&((*structType)->keys), identifier, classType->tagName, classType->functions);
        (*structType)->next = NULL;
    } else {
        insertStruct(&((*structType)->next), identifier, classType);
    }
}

void freeBlock(struct BlockType * block)
{
    if (block != NULL) {
        freeBlock(block->next);
        free(block->statement);
        block->statement = NULL;
        block->next = NULL;
        free(block);
    }
}

void freeClass(struct ClassType * classes)
{
    if (classes != NULL) {
        freeClass(classes->next);
        freeData(classes->variables);
        freeFunction(classes->functions);
        free(classes->tagName);
        classes->tagName = NULL;
        classes->next = NULL;
        free(classes);
    }
}

void freeFunctionCall(struct FunctionCall * functionCalls)
{
    if (functionCalls != NULL) {
        freeFunctionCall(functionCalls->next);
        free(functionCalls->functionCall);
        free(functionCalls->replaceCall);
        functionCalls->functionCall = NULL;
        functionCalls->next = NULL;
        free(functionCalls);
    }
}

void freeFunction(struct FunctionType * functions)
{
    if (functions != NULL) {
        freeFunction(functions->next);
        freeData(functions->parameters);
        freeData(functions->variables);
        free(functions->identifier);
        free(functions->originalName);
        functions->identifier = NULL;
        functions->originalName = NULL;
        functions->parameters = NULL;
        functions->next = NULL;
        free(functions);
    }
}


void freeData(struct DataType * data)
{
    if (data != NULL) {
        freeData(data->next);
        free(data->dataTypeName);
        free(data->identifier);
        data->dataTypeName = NULL;
        data->identifier = NULL;
        data->next = NULL;
        free(data);
    }
}

void freeStruct(struct StructType * structType)
{
    if (structType != NULL) {
        freeStruct(structType->next);
        freeFunctionCall(structType->keys);
        free(structType->identifier);
        structType->identifier = NULL;
        structType->classType = NULL;
        structType->next = NULL;
        free(structType);
    }
}

void printBlock(FILE * outStream, struct BlockType * block)
{
    int level;
    int noOfshifts;
    
    if (block != NULL) {
        for (level=0; level < block->indentLevel; level++) {
            if (block->indent == '\t') {
                for (noOfshifts = 0; noOfshifts < TAB_INDENT_SIZE; noOfshifts++) {
                    fprintf(outStream, "%c", block->indent);
                }
            } else if (block->indent == ' ') {
                for (noOfshifts = 0; noOfshifts < SPACE_INDENT_SIZE; noOfshifts++) {
                    fprintf(outStream, "%c", block->indent);
                }
            }
        }
        fprintf(outStream, "%s", block->statement);
        printBlock(outStream, block->next);
    }   
}

void printClass(FILE * outStream, struct ClassType * classes)
{
    if (classes != NULL) {
        fprintf(outStream, "%s\n", classes->tagName);
        printFunction(outStream, classes->functions);
        printData(outStream, classes->variables);
        printClass(outStream, classes->next);
    }   
}

void printFunctionCall(FILE * outStream, struct FunctionCall * functionCalls)
{
    if (functionCalls != NULL) {
        fprintf(outStream, "%s\n", functionCalls->functionCall);
        fprintf(outStream, "%s\n", functionCalls->replaceCall);
        printFunctionCall(outStream, functionCalls->next);
    }   
}

void printFunction(FILE * outStream, struct FunctionType * functions)
{
    if (functions != NULL) {
        fprintf(outStream, "%d\n", functions->returnType);
        fprintf(outStream, "%s\n", functions->identifier);
        fprintf(outStream, "%s\n", functions->originalName);
        printData(outStream, functions->parameters);
        printData(outStream, functions->variables);
        printFunction(outStream, functions->next);
    }   
}

void printData(FILE * outStream, struct DataType * data)
{
    if (data != NULL) {
        fprintf(outStream, "%d\n", data->dataType);
        fprintf(outStream, "%s\n", data->dataTypeName);
        fprintf(outStream, "%s\n", data->identifier);
        printData(outStream, data->next);
    }   
}

void printStruct(FILE * outStream, struct StructType * structs)
{
    if (structs != NULL) {
        fprintf(outStream, "%s\n", structs->identifier);
        printClass(outStream, structs->classType);
        printFunctionCall(outStream, structs->keys);
        printStruct(outStream, structs->next);
    }   
}
