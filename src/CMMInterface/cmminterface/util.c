#include "globals.h"
#include "util.h"
/* Function copyString allocates and makes a new 
* copy of an existing copyString
*/
char *copyString(char *s){
	int n;
	char *t;
	if(s==NULL)
		return NULL;
	n=strlen(s)+1;
	t=(char*)malloc(n);
	if(t==NULL){ 
		printf("Out of memory error\n");
	}else
		strcpy(t,s);
	return t;
}
/* add two strings together */
char *strplus(const char *first,const char *second){
	int length=strlen(first)+strlen(second)+1;
	char *result=(char*)malloc(length);
	memset(result,'\0',length);
	strcat(result,first);
	strcat(result,second);
	return result;
}
/* cut string */
char* substr(const char *source,unsigned int start,unsigned int end){
	int i;
	int length=end-start+2;
	char *result=(char*)malloc(sizeof(char)*length);
	for(i=0;i<length-1;i++)
		result[i]=source[start+i];
	result[length-1]='\0';
	return result;
}
/* check file wheather is cmm file*/
int isCMM(char *name){
	int length=strlen(name);
	if(length>4){
		char *result=substr(name,length-4,length-1);
		if(!strcmp(result,".cmm")){
			return 0;
		}
	}else{
		return -1;
	}
	return -1;
}
/* change file name */
char *cmmToCmc(char *filename){
	int length=strlen(filename);
	char *result=(char*)malloc(length+1);
	memset(result,'\0',length+1);
	int i=0;
	for(i=0;i<length-1;i++)
		result[i]=filename[i];
	result[length-1]='c';
	return result;
}
/* search some header file*/
int searchHeader(char *name){
	return access(name,F_OK);
}
/* get project's name */
char *getProjectName(const char *name){
	int length=strlen(name);
	int i;
	int position=-1;
	for(i=length-1;i>=0;i--){
		if(name[i]=='/'){
			position=i;
			break;
		}
	}
	if(position==-1){
		return NULL;
	}
	char *result=substr(name,position+1,length-1);
	return result;
}