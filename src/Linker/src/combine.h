#ifndef _COMBINE_H_
#define _COMBINE_H_
#include "globals.h"
/* function structure */
typedef struct FunctionListRec{
	int position;
	char *name;
	struct FunctionListRec *next;
}FunctionList,*PFunctionList;
typedef struct IdentifierListRec{
	char *name;
	int position;
	struct IdentifierListRec *next;
}IdentifierList,*PIdentifierList;
/* combine all files */
extern void combine(PFileList filelist);
#endif