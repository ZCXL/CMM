#include "globals.h"
#include "util.h"
#include "combine.h"
static PFileList main=NULL;
static PFileList globalFileList=NULL;
/* function research map */
static PFunctionList functionsList=NULL;
/* identifier research map */
static PIdentifierList identifiersList=NULL;
/* print link error info */
static void linkError(char *msg){
	fprintf(listing,"Link Error:%s\n",msg);
	Error=TRUE;
}
/* insert a function name and absolute position */
static void insertFunc(char *name,int pos){
	PFunctionList list=(PFunctionList)malloc(sizeof(struct FunctionListRec));
	list->position=pos;
	list->name=copyString(name);
	PFunctionList temp=functionsList;
	if(temp==NULL){
		functionsList=list;
	}else{
		while(temp->next!=NULL)temp=temp->next;
		temp->next=list;
	}
}
/* look up a function and return it's absolute position */
static int lookupFunc(char *name){
	PFunctionList temp=functionsList;
	while(temp){
		if(strcmp(name,temp->name)==0)
			return temp->position;
		temp=temp->next;
	}
	return -1;
}
static int excurtion=0;
/* insert a new global variable */
static void insertIdentifier(char *name,int pos){
	PIdentifierList list=(PIdentifierList)malloc(sizeof(struct IdentifierListRec));
	list->position=excurtion;
	excurtion=pos;
	list->name=copyString(name);
	PIdentifierList temp=identifiersList;
	if(temp==NULL){
		identifiersList=list;
	}else{
		while(temp->next!=NULL)temp=temp->next;
		temp->next=list;
	}
}
/* look up a identifier */
static int lookupIdentifier(char *name){
	PIdentifierList temp=identifiersList;
	while(temp){
		if(strcmp(name,temp->name)==0)
			return temp->position;
		temp=temp->next;
	}
	return -1;
}
/* find main function */
static int findMain(PFileList filelist){
	PFunction functions=NULL;
	int isMain=0;
	while(filelist!=NULL){
		functions=filelist->functions;
		while(functions!=NULL){
			if(!strcmp("main",functions->name)){
				if(isMain==0){
					main=filelist;
					isMain++;
				}else{
					linkError("More than one main function.");
				}
				break;
			}
			functions=functions->next;
		}
		filelist=filelist->next;
	}
	if(main==NULL&&isMain==0){
		linkError("There is no main function.");
		return -1;
	}else if(isMain>1){
		return -1;
	}
	return 0;
}
static PFunction copyFunction(PFunction func){
	PFunction result=(PFunction)malloc(sizeof(struct Function));
	result->instructions=func->instructions;
	result->name=copyString(func->name);
	result->position=func->position;
	result->parent=func->parent;
	result->next=NULL;
	return result;
}
/* return some file list pointer */
static PFileList findSomeFile(char*filename){
	PFileList file=globalFileList;
	while(file!=NULL){
		if(!strcmp(filename,file->filename)){
			return file;
		}
		file=file->next;
	}
	return NULL;
}
static PFunction findSomeFunc(char*funcname,PInclude includes){
	PFileList file=NULL;
	PFunction functions=NULL;
	while(includes!=NULL){
		file=findSomeFile(includes->filename);
		if(file!=NULL){
			functions=file->functions;
			while(functions!=NULL){
				if(!strcmp(funcname,functions->name)){
					return functions;
				}
				functions=functions->next;
			}
		}
		includes=includes->next;
	}
	return NULL;
}
static int findSomeFuncSelf(char *funcname,PFunction functions){
	while(functions!=NULL){
		if(!strcmp(funcname,functions->name))
			return 0;
		functions=functions->next;
	}
	return -1;
}
static PFunction combineFunc(PFunction functions){
	PInstruction instructions=NULL;
	PFunction tmp=functions;
	PFunction func=NULL;
	PFunction list=NULL;
	PInclude includes=NULL;
	char *funcname=NULL;
	while(tmp!=NULL){
		instructions=tmp->instructions;
		includes=tmp->parent->includes;
		while(instructions!=NULL){
			if(instructions->op==CALL){
				funcname=instructions->a1->position;
				int i=-1;
				i=findSomeFuncSelf(funcname,functions);
				if(i!=-1){
					instructions=instructions->next;
					continue;
				}
				func=findSomeFunc(funcname,includes);
				if(func!=NULL){
					func=copyFunction(func);
					if(list==NULL){
						list=func;
					}else{
						func->next=list;
						list=func;
					}
				}else{
					printf("%s:No such function.\n",funcname);
					Error=TRUE;
				}
			}
			instructions=instructions->next;
		}
		tmp=tmp->next;
	}
	/* combine new functions */
	if(list!=NULL){
		list=combineFunc(list);
		PFunction tmp=list;
		while(tmp->next!=NULL)tmp=tmp->next;
		tmp->next=functions;
		functions=list;
	}
	return functions;
}
void combine(PFileList filelist){
	globalFileList=filelist;
	int top=0;
	int skipno=0;
	lineno=0;
	PAddress address=NULL;
	if(findMain(filelist)!=0){
		return;
	}
	fprintf(code,"%3d:  %5s  %s,%s\n",lineno++,"movl","$0x00002600","%edi");
	fprintf(code,"%3d:  %5s  %s,%s\n",lineno++,"movl","$0x000025ff","%ebp");
	fprintf(code,"%3d:  %5s  %s,%s\n",lineno++,"movl","$0x000025ff","%esp");
	PInclude includes=main->includes;
	PGlobal globals=main->globals;
	PFunction functions=main->functions;
	functions=combineFunc(functions);
	PFunction tmp=functions;
	while(globals!=NULL){
		insertIdentifier(globals->name,hexToNum(globals->position));
		switch (globals->type){
			case Void:
				break;
			case Int:
				address=(PAddress)malloc(sizeof(struct Address));
				top=lookupIdentifier(globals->name);
				address->position=numToStr(top);
				address->r1=EDI;
				address->aType=EXCUR_A;
				fprintf(code,"%3d:  %5s  %s,%s\n",lineno,"movl",strplus("$",globals->val),getStyle(address));
				lineno++;
				free(address);
				break;
			case Real:
				address=(PAddress)malloc(sizeof(struct Address));
				top=lookupIdentifier(globals->name);
				address->position=numToStr(top);
				address->r1=EDI;
				address->aType=EXCUR_A;
				fprintf(code,"%3d:  %5s  %s,%s\n",lineno,"movlabs",strplus("$",globals->val),getStyle(address));
				free(address);
				lineno++;
				break;
			case Bool:
			case Char:
				address=(PAddress)malloc(sizeof(struct Address));
				top=lookupIdentifier(globals->name);
				address->position=numToStr(top);
				address->r1=EDI;
				address->aType=EXCUR_A;
				fprintf(code,"%3d:  %5s  %s,%s\n",lineno,"movb",strplus("$",globals->val),getStyle(address));
				free(address);
				lineno++;
				break;
		}
		globals=globals->next;
	}
	skipno=lineno;
	lineno+=4;
	IsPrint=FALSE;
	while(functions!=NULL){
		insertFunc(functions->name,lineno);
		PInstruction instructions=functions->instructions;
		int pos=-1;
		int offset=0;
		while(instructions!=NULL){
			if(instructions->op==CALL){
				pos=lookupFunc(instructions->a1->position);
				if(pos==-1){
					linkError("No Decleration.");
				}
				instructions->a1->position=getHex(Int,&pos);
			}else{
				if(instructions->a1!=NULL&&instructions->a1->position!=NULL){
					if(instructions->a1->aType==DIRC_A){
						pos=hexToNum(instructions->a1->position);
						offset=pos-instructions->position;
						pos=lineno+offset;
						instructions->a1->position=getHex(Int,&pos);
					}else{
						pos=lookupIdentifier(instructions->a1->position);
						if(pos!=-1)
							instructions->a1->position=numToStr(pos);
					}
				}
				if(instructions->a2!=NULL&&instructions->a2->position!=NULL){
					if(instructions->a2->aType==DIRC_A){
						pos=hexToNum(instructions->a2->position);
						offset=pos-instructions->position;
						pos=lineno+offset;
						instructions->a2->position=getHex(Int,&pos);
					}else{
						pos=lookupIdentifier(instructions->a2->position);
						if(pos!=-1)
							instructions->a2->position=numToStr(pos);
					}
				}
			}
			printFile(instructions,code);
			instructions=instructions->next;
		}
		functions=functions->next;
	}
	IsPrint=TRUE;
	int pos=lookupFunc("main");
	if(pos==-1){
		linkError("No main function.");
	}else{
		fprintf(code,"%3d:  %5s  %s,%s\n",skipno++,"subl","$0x00000080","%esp");
		fprintf(code,"%3d:  %5s  %s,%s\n",skipno++,"movl",strplus("$",getHex(Int,&lineno)),"-4(%esp)");
		fprintf(code,"%3d:  %5s  %s,%s\n",skipno++,"subl","$0x00000004","%esp");
		fprintf(code,"%3d:  %5s  %s,%s\n",skipno++,"leal",getHex(Int,&pos),"%eip");
	}
	printToFile();
}