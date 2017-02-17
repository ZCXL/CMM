/**************************************************/
/* File: symtab.h                                 */
/* Symbol table interface for the CMM compiler    */
/* (allows only one symbol table )                */
/* Compiler Construction: Principles and Practice */
/* Author:zhuchao 2015-10-11                      */
/**************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"
/* SIZE is the size of the hash table */
#define SIZE 255

/* the list fo line numbers of the source 
 * code in while a variable is referenced 
 */
typedef struct LineListRec{
	int lineno;
	struct LineListRec *next;
} *LineList;

/* The record in the bucket lists for 
 * each variable, including name,
 * assigned memory location, and 
 * the list of line numbers in which 
 * it appears in the source code
 */
typedef struct BucketListRec{
	char *name;
	int params_count;
	struct {
		char *name;
		ExprType type;
	}params[MAXPARAMS];
	int size;
	int level;
	LineList lines;
	ExprType type;
	ExprStatType sType;
	int memloc;
	struct BucketListRec *next;	
} * BucketList;
typedef struct FieldListRec{
	BucketList hashTable[SIZE];
	int position;
	int level;
	struct FieldListRec *child;
	struct FieldListRec *sibling;
	struct FieldListRec *last;
} *FieldList;
/* Procedure st_insert inserts line numbers and 
 * memory locations into the symbol table 
 * loc=memory location is inserted only the 
 * first time,otherwise ignored
 */
void st_insert(char *name,int lineno,int loc,ExprType type,ExprStatType sType);

/* Function st_lookup returns the memory
 * location of a variable or -1 if not founc
 */
int st_lookup(char *name);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents
 * to the listing File
 */
void printSymTab(FILE *listing);
/* Procedure createFieldList create filed list at first*/
void createFieldList();
/* Procedure getNextHashTable create a new hash table
 * linked with current filed
 */
void getNextHashTable(int lineno);
/* Procedure getLastHashTable return last level hash table*/
void getLastHashTable();
/* Procedure lookup find declaration nearly */
BucketList lookup(char *name,int lineno);
/* Procedure insert_params add params to function declare */
void insert_params(char *name,char *param_name,int lineno,ExprType type);
/* insert array's size */
void insert_size(char *name,int lineno,int size);
/* function getCurrentList is used to get current FieldList*/
FieldList getCurrentList();
/* function freeSymbolTable is used to free all
 * information in symbol table
 */
void freeSymbolTable();
#endif