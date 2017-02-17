/**************************************************/
/*File:scan.c                                     */
/*The scanner implementation for the CMM compiler */
/*Compiler Construction:Principles and Practice   */
/*Author:zhuchao 2015-12-29                       */
/**************************************************/
#include "scan.h"
#include "globals.h"
#include "util.h"
#define BUFLEN 256

static char lineBuf[BUFLEN];
static int bufsize=0;
static int linepos=0;
static int EOF_Flag=FALSE;
/* get next line from source code */
static char *getNextLine(void){
	if(fgets(lineBuf,BUFLEN-1,source)){
		bufsize=strlen(lineBuf);
		linepos=0;
		return lineBuf;
	}else{
		EOF_Flag=TRUE;
		return NULL;
	}
}
static AddressType getAddressType(PAddress address){
	if(address->position!=NULL){
		if(address->position[0]=='$')
			return NUM_A;
		if(address->r1==NO&&address->r2==NO&&address->times==NULL)
			return DIRC_A;
		if(address->r1!=NO&&address->r2==NO&&address->times==NULL)
			return EXCUR_A;
		if(address->r1!=NO&&address->r2!=NO&&address->times==NULL)
			return CHANGE_A;
		if(address->r1!=NO&&address->r2!=NO&&address->times!=NULL)
			return RATIO_A;
	}else{
		if(address->r1!=NO&&address->r2==NO&&address->times==NULL)
			return REG_A;
		if(address->r1!=NO&&address->r2!=NO&&address->times==NULL)
			return CHANGE_A;
		if(address->r1!=NO&&address->r2!=NO&&address->times!=NULL)
			return RATIO_A;
	}
}
static PAddress getAddress(char *s,unsigned int length){
	int startPos=0,currentPos=0;
	char *p[4]={NULL};
	int pos=0,r=0;
	int curr;
	PAddress address=(PAddress)malloc(sizeof(struct Address));
	address->position=NULL;
	address->r1=NO;
	address->r2=NO;
	address->times=NULL;
	for(currentPos=0;currentPos<length;currentPos++){
		curr=s[currentPos];
		if(curr=='('&&currentPos==0){
			startPos++;
		}else if(curr=='('&&currentPos>0){
			p[pos]=substr(s,startPos,currentPos-1);
			pos++;
			startPos=currentPos+1;
		}else if(curr==','){
			p[pos]=substr(s,startPos,currentPos-1);
			pos++;
			startPos=currentPos+1;
		}else if(curr==')'){
			p[pos]=substr(s,startPos,currentPos-1);
			pos++;
			startPos=currentPos+1;
		}
	}
	if(startPos!=currentPos)
		p[pos]=substr(s,startPos,currentPos-1);
	for(pos=0;pos<4;pos++)
		if(p[pos]!=NULL){
			p[pos]=clearSpace(p[pos]);
			if(p[pos][0]=='%'){
				if(r==0){
					address->r1=registerLookup(p[pos]);
					r++;
				}else if(r==1){
					address->r2=registerLookup(p[pos]);
				}
			}else{
				if(pos==0){
					address->position=p[pos];
				}else{
					address->times=p[pos];
				}
			}
		}
	address->aType=getAddressType(address);
	return address;
}
static PInstruction getInstruction(char *line){
	int startPos=0,currentPos=0;
	char *p[4]={NULL};
	char *tmp;
	int pos=0,r=0;
	int curr;
	int length=strlen(line);
	PAddress a=NULL;
	PInstruction instruction=(PInstruction)malloc(sizeof(struct Instruction));
	instruction->a1=NULL;
	instruction->a2=NULL;
	if(line[0]=='*'){
		return NULL;
		free(instruction);
	}
	for(currentPos=0;currentPos<length;currentPos++){
		curr=line[currentPos];
		if((curr=='\t'||curr==' ')&&startPos==currentPos){
			startPos++;
		}else if((curr=='\t'||curr==' ')&&startPos!=currentPos){
			p[pos]=substr(line,startPos,currentPos-1);
			pos++;
			startPos=currentPos+1;
		}
	}
	if(startPos!=currentPos)
		p[pos]=substr(line,startPos,currentPos-1);
	if(p[2]!=NULL){
		length=strlen(p[2]);
		int dot[3];
		startPos=0;
		for(currentPos=0;currentPos<length;currentPos++){
			curr=p[2][currentPos];
			if(curr==','){
				dot[startPos]=currentPos;
				startPos++;
			}
		}
		switch(startPos){
			case 0:
				break;
			case 1:
				tmp=p[2];
				p[2]=substr(tmp,0,dot[0]-1);
				p[3]=substr(tmp,dot[0]+1,length);
				free(tmp);
				break;
			case 2:
				if(p[2][dot[0]+1]=='('){
					tmp=p[2];
					p[2]=substr(tmp,0,dot[0]-1);
					p[3]=substr(tmp,dot[0]+1,length);
					free(tmp);
				}else if(p[2][dot[1]-1]==')'){
					tmp=p[2];
					p[2]=substr(tmp,0,dot[1]-1);
					p[3]=substr(tmp,dot[1]+1,length);
					free(tmp);
				}
				break;
			case 3:
				if(p[2][dot[0]+1]=='('){
					tmp=p[2];
					p[2]=substr(tmp,0,dot[0]-1);
					p[3]=substr(tmp,dot[0]+1,length);
					free(tmp);
				}else if(p[2][dot[2]-1]==')'){
					tmp=p[2];
					p[2]=substr(tmp,0,dot[2]-1);
					p[3]=substr(tmp,dot[2]+1,length);
					free(tmp);
				}
				break;
		}
	}
	instruction->position=atoi(substr(p[0],0,strlen(p[0])-1));
	free(p[0]);
	instruction->op=tokenLookup(p[1]);
	if(p[2]!=NULL){
		p[2]=clearSpace(p[2]);
		instruction->a1=getAddress(p[2],strlen(p[2]));
	}		
	if(p[3]!=NULL){
		p[3]=clearSpace(p[3]);
		instruction->a2=getAddress(p[3],strlen(p[3]));
	}
	return instruction;
}
/* get instructions in source file */
PInstruction instructions=NULL;
PInstruction getInstructions(){
	PInstruction temp=instructions;
	char *line;
	while(1){
		line=getNextLine();
		if(line!=NULL){
			PInstruction p=getInstruction(line);
			if(p==NULL)
				continue;
			if(temp==NULL){
				temp=p;
				instructions=temp;
			}else{
				temp->next=p;
				temp=p;
			}
		}else{
			break;
		}
	}
	return instructions;
}