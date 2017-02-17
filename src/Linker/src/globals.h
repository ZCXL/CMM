#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#ifndef BOOL
#define BOOL int
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*The max size of a token*/
#define MAXTOKENLEN 40
/*The max size of registers */
#define MAXREGISTER 24
/*The max size of instructions */
#define MAXINSTRUCTION 68
/*The max size of files */
#define MAXFILENUM 20

typedef enum
{
	ENDFILE,ERROR,ADDRESS,
	/* reserverd words
	 * .file,.include,.global,.align,.size,.type,.Int,.Real,.Char,.Bool,.dclr
	 */
	DOTFILE,DOTINCLUDE,DOTGLOBAL,DOTALIGN,DOTSIZE,DOTTYPE,DOTINT,DOTREAL,DOTCHAR,DOTBOOL,DOTDCLR,
	/* special symbols
	 * push,pushl,pushlabs,pushb,pushw,pop,popl,poplabs,popb,popw,mov,movl,movb,movw,movlabs,
	 * leal,inc,neg,dec,not,addl,addlabs,addb,addw,subl,sudlabs,subb,subw,mull,mullabs,mulb,mulw,divl,divlabs,divb,divw
	 * cmpl,je,jne,jg,jl,jge,jle,call,ret,leave,in,out
	 */
	PUSH,PUSHL,PUSHLABS,PUSHB,PUSHW,POP,POPL,POPLABS,POPB,POPW,MOV,MOVL,MOVB,MOVW,MOVLABS,
	LEAL,INC,NEG,DEC,NOT,ADDL,ADDLABS,ADDB,ADDW,SUBL,SUBLABS,SUBB,SUBW,MULL,MULLABS,MULB,MULW,DIVL,DIVLABS,DIVB,DIVW,
	CMPL,JE,JNE,JG,JL,JGE,JLE,CALL,RET,LEAVE,IN,INL,INLABS,INB,INW,OUT,OUTL,OUTLABS,OUTB,OUTW,OUTP
}TokenType;
extern FILE* source;
extern FILE* listing;
extern FILE* code;
extern char filename[MAXFILENUM][120];
/* eight registers*/
typedef enum {EAX,AX,AH,AL,ECX,CX,CH,CL,EDX,DX,DH,DL,EBX,BX,BH,BL,EIP,IP,EDI,DI,ESP,SP,EBP,BP,NO}RegisterType;
/* The way to get address */
typedef enum {NUM_A,REG_A,DIRC_A,INDIRC_A,EXCUR_A,CHANGE_A,RATIO_A}AddressType;

typedef enum {Void,Int,Real,Bool,Char,String}ExprType;
/* memery address */
typedef struct Address{
	char* position;
	RegisterType r1;
	RegisterType r2;
	AddressType aType;
	char *times;
}Address,*PAddress;
/* instruction set */
typedef struct Instruction{
	int position;
	TokenType op;
	PAddress a1;
	PAddress a2;
	struct Instruction *next;
}Instruction,*PInstruction;
/* included files */
typedef struct Include{
	char *filename;
	struct Include *next;
}Include,*PInclude;
/* global variable information */
typedef struct Global{
	char *name;
	char *position;
	ExprType type;
	char *val;
	struct Global *next;
}Global,*PGlobal;
/* functon info */
typedef struct Function{
	PInstruction instructions;
	struct FileListRec* parent;
	char *name;
	int position;
	struct Function *next;
}Function,*PFunction;
/* file info */
typedef struct FileListRec{
	PInclude includes;
	PGlobal globals;
	PFunction functions;
	char *filename;
	struct FileListRec *next;
}FileList,*PFileList;
/* File cache */
typedef struct BufferList{
	char *string;
	int position;
	struct BufferList *next;
} BufferList,*PBufferList;
/* error */
extern int Error;
extern int lineno;
extern int IsPrint;
#endif