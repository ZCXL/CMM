/**************************************************/
/* File: code.c                                   */
/* Code emitting utilites for the CMM compiler    */
/* Compiler Construction: Principles and Practice */
/* Author: zhuchao 2015-10-26                     */
/**************************************************/
#include "globals.h"
#include "code.h"
#include "symtab.h"
#include "limits.h"
#include "util.h"

/* code file buffer */
static PFileList pfile=NULL;
/* create a new list to save each line buffer*/
static PFileList createList(){
	PFileList p=(PFileList)malloc(sizeof(struct FileList));
	return p;
}
/* insert a line buffer */
static void insertList(PFileList p){
	if(pfile==NULL){
		pfile=p;
	}else{
		PFileList temp=pfile;
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next=p;
	}
}
/* free buffer space */
static void freeList(PFileList pfile){
	PFileList temp=NULL;
	while(pfile!=NULL){
		temp=pfile->next;
		free(pfile->string);
		pfile->string=NULL;
		free(pfile);
		pfile=temp;
	}
	pfile=NULL;
}
/* print buffer lines to file */
void printToFile(){
	PFileList temp=pfile;
	while(temp!=NULL){
		fprintf(code,"%s",temp->string);
		temp=temp->next;
	}
	freeList(pfile);
}
/*register words*/
static struct {
	char*str;
	RegisterType rType;
}registers[24]
	={
		{"%eax",EAX},{"%ax",AX},{"%ah",AH},{"%al",AL},
		{"%ecx",ECX},{"%cx",CX},{"%ch",CH},{"%cl",CL},
		{"%edx",EDX},{"%dx",DX},{"%dh",DH},{"%dl",DL},
		{"%ebx",EBX},{"%bx",BX},{"%bh",BH},{"%bl",BL},
		{"%eip",EIP},{"%ip",IP},
		{"%edi",EDI},{"%di",DI},
		{"%esp",ESP},{"%sp",SP},
		{"%ebp",EBP},{"%bp",BP}
	};
/* look up some register by using name */
static char* registerLookup(RegisterType rType){
	int i;
	for(i=0;i<24;i++)
		if(rType==registers[i].rType)
			return registers[i].str;
	return NULL;
}
/* line buffer */
static char buffer[BUFFER_SIZE];
/* CMM location number for current instruction emission */
static int emitLoc=0;
/* highest CMM location emitted so far 
 * For use in statements with emitSkip,
 * emitBackUp,and emitRestore
 */
static int highEmitLoc=0;
/* Procedure emitComment prints a comment line 
 * with comment c in the code file
 */
void emitComment(char *c){
	if(TraceCode){
		if(IsPrint){
			fprintf(code,"* %s\n",c);
		}else{
			PFileList p=createList();
			memset(buffer,'\0',BUFFER_SIZE);
			int size=sprintf(buffer,"* %s\n",c);
			p->string=(char*)malloc(sizeof(char)*(size+1));
			strcpy(p->string,buffer);
			insertList(p);
		}
	}
}
/* Procedure emitSingle emits a register-only
 * CMM instruction
 * op = the operation code
 * r = target register
 */
void emitSingle(char *op,char *r){
	if(IsPrint){
		fprintf(code,"%3d:  %5s  %s",emitLoc++,op,r);
		fprintf(code,"\n");
	}else{
		PFileList p=createList();
		memset(buffer,'\0',BUFFER_SIZE);
		int size=sprintf(buffer,"%3d:  %5s  %s",emitLoc++,op,r);
		size+=sprintf(buffer,"%s\n",buffer);
		p->string=(char*)malloc(sizeof(char)*(size+1));
		strcpy(p->string,buffer);
		insertList(p);
	}
	if(highEmitLoc<emitLoc)highEmitLoc=emitLoc;
}
void emit(char *op){
	if(IsPrint){
		fprintf(code,"%3d:  %5s",emitLoc++,op);
		fprintf(code,"\n");
	}else{
		PFileList p=createList();
		memset(buffer,'\0',BUFFER_SIZE);
		int size=sprintf(buffer,"%3d:  %5s",emitLoc++,op);
		size+=sprintf(buffer,"%s\n",buffer);
		p->string=(char*)malloc(sizeof(char)*(size+1));
		strcpy(p->string,buffer);
		insertList(p);
	}
	if(highEmitLoc<emitLoc)highEmitLoc=emitLoc;
}
/* Procedure emitDouble emits a register-only
 * CMM instruction
 * op = the operation code
 * r = target register
 * s = 1st source register
 */
