/**************************************************/
/* File: symtab.c                                 */
/* Symbol table interface for the CMM compiler    */
/* (allows only one symbol table )                */
/* Compiler Construction: Principles and Practice */
/* Author:zhuchao 2015-10-11                      */
/**************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"
#include "util.h"

/* SHIFT is the power of two used as multiplier
 in hash function*/
#define SHIFT 4
typedef enum{
	MultiDeclr,NoDeclr,MixDeclr
}ErrorType;
static void printError(int lineno,char *name,ErrorType type){
	switch(type){
		case MultiDeclr:
			fprintf(listing,"Error occur at line:%d -> Variable \'%s\' is declared many times\n",lineno,name);
			break;
		case NoDeclr:
			fprintf(listing,"Error occur at line:%d -> Variable \'%s\' must be declared before used\n",lineno,name);
			break;
		case MixDeclr:
			fprintf(listing,"Error occur at line:%d -> Variable \'%s\' must be diffirent among array,function and identifier\n",lineno,name);
			break;
		default:
			break;
	}
	Error=TRUE;
}
/* the hash function */
static int hash(char *key){
	int temp=0;
	int i=0;
	while(key[i]!='\0'){
		temp=((temp << SHIFT )+key[i])%SIZE;
		i++;
	}
	return temp;
} 
FieldList field;
FieldList currentList;
/* the hash table
 * chaining
 * open addressing
 */
static BucketList *hashTable;
/* Procedure st_insert inserts line numbers and 
 * memory locations into the symbol table 
 * loc=memory location is inserted only the 
 * first name,otherwise ignored
 */
void st_insert(char *name,int lineno,int loc,ExprType type,ExprStatType sType){
	int h=hash(name);
	BucketList l=hashTable[h];
	while(l!=NULL&&strcmp(name,l->name)!=0)
		l=l->next;
	if(l==NULL){
		l=(BucketList)malloc(sizeof(struct BucketListRec));
		l->name=copyString(name);
		l->lines=(LineList)malloc(sizeof(struct LineListRec));
		l->lines->lineno=lineno;
		l->memloc=loc;
		l->level=currentList->level;
		l->type=type;
		l->sType=sType;
		l->lines->next=NULL;
		/* insert at first position */
		l->next=hashTable[h];
		hashTable[h]=l;
	}else{
		/*insert next referenced position */
		if(l->type!=NO){
			if(type!=NO&&type!=l->type)
				/* same variable is declared as diffirent type */
				printError(lineno,l->name,MultiDeclr);
			if(type==l->type)
				/* same variable is declared as same type */
				printError(lineno,l->name,MultiDeclr);
		}
		if(l->sType!=sType){
			/* variable should be declared diffirently between function and identifier */
			printError(lineno,l->name,MixDeclr);
		}
		LineList t=l->lines;
		while(t->next!=NULL)t=t->next;
		t->next=(LineList)malloc(sizeof(struct LineListRec));
		t->next->lineno=lineno;
		t->next->next=NULL;
	}
}
/* Procedure insert_params add params to function declare */
void insert_params(char *name,char *param_name,int lineno,ExprType type){
	BucketList l=lookup(name,lineno);
	if(l!=NULL){
		l->params[l->params_count].type=type;
		l->params[l->params_count].name=copyString(param_name);
		l->params_count++;
	}
}
/* insert array's size */
void insert_size(char *name,int lineno,int size){
	BucketList l=lookup(name,lineno);
	if(l!=NULL)
		l->size=size;
}
/* Function st_lookup returns the memory
 * location of a variable or -1 if not found 
 */
int st_lookup(char *name){
	int h=hash(name);
	BucketList l=hashTable[h];
	while(l!=NULL&&strcmp(name,l->name)!=0){
		l=l->next;
	}
	if(l==NULL){
		return -1;
	}
	else
		return l->memloc;
	return -1;
}
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents
 * to the listing File
 */
void printSymTab(FILE *listing){
	int i;
	fprintf(listing,"Variable Name	Location	Line Numbers\n");
	fprintf(listing,"-------------------------------------------\n");
	for(i=0;i<SIZE;++i){
		if(hashTable[i]!=NULL){
			BucketList l=hashTable[i];
			while(l!=NULL){
				LineList t=l->lines;
				fprintf(listing,"%-15s ",l->name);
				fprintf(listing,"%-14d ",l->memloc);
				while(t!=NULL){
					fprintf(listing,"%4d ",t->lineno);
					t=t->next;
				}
				fprintf(listing,"\n");
				l=l->next;
			}
		}
	}
}
/* Procedure createFieldList create filed list at first*/
void createFieldList(){
	field=(FieldList)malloc(sizeof(struct FieldListRec));
	field->last=NULL;
	field->position=1;
	field->level=1;
	currentList=field;
	hashTable=currentList->hashTable;
}
/* Procedure getNextHashTable create a new hash table
 * linked with current filed 
 
currentList
 |------|
 |      |<--------------|<-------|<---------|
 |      |               |		 |			|
 |		|----->child->sibling->sibling->sibling........
 |		|
 |		|
 |		|
 |		|
 |------|
 223,so beautiful data structure.
 */
void getNextHashTable(int lineno){
	FieldList temp=currentList->child;
	while(temp!=NULL&&temp->position!=lineno)
		temp=temp->sibling;
	if(temp==NULL){
		FieldList child=(FieldList)malloc(sizeof(struct FieldListRec));
		child->position=lineno;
		child->level=currentList->level+1;
		child->last=currentList;
		child->sibling=currentList->child;
		currentList->child=child;
		currentList=child;
		hashTable=currentList->hashTable;
	}else{
		currentList=temp;
		hashTable=currentList->hashTable;
	}
}
/* Procedure getLastHashTable return last level hash table*/
void getLastHashTable(){
	currentList=currentList->last;
	if(currentList!=NULL)
		hashTable=currentList->hashTable;
}

BucketList lookup(char *name,int lineno){
	BucketList b=NULL;
	FieldList f=currentList;
	while(f!=NULL){
		hashTable=f->hashTable;
		int h=hash(name);
		BucketList l=hashTable[h];
		while(l!=NULL&&strcmp(name,l->name)!=0){
			l=l->next;
		}
		if(l!=NULL&&l->type!=NO){
			b=l;
			hashTable=currentList->hashTable;
			return b;
		}
		f=f->last;
	}
	hashTable=currentList->hashTable;
	return b;
}
/* function getCurrentList is used to get current FieldList */
FieldList getCurrentList(){
	return currentList;
}
static void freeField(FieldList field){
	FieldList f=NULL;
	BucketList *table=NULL;
	BucketList item=NULL,temp=NULL;
	LineList line=NULL;
	if(field->child!=NULL){
		f=field->child;
		while(f!=NULL){
			if(f->child!=NULL)
				freeField(f->child);
			freeField(f);
			f=f->sibling;
		}
	}
	table=field->hashTable;
	int i,j;
	for(i=0;i<SIZE;i++){
		if(table[i]!=NULL){
			item=table[i];
			while(item!=NULL){
				free(item->name);
				item->name=NULL;
				for(j=0;j<item->params_count;j++){
					if(item->params[j].name!=NULL){
						free(item->params[j].name);
						item->params[j].name=NULL;
					}
				}
				line=item->lines;
				while(line!=NULL){
					LineList l=line;
					line=line->next;
					free(l);
					l=NULL;
				}
				temp=item;
				item=item->next;
				free(temp);
				temp=NULL;
			}
		}
	}
}
/* function freeSymbolTable is used to free all
 * information in symbol table
 */
void freeSymbolTable(){
	freeField(field);
}