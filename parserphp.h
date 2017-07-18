#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_TAG_LENGTH 2048

enum tagType {
    ACTUPON,
    BUTTON,
    DRAW,
    EXECUTABLE,
    FORM,
    HEADING,
    INIT,
    INPUT,
    LINK,
    PICTURE,
    OPENPROC,
    QUERY,
    RADIO,
    SELECT,
    STATEMENT,
    TEXT,
    UNDEFINE
};

typedef struct InputAttribute {
    int layout;
    int linefeed;
    char * type;
    char * text;
    char * name;
    char * value;
    char * exe;
    char * para;
    char * result;
    struct InputAttribute * next;
} InputAttribute;

typedef struct RadioButton {
    char * value;
    struct RadioButton * next;
} RadioButton;

typedef struct TagAttribute {
    RadioButton * buttons;
    InputAttribute * inputs;
    char * name;
    char * link;
    char * exe;
    char * text;
    char * action;
    char * value;
    char * image;
    char * file;
    int layout;
    int submit;
    int indent;
    int align;
    int width;
    int height;
    int size;
} TagAttribute;

typedef struct Tag {
    enum tagType type;
    TagAttribute attribute;
    struct Tag * next;
} Tag;

bool is(char * tag, char * expected);
bool isAlphabet(char ch);
bool isAlphaNumeric(char ch);
bool isDigit(char ch);
bool isDot(char ch);
bool isActUpon(char * tag, TagAttribute * attribute);
bool isButton(char * tag, TagAttribute * attribute);
bool isDraw(char * tag, TagAttribute * attribute);
bool isExecutable(char * tag, TagAttribute * attribute);
bool isFoot(char * tag, TagAttribute * attribute);
bool isForm(char * tag, TagAttribute * attribute);
bool isHead(char * tag, TagAttribute * attribute);
bool isHeading(char * tag, TagAttribute * attribute);
bool isInit(char * tag, TagAttribute * attribute);
bool isInput(char * tag, TagAttribute * attribute);
bool isLink(char * tag, TagAttribute * attribute);
bool isOpenProc(char * tag, TagAttribute * attribute);
bool isPicture(char * tag, TagAttribute * attribute);
bool isQuery(char * tag, TagAttribute * attribute);
bool isRadio(char * tag, TagAttribute * attribute);
bool isStatement(char * tag, TagAttribute * attribute);
bool isText(char * tag, TagAttribute * attribute);
bool mustbe(char * tag, char * expected);
bool tagFound(char * line, int * atPos);
void passthrough(FILE * phpFile, char * line, int noOfChar);
void skipAllSpaces(char * line);
void initialize(TagAttribute * attribute);
void freeup(TagAttribute * attribute);
void getTag(char * line, char * tag);
void parse(FILE * phpFile, char * tag, int indent);
Tag * search(Tag * allTags, enum tagType type);
void insertInput (InputAttribute ** root, int layout, int linefeed, char * type, char * text, char * name, char * value, char * exe, char * para, char * result);
void insertButton (RadioButton ** root, char * value);
void insertTag (Tag ** root, enum tagType type, TagAttribute * attribute);
void freeInput(InputAttribute * root);
void freeButton(RadioButton * root);
void freeTag(Tag * root);
void getToken(char * tag, char * token);
void printTag(Tag * root);
void printHTML(FILE * phpFile, Tag * allTags);
void printActUponTag(FILE * phpFile, TagAttribute attribute, int indent);
void printButtonTag(FILE * phpFile, TagAttribute attribute, int indent);
void printDrawTag(FILE * phpFile, TagAttribute attribute, int indent);
void printExecutableTag(FILE * phpFile, TagAttribute attribute, int indent);
void printFootTag(FILE * phpFile, TagAttribute attribute, int indent);
void printFormTag(FILE * phpFile, TagAttribute attribute, int indent);
void printHeadTag(FILE * phpFile, TagAttribute attribute, int indent);
void printHeadingTag(FILE * phpFile, TagAttribute attribute, int indent);
void printInitTag(FILE * phpFile, TagAttribute attribute, int indent);
void printInputTag(FILE * phpFile, TagAttribute attribute, int indent);
void printLinkTag(FILE * phpFile, TagAttribute attribute, int indent);
void printOpenProcTag(FILE * phpFile, TagAttribute attribute, int indent);
void printPictureTag(FILE * phpFile, TagAttribute attribute, int indent);
void printQueryTag(FILE * phpFile, TagAttribute attribute, int indent);
void printRadioTag(FILE * phpFile, TagAttribute attribute, int indent);
void printSelectTag(FILE * phpFile, TagAttribute attribute, int indent);
void printStatementTag(FILE * phpFile, TagAttribute attribute, int indent);
void printTextTag(FILE * phpFile, TagAttribute attribute, int indent);

#endif /* PARSER_H */

