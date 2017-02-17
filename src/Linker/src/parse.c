#include "parse.h"
#include "globals.h"
#include "scan.h"
#include "util.h"
/* the header of file list */
static PFileList filelist=NULL;
/* enviorment value */
static char *systemlibpath=NULL;
static char* getSystemLibPath(){
	char *name="CMM_LIB";
	systemlibpath=getenv(name);
	if(systemlibpath==NULL){
		fprintf(stderr,"No System Library Path");
		Error=TRUE;
	}
	return systemlibpath;
}
/* look up some file */
static int searchFile(char* name){
	PFileList tmp=filelist;
	while(tmp){
		if(!strcmp(name,tmp->filename))
			return 0;
		tmp=tmp->next;
	}
	return -1;
}
/* insert some file */
static void insertFile(PFileList file){
	PFileList tmp=filelist;
	if(tmp==NULL){
		filelist=file;
	}else{
		while(tmp->next!=NULL)tmp=tmp->next;
		tmp->next=file;
	}
}
static void insertInclude(PFileList file,PInclude include){
	PInclude tmp=file->includes;
	if(tmp==NULL){
		file->includes=include;
	}else{
		while(tmp->next!=NULL)tmp=tmp->next;
		tmp->next=include;
	}
}
static void insertGlobal(PFileList file,PGlobal global){
	PGlobal tmp=file->globals;
	if(tmp==NULL){
		file->globals=global;
	}else{
		while(tmp->next!=NULL)tmp=tmp->next;
		tmp->next=global;
	}
}
static void insertFunction(PFileList file,PFunction function){
	PFunction tmp=file->functions;
	if(tmp==NULL)
		file->functions=function;
	else{
		while(tmp->next!=NULL)tmp=tmp->next;
		tmp->next=function;
	}
}
static void freeInstruction(PInstruction instruction){
	PAddress address=NULL;
	if(instruction!=NULL){
		address=instruction->a1;
		if(address!=NULL){
			if(address->position!=NULL)
				free(address->position);
			if(address->times!=NULL){
				free(address->times);
			}
			free(address);
			address=NULL;
		}
		address=instruction->a2;
		if(address!=NULL){
			if(address->position!=NULL)
				free(address->position);
			if(address->times!=NULL){
				free(address->times);
			}
			free(address);
		}
		free(instruction);
		instruction=NULL;
	}
}
/*get all instructions of some file */
static PFileList getFile(PInstruction instructions){
	char *name=NULL;
	PInstruction instruction=NULL;
	PFileList file=(PFileList)malloc(sizeof(struct FileListRec));
	if(instructions->op==DOTFILE){
		name=instructions->a1->position;
		file->filename=copyString(name);
	}
	instructions=instructions->next;
	//freeInstruction(instructions);
	//instructions=instruction;
	while(instructions!=NULL&&instructions->op==DOTINCLUDE){
		name=instructions->a1->position;
		PInclude include=(PInclude)malloc(sizeof(struct Include));
		include->filename=copyString(name);
		insertInclude(file,include);
		instructions=instructions->next;
	}
	while(instructions!=NULL){
		PGlobal global=NULL;
		PFunction function=NULL;
		if(instructions->op==DOTGLOBAL){
			name=instructions->a1->position;
			instructions=instructions->next;
			if(instructions->op==DOTALIGN){
				global=(PGlobal)malloc(sizeof(struct Global));
				global->type=Void;
				global->name=copyString(name);
				global->position=copyString(instructions->a1->position);
				instructions=instructions->next->next->next;
				if(instructions->op==DOTGLOBAL){
					
				}else{
					switch(instructions->op){
						case DOTINT:
							global->type=Int;
							global->val=copyString(instructions->a1->position);
							break;
						case DOTREAL:
							global->type=Real;
							global->val=copyString(instructions->a1->position);
							break;
						case DOTCHAR:
							global->type=Char;
							global->val=copyString(instructions->a1->position);
							break;
						case DOTBOOL:
							global->type=Bool;
							global->val=copyString(instructions->a1->position);
							break;
					}
					instructions=instructions->next;
				}
			}else if(instructions->op==DOTDCLR){
				function=(PFunction)malloc(sizeof(struct Function));
				function->name=copyString(name);
				function->position=instructions->position+2;
				function->parent=file;
				instructions=instructions->next->next;
				PInstruction tmp=NULL;
				while(instructions!=NULL&&instructions->op!=DOTGLOBAL){
					if(function->instructions==NULL){
						function->instructions=instructions;
						tmp=function->instructions;
					}else{
						tmp->next=instructions;
						tmp=tmp->next;
					}
					instructions=instructions->next;
				}
				tmp->next=NULL;
			}
		}
		if(global!=NULL){
			insertGlobal(file,global);
		}else if(function!=NULL){
			insertFunction(file,function);
		}
	}
	return file;
}
/* insert system file */
static void getSystemFile(PInclude includes){
	PInstruction instructions=NULL,tmp=NULL;
	PFileList file=NULL;
	while(includes!=NULL){
		char *filename=strplus(systemlibpath,includes->filename);
		printf("%s\n",filename);
		if(searchHeader(filename)!=-1&&searchFile(filename)!=0){
			source=fopen(filename,"r");
			if(source==NULL){
				fprintf(stderr,"File %s not found",filename);
				Error=TRUE;
			}
			instructions=getInstructions();
			tmp=instructions;
			lineno=0;
			while(tmp!=NULL){
				printFile(tmp,listing);
				tmp=tmp->next;
			}
			file=getFile(instructions);
			insertFile(file);
			fclose(source);
		}
		free(filename);
		filename=NULL;
		includes=includes->next;
	}
}
/* get all file list */
PFileList getFileList(){
	PInstruction instructions=NULL,tmp=NULL;
	PFileList file=NULL;
	PInclude includes=NULL;
	int i;
	for(i=0;i<MAXFILENUM;i++){
		if(filename[i][0]!='\0'){
			source=fopen(filename[i],"r");
			if(source==NULL){
				fprintf(stderr,"File %s not found",filename[i]);
				Error=TRUE;
			}
			instructions=getInstructions();
			tmp=instructions;
			lineno=0;
			while(tmp!=NULL){
				printFile(tmp,listing);
				tmp=tmp->next;
			}
			file=getFile(instructions);	
			insertFile(file);
			fclose(source);
		}else{
			break;
		}
	}
	if(getSystemLibPath()!=NULL){
		/* insert system liberary */
		file=filelist;
		while(file!=NULL){
			includes=file->includes;
			getSystemFile(includes);
			file=file->next;
		}
	}
	return filelist;
}
