#include "cmminterface_CMMInterface.h"
#include "globals.h"
#include "util.h"
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
/*
 State Num:
 -1:Error
 0:Success
 1:Arguement Error
 2:Out of memory
 3:Can't open project
 4:No source file
 5:Can't open src
 6:Create classes Error
 7:Link Error
 */
static int state=0;
static PFileList filelist=NULL;
/* insert a file information */
static void insertFile(char *name){
	PFileList p=(PFileList)malloc(sizeof(struct FileList));
	p->name=copyString(name);
	p->next=filelist;
	filelist=p;
}
/* create classes folder */
static int createClasses(const char *path){
	char *pathname=strplus(path,"/classes/");
	if(searchHeader(pathname)==-1){
		int state=mkdir(pathname,0777);
		if(state!=0){
			free(pathname);
			printf("Create classes failed\n");
			return -1;
		}
	}
	free(pathname);
	return 0;
}
/* run cmm and linker to compile all file in directory path*/
JNIEXPORT jint JNICALL Java_cmminterface_CMMInterface_compile(JNIEnv * e, jclass jc, jstring path){
	DIR *dp;
	struct dirent *dirp;
	FILE *fp;
	int compileError=0;
	state=0;
	filelist=NULL;
	if(path==NULL){
		state=1;
		jniThrowException(e,"java/lang/IllegalArguementException",NULL);
		return -1;
	}
	const char *pathStr=(*e)->GetStringUTFChars(e,path,NULL);
	if(pathStr==NULL){
		state=2;
		jniThrowException(e,"java/lang/RuntimeException","Out of memory");
		return -1;
	}
	if((dp=opendir(pathStr))==NULL){
		state=3;
		jniThrowException(e,"java/lang/Exception","Open Directory Error");
		return -1;
	}
	memset(buffer,'\0',BUFFER_SIZE);
	while((dirp=readdir(dp))!=NULL){
		if(!strcmp(dirp->d_name,"src"))
			strcpy(buffer,dirp->d_name);
	}
	/* close project directory */
	closedir(dp);
	if(buffer[0]=='\0'){
		state=4;
		jniThrowException(e,"java/lang/Exception","No Source File");
		return -1;
	}
	/* open source file directory */
	char *filename=strplus(pathStr,"/src");
	if((dp=opendir(filename))==NULL){
		free(filename);
		state=5;
		jniThrowException(e,"java/lang/Exception","Open Directory Error");
		return -1;
	}
	free(filename);
	/* insert all file that will be compiled*/
	while((dirp=readdir(dp))!=NULL){
		if(isCMM(dirp->d_name)!=-1){
			insertFile(dirp->d_name);
		}
	}
	/*compile all files*/
	PFileList tmp=filelist;
	while(tmp!=NULL){
		char *f=strplus(pathStr,"/src/");
		char *filename=strplus(f,tmp->name);
		free(f);
		fp=popen(strplus("cmm ",filename),"r");
		while(fgets(buffer,BUFFER_SIZE,fp)){
			printf("%s",buffer);
		}
		free(filename);
		pclose(fp);
		tmp=tmp->next;
	}
	/* create classes folder */
	if((createClasses(pathStr))==-1){
		state=6;
		jniThrowException(e,"java/lang/Exception","Create Classes Error");
		return -1;
	}
	/* move files */
	tmp=filelist;
	while(tmp!=NULL){
		char *f=strplus(pathStr,"/src/");
		char *t=cmmToCmc(tmp->name);
		char *filename=strplus(f,t);
		free(f);
		if(searchHeader(filename)==-1){
			printf("Compile file:%s failed.\n",filename);
			compileError=-1;
		}else{
			f=strplus(pathStr,"/classes/");
			char *x=strplus(f,t);
			rename(filename,x);
			free(f);
			free(x);
		}
		free(t);
		free(filename);
		tmp=tmp->next;
	}
	/* check file */
	if(compileError==-1){
		state=7;
		jniThrowException(e,"java/lang/Exception","Compile File Error");
		return -1;
	}
	/* linker all file */
	memset(buffer,'\0',BUFFER_SIZE);
	tmp=filelist;
	char *f=strplus(pathStr,"/classes/");
	while(tmp!=NULL){
		char *t=cmmToCmc(tmp->name);
		char *y=strplus(f,t);
		sprintf(buffer,"%s%s ",buffer,y);
		free(t);
		free(y);
		tmp=tmp->next;
	}
	free(f);
	f=getProjectName(pathStr);
	sprintf(buffer,"%s-o %s/%s",buffer,pathStr,f);
	free(f);
	printf("%s\n",buffer);
	fp=popen(strplus("linker ",buffer),"r");
	while(fgets(buffer,BUFFER_SIZE,fp)){
		printf("%s",buffer);
	}
	pclose(fp);
	/* release */
	(*e)->ReleaseStringUTFChars(e, path, pathStr);
	return 0;
}
JNIEXPORT void JNICALL Java_cmminterface_CMMInterface_run(JNIEnv * e, jclass jc, jstring path){
	FILE *fp;
	if(state!=0){
		jniThrowException(e,"java/lang/Exception","No excute file.");
		return;
	}
	if(path==NULL){
		jniThrowException(e,"java/lang/Exception","Path is NULL");
		return;
	}
	const char *pathStr=(*e)->GetStringUTFChars(e,path,NULL);
	char *filename=getProjectName(pathStr);
	char *f=strplus(pathStr,"/");
	char *excutename=strplus(f,filename);
	free(filename);
	free(f);
	if(searchHeader(excutename)==-1){
		jniThrowException(e,"java/lang/Exception","No excute file");
		return;
	}
	f=strplus("cvm ",excutename);
	system(f);
	free(f);
	return;
}
JNIEXPORT jstring JNICALL Java_cmminterface_CMMInterface_getResult(JNIEnv * e, jclass jc){
	return (*e)->NewStringUTF(e,"Success");
}
JNIEXPORT jint JNICALL Java_cmminterface_CMMInterface_getState(JNIEnv * e, jclass jc){
	return state;
}