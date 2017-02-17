#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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
#define MAXINSTRUCTION 57
/*The max size of memory*/
#define MAXMEMORY 10240
/*The num of registers */
#define REGISTERS 8
typedef enum
{
	/* special symbols
	 * push,pushl,pushlabs,pushb,pushw,pop,popl,poplabs,popb,popw,mov,movl,movb,movw,movlabs,
	 * leal,inc,neg,dec,not,addl,addlabs,addb,addw,subl,sudlabs,subb,subw,mull,mullabs,mulb,mulw,divl,divlabs,divb,divw
	 * cmpl,je,jne,jg,jl,jge,jle,call,ret,leave,in,out
	 */
	PUSH,PUSHL,PUSHLABS,PUSHB,PUSHW,POP,POPL,POPLABS,POPB,POPW,MOV,MOVL,MOVB,MOVW,MOVLABS,
	LEAL,INC,NEG,DEC,NOT,ADDL,ADDLABS,ADDB,ADDW,SUBL,SUBLABS,SUBB,SUBW,MULL,MULLABS,MULB,MULW,DIVL,DIVLABS,DIVB,DIVW,
	CMPL,JE,JNE,JG,JL,JGE,JLE,CALL,RET,LEAVE,IN,INL,INLABS,INB,INW,OUT,OUTL,OUTLABS,OUTB,OUTW,OUTP
}TokenType;

/* eight registers*/
typedef enum {EAX,AX,AH,AL,ECX,CX,CH,CL,EDX,DX,DH,DL,EBX,BX,BH,BL,EIP,IP,EDI,DI,ESP,SP,EBP,BP,NO}RegisterType;
/* The way to get address */
typedef enum {NUM_A,REG_A,DIRC_A,INDIRC_A,EXCUR_A,CHANGE_A,RATIO_A}AddressType;

typedef enum {Void,Int,Real,Bool,Char,String}ExprType;
extern FILE* source;
extern FILE* listing;
/* memory address */
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
/* error */
extern int Error;
extern int lineno;
extern int IsPrint;
#endif