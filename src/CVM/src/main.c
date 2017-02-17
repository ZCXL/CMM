/*************************************************/
/* File: cm.c                                    */
/* The CMMM virtual machine                      */
/* Compiler Construction:Principles and Practice */
/* Author:zhuchao 2015-12-8                      */
/*************************************************/
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "excute.h"
FILE * source;
FILE * listing;
int lineno=0;
int Error=FALSE;
int IsPrint=TRUE;
int main(int argc,char*argv[]){
	PInstruction instructions=NULL;
	char pgm[120];
	if(argc!=2){
		fprintf(stderr,"usage:%s <filename> \n",argv[0]);
		exit(1);
	}
	strcpy(pgm,argv[1]);
	source = fopen(pgm,"r");
	if(source==NULL){
		fprintf(stderr,"File %s not found",pgm);
		exit(1);
	}
	listing=stdout;
	instructions=getInstructions();
	excute(instructions);
}