/**************************************************/
/* File: code.h                                   */
/* Code emitting utilites for the CMM compiler    */
/* Compiler Construction: Principles and Practice */
/* Author: zhuchao 2015-10-21                     */
/**************************************************/
#ifndef _CODE_H_
#define _CODE_H_
#include "globals.h"
#include "symtab.h"
/* eight registers*/
typedef enum {EAX,AX,AH,AL,ECX,CX,CH,CL,EDX,DX,DH,DL,EBX,BX,BH,BL,EIP,IP,EDI,DI,ESP,SP,EBP,BP}RegisterType;
/* The way to get address */
typedef enum {NUM_A,REG_A,DIRC_A,INDIRC_A,EXCUR_A,CHANGE_A,RATIO_A}AddressType;
/* file buffer size*/
#define BUFFER_SIZE 100
/* File cache */
typedef struct FileList{
	char *string;
	int position;
	struct FileList *next;
} *PFileList;

typedef struct FunctionListRec{
	int position;
	char *name;
	struct FunctionListRec *next;
}* FunctionList;
typedef struct IdentifierListRec{
	char *name;
	int position;
	struct IdentifierListRec *next;
}* IdentifierList;
/* PrintToFile=TRUE*/
void printToFile();
/* Procedure emitRO emits a register-only 
 */
void emitSingle(char *op,char *r);

void emit(char *op);

void emitDouble(char *op,char *r,char *s);

void emitFunction(char *op,char *f);

int	emitSkip(int howMany);

void emitBackup(int loc);

void emitRestore(void);

void emitComment(char *c);

void emitAbs(char *op,RegisterType r,int s);

void insertFunc(char *name,int pos);

int lookupFunc(char *name);

void freeFunc();

void insertIdentifier(char *name,int pos);

int lookupIdentifier(char *name);

void freeIdentifier();

char *getStyle(int p,RegisterType r1,RegisterType r2,int i,AddressType aType);
#endif