void emitDouble(char *op,char *r,char *s){
	if(IsPrint){
		fprintf(code,"%3d:  %5s  %s,%s",emitLoc++,op,r,s);
		fprintf(code,"\n");	
	}else{
		PFileList p=createList();
		memset(buffer,'\0',BUFFER_SIZE);
		int size=sprintf(buffer,"%3d:  %5s  %s,%s",emitLoc++,op,r,s);
		size=sprintf(buffer,"%s\n",buffer);
		p->string=(char*)malloc(sizeof(char)*(size+1));
		strcpy(p->string,buffer);
		insertList(p);
	}
	if(highEmitLoc<emitLoc)highEmitLoc=emitLoc;
}
/* declare a function and insert it's position */
void emitFunction(char *op,char *f){
	insertFunc(f,emitLoc);//insert function location;
	fprintf(code,"%3d:  %5s  %s",emitLoc++,op,f);
	fprintf(code,"\n");	
	if(highEmitLoc<emitLoc)highEmitLoc=emitLoc;
}
/* escape and restore*/
int emitSkip(int howMany){
	int i=emitLoc;
	emitLoc+=howMany;
	if(highEmitLoc<emitLoc)highEmitLoc=emitLoc;
	return i;
}
void emitBackup(int loc){
	if(loc>highEmitLoc)emitComment("BUG in emitBackup");
	emitLoc=loc;
}
void emitRestore(void){
	emitLoc=highEmitLoc;
}
/* Procedure emitRM_Abs converts an absolute reference 
 * to a pc-relative reference when emitting a 
 * register-to-memory CMM instruction
 * op = the operation code
 * r = target register
 * a = the absolute location in memory
 */
void emitAbs(char *op,RegisterType r,int a){
	int length=a-(emitLoc+1);
	if(IsPrint){
		fprintf(code,"%3d:  %5s  %s,%s",emitLoc,op,getStyle(0,EAX,0,0,REG_A),getStyle(length,EIP,0,0,EXCUR_A));
		++emitLoc;
		fprintf(code,"\n");
	}else{
		PFileList p=createList();
		memset(buffer,'\0',BUFFER_SIZE);
		int size=sprintf(buffer,"%3d:  %5s  %s,%s",emitLoc,op,getStyle(0,EAX,0,0,REG_A),getStyle(length,EIP,0,0,EXCUR_A));
		++emitLoc;
		size=sprintf(buffer,"%s\n",buffer);
		p->string=(char*)malloc(sizeof(char)*(size+1));
		strcpy(p->string,buffer);
		insertList(p);
	}
	if(highEmitLoc<emitLoc)highEmitLoc=emitLoc;
}
/* function research map */
FunctionList functions=NULL;
/* insert a function name and absolute position */
void insertFunc(char *name,int pos){
	FunctionList list=(FunctionList)malloc(sizeof(struct FunctionListRec));
	list->position=pos;
	list->name=copyString(name);
	FunctionList temp=functions;
	if(temp==NULL){
		functions=list;
	}else{
		while(temp->next!=NULL)temp=temp->next;
		temp->next=list;
	}
}
/* look up a function and return it's absolute position */
int lookupFunc(char *name){
	FunctionList temp=functions;
	while(temp){
		if(strcmp(name,temp->name)==0)
			return temp->position;
		temp=temp->next;
	}
	return -1;
}
void freeFunc(){
	FunctionList temp=NULL;
	while(functions!=NULL){
		temp=functions->next;
		free(functions->name);
		functions->name=NULL;
		free(functions);
		functions=temp;
	}
	functions=NULL;
}
/* global variable map */
IdentifierList identifers=NULL;
int excurtion=0;
/* insert a new global variable */
void insertIdentifier(char *name,int pos){
	IdentifierList list=(IdentifierList)malloc(sizeof(struct IdentifierListRec));
	list->position=excurtion;
	excurtion=pos;
	list->name=copyString(name);
	IdentifierList temp=identifers;
	if(temp==NULL){
		identifers=list;
	}else{
		while(temp->next!=NULL)temp=temp->next;
		temp->next=list;
	}
}
/* look up a identifier */
int lookupIdentifier(char *name){
	IdentifierList temp=identifers;
	while(temp){
		if(strcmp(name,temp->name)==0)
			return temp->position;
		temp=temp->next;
	}
	return -1;
}
void freeIdentifier(){
	IdentifierList temp=NULL;
	while(identifers!=NULL){
		temp=identifers->next;
		free(identifers->name);
		identifers->name=NULL;
		free(identifers);
		identifers=temp;
	}
	identifers=NULL;
}
/* eight kinds of address way */
char *getStyle(int p,RegisterType r1,RegisterType r2,int i,AddressType aType){
	int size;
	char *result;
	switch(aType){
		case NUM_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"$%s",getHex(Int,&p));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case REG_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%s",registerLookup(r1));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case DIRC_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%s",getHex(Int,&p));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case INDIRC_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"(%s)",registerLookup(r1));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case EXCUR_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%d(%s)",p,registerLookup(r1));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case CHANGE_A:
			memset(buffer,'\0',BUFFER_SIZE);
			if(p!=INT_MIN){
				size=sprintf(buffer,"%d(%s,%s)",p,registerLookup(r1),registerLookup(r2));
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}else{
				size=sprintf(buffer,"(%s,%s)",registerLookup(r1),registerLookup(r2));
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}
			break;
		case RATIO_A:
			memset(buffer,'\0',BUFFER_SIZE);
			if(p!=INT_MIN){
				size=sprintf(buffer,"%d(%s,%s,%d)",p,registerLookup(r1),registerLookup(r2),i);
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}else{
				size=sprintf(buffer,"(%s,%s,%d)",registerLookup(r1),registerLookup(r2),i);
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}
			break;
	}
	return result;
}