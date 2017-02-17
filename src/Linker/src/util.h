#ifndef _UTIL_H_
#define _UTIL_H_
#include "globals.h"
/* file buffer size*/
#define BUFFER_SIZE 100
char *clearSpace(char *str);
/* look up some register by using name */
RegisterType registerLookup(char *name);
char *lookupRegister(RegisterType rType);
/* look up some instuction */
TokenType tokenLookup(char *name);
char *lookupToken(TokenType tok);
/* string cut */
char* substr(char *source,unsigned int start,unsigned int end);
 /* Function copyString allocates and makes a new 
 * copy of an existing copyString
 */
char *copyString(char *s);
/* add two strings together */
char *strplus(char *first,char *second);
/* pow */
int pow_num(int num1,int num2);
/* Function getHex compute number to Hex */
char *getHex(ExprType t,void*num);
/* translate hex to number */
int hexToNum(char *str);
/* translate number to string */
char *numToStr(int num);
/* print file information */
void printFile(PInstruction instructions,FILE *file);
/* eight kinds of address way */
char *getStyle(PAddress address);
/* searche some header file */
int searchHeader(char *name);
/* print buffer lines to file */
void printToFile();
#endif