#ifndef _GLOBLES_H_
#define _GLOBLES_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef BOOL
#define BOOL int
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 15

typedef enum
/*All reserved tokens*/ 
{
	ENDFILE,ERROR,
	/*reserved words 
	 if,else,while,read,write,return,int,void,real,bool,false,true
	 */
	IF,ELSE,WHILE,READ,WRITE,RETURN,INT,VOID,REAL,CHAR,STRING,BOOLTYPE,
	/*multicharacter tokens*/
	ID,NUMINT,NUMREAL,CHARVALUE,STRINGVALUE,BOOLVALUE,
	/*special symbols
	 =,==,+,-,*,/,(,),{,},[,],<,>,!=,;,,,++,--,+=,-=,*=,/=,#include
	 */
	ASSIGN,EQ,PLUS,MINUS,MULTIPLY,DIVIDE,LPAREN,
	RPAREN,LBRACE,RBRACE,LBRACKET,RBRACKET,LESSTHAN,
	MORETHAN,NOEQ,SEMI,COMMA,PLUSEQ,MINUSEQ,MULTIEQ,
	DIVIDEEQ,DBLPLUS,DBLMINUS,NOT,INCLUDE
}TokenType;
extern FILE* source;
extern FILE* listing;
extern FILE* code;
extern int lineno;

/***************************/
/* Syntax tree for parsing */
/***************************/
/*Nodekind contains of single expression,multiply expression,
 *function decleration and anotation expression.
 */
typedef enum {
	SingleExpr,
	MultiExpr,
	FunctionDeclr,
	AnotationExpr,
	Expr
	}NodeKind;
typedef enum {
	IncludeStat,
	AssignStat,
	DeclrStat,
	/*Io statement*/
	Read,Write,
	ReturnStat,
	FunctionStat
	}SingleExprKind;
typedef enum {WhileExpr,IfExpr}MultiExprKind;
typedef enum {FunctionExpr}FunctionDeclrKind;
/*I don't know whether I should process anotaion or not,
 *because anotation has been innocent.
 */
typedef enum {Anotaion}AnotationExprKind;
typedef enum {Id,ConstInt,ConstReal,ConstChar,ConstBool,ConstString,Array,PrefixExpr,SuffixExpr,Operation,Function}ExprKind;
typedef enum {Void,Int,Real,Bool,Char,String,NO}ExprType;
typedef enum {Identifier,Fidentifier,ArrayType}ExprStatType;
#define MAXCHILDREN 10
#define MAXPARAMS 10
typedef struct treeNode{
	struct treeNode *child[MAXCHILDREN];
	struct treeNode *sibling;
	struct treeNode *params[MAXPARAMS];
	int lineno;
	NodeKind nodeKind;
	union{
		SingleExprKind singleExprKind;
		MultiExprKind multiExprKind;
		FunctionDeclrKind functionDeclrKind;
		AnotationExprKind anotationExpr;
		ExprKind exprKind;
	}kind;
	struct{
		TokenType token;
		union{int integer;float real;char character;char *string;BOOL b;} val;
		char *name;
	}attr;
	ExprType type;
}TreeNode;
/*echo every line in file stream*/
extern int EchoSource;
/* TraceScan=TRUE causes token information to be 
 * printed to the listing file as each token is 
 * recognized by the scanner 
 */
extern int TraceScan;
/* TraceParse =TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children
 */
extern int TraceParse;
/* TraceAnalyze =TRUE causes symbol table inserts 
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;
/* TraceCode =TRUE causes comments to be written
 * to the CMM code files as code is generated 
 */
extern int TraceCode;
/*Error =TRUE prevents further passes if an error occurs*/
extern int IsPrint;
extern int Error;
#endif