#include "globals.h"
#include "scan.h"
#include "parse.h"
#include "combine.h"

FILE * source;
FILE * listing;
FILE * code;
char filename[MAXFILENUM][120];
int lineno=0;
int Error=FALSE;
int IsPrint=TRUE;
int main(int argc,char*argv[]){
	PFileList filelist=NULL;
	char excutename[100]="a.out";
	char pgm[120];
	if(argc<2){
		fprintf(stderr,"usage:%s <filenames>... [-o][excute file name] \n",argv[0]);
		exit(1);
	}
	int i,j;
	for(i=1,j=0;i<argc;i++){
		if(!strcmp("-o",argv[i])){
			memset(excutename,'\0',100);
			strcpy(excutename,argv[i+1]);
			i++;
		}else{
			strcpy(filename[j],argv[i]);
			j++;
		}
	}
	listing=stdout;
	filelist=getFileList();
	if(!Error){
		code=fopen(excutename,"w");
		if(code==NULL){
			printf("Unable to open %s\n",excutename);
			exit(0);
		}
		combine(filelist);
		if(!Error){
			printf("Link Successfully\n");
			fclose(code);
		}else{
			fprintf(stderr,"Link Error\n");
			remove(excutename);
			exit(1);
		}
	}else{
		fprintf(stderr,"Load Error\n");
		exit(1);
	}
	
}