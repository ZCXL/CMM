/****************************************************/
/* File:main.c                                      */
/* Main program for CMM compiler                    */
/* Compiler Construction : Principles and Practise  */
/* Author :zhuchao 2015-10-8                        */
/****************************************************/
#include "globals.h"


/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parse-only compiler */
#define NO_ANALYZE FALSE
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno=0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource= TRUE;
int TraceScan=TRUE;
int TraceParse=TRUE;
int TraceAnalyze=TRUE;
int TraceCode=TRUE;
int IsPrint=TRUE;

int Error=FALSE;
TokenType type;

/* main function */
int main(int argc,char*argv[]){
	TreeNode * syntaxTree;
	char pgm[120];
	if(argc!=2){
		fprintf(stderr,"usage:%s <filename> \n",argv[0]);
		exit(1);
	}
	strcpy(pgm,argv[1]);
	if(strchr(pgm,'.')==NULL)
		strcat(pgm,".cmm");
	source = fopen(pgm,"r");
	if(source==NULL){
		fprintf(stderr,"File %s not found",pgm);
		exit(1);
	}
	listing=stdout;
	fprintf(listing,"\nCMM COMPILATION:%s\n",pgm);
#if NO_PARSE
	do{
		type=getToken();
	}while(type!=ENDFILE&&type!=ERROR);
#else
	syntaxTree=parse();
	if(TraceParse){
		fprintf(listing,"\nSyntax tree:\n");
		printTree(syntaxTree);
	}
#if !NO_ANALYZE
	if(Error){
		freeSyntaxTree(syntaxTree);
		exit(0);
	}
	if(!Error){
		fprintf(listing,"\nBuilding Symbol Table...\n");
		buildSymTab(syntaxTree);
		fprintf(listing,"\nBuild Symbol Table Finished\n");
	}
	if(!Error){
		fprintf(listing,"\nChecking Type...\n");
		typeCheck(syntaxTree);
		fprintf(listing,"Type Checking Finished\n");
	}
	if(Error){
		freeSyntaxTree(syntaxTree);
		freeSymbolTable();
		exit(0);
	}
#if !NO_CODE
	if(!Error){
		char *codefile=cmmToCmc(pgm);
		code=fopen(codefile,"w");
		if(code==NULL){
			printf("Unable to open %s\n",codefile);
			exit(0);
		}
		fprintf(listing,"\nCompiling File...\n");
		codeGen(syntaxTree,codefile);
		fclose(code);
		freeSyntaxTree(syntaxTree);
		freeSymbolTable();
		if(!Error){
			fprintf(listing,"Compile Finished\n");
		}else{
			fprintf(listing,"Compile Error\n");
		}
	}
#endif
#endif
#endif
	fclose(source);
	return 0;
}