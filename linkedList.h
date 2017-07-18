#ifndef H_LINKEDLIST_H
#define H_LINKEDLIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE_INDENT_SIZE 3
#define TAB_INDENT_SIZE 1

enum PRIMITIVE_TYPE {
    UNDEFINE = 0,
    VOID = 1,
    BOOL = 2,
    CHAR = 3,
    INT = 4,
    LONG = 5,
    SHORT = 6,
    FLOAT = 7,
    DOUBLE = 8,
    SIGNED_CHAR = 9,
    SIGNED_INT = 10,
    SIGNED_LONG = 11,
    SIGNED_SHORT = 12,
    UNSIGNED_CHAR = 13,
    UNSIGNED_INT = 14,
    UNSIGNED_LONG = 15,
    UNSIGNED_SHORT = 16,
    LONG_DOUBLE = 17,
    STRUCT = 18
};

struct BlockType {
    char * statement;
    int indentLevel;
    char indent;
    struct BlockType * next;
};

struct CodeFragments {
    struct BlockType * prototypeBlock;
    struct BlockType * globalBlock;
    struct BlockType * classBlock;
    struct BlockType * constructorDeclarationBlock;
    struct BlockType * constructorCallBlock;
    struct BlockType * functionBlock;
    struct BlockType * mainDeclarationBlock;
    struct BlockType * mainBodyBlock;
    struct BlockType * endBlock;
};

struct FunctionCall {
    char * functionCall;
    char * replaceCall;
    struct FunctionCall * next;
};

struct DataType {
    enum PRIMITIVE_TYPE dataType;
    char * dataTypeName;
    char * identifier;
    struct DataType * next;
};

struct FunctionType {
    enum PRIMITIVE_TYPE returnType;
    char * identifier;
    char * originalName;
    struct DataType * parameters;
    struct DataType * variables;
    struct FunctionType * next;
};

struct ClassType {
    char * tagName;
    struct DataType * variables;
    struct FunctionType * functions;
    struct ClassType * next;
};

struct StructType {
    char * identifier;
    struct ClassType * classType;
    struct FunctionCall * keys;
    struct StructType * next;
};

struct ClassType * locateClass(struct ClassType * classes, char * tagName);
struct FunctionType * locateFunction(struct FunctionType * functions, char * identifier);
struct DataType * locateData(struct DataType * data, char * identifier);
struct StructType * locateStructure(struct StructType * structType, char * identifier);
void insertBlock(struct BlockType ** block, char * statement, int indentLevel, char indent);
void insertClass(struct ClassType ** classes, char * tagName);
void insertFunction(struct FunctionType ** functions, enum PRIMITIVE_TYPE dataType, char * identifier, char * originalName, struct DataType * parameters);
void insertFunctionCall(struct FunctionCall ** functionCalls, char * functionCall, char * replaceCall);
void insertData(struct DataType ** data, enum PRIMITIVE_TYPE dataType, char * dataTypeName, char * identifier);
void insertStruct(struct StructType ** structs, char * identifier, struct ClassType * classType);
void freeBlock(struct BlockType * block);
void freeClass(struct ClassType * classes);
void freeFunction(struct FunctionType * functions);
void freeFunctionCall(struct FunctionCall * functionCalls);
void freeData(struct DataType * data);
void freeStruct(struct StructType * structs);
void printBlock(FILE * outStream, struct BlockType * block);
void printClass(FILE * outStream, struct ClassType * classes);
void printFunction(FILE * outStream, struct FunctionType * functions);
void printFunctionCall(FILE * outStream, struct FunctionCall * functionCalls);
void printData(FILE * outStream, struct DataType * data);
void printStruct(FILE * outStream, struct StructType * structs);
#endif