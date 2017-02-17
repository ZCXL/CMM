#include "globals.h"
#include "util.h"
/*register words*/
static struct {
	char*str;
	RegisterType rType;
}registers[MAXREGISTER]
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
/* instuction map */
static struct{
	char *name;
	TokenType tok;
}instructionnames[MAXINSTRUCTION]
	={
		{".file",DOTFILE},{".include",DOTINCLUDE},{".global",DOTGLOBAL},{".align",DOTALIGN},
		{".size",DOTSIZE},{".type",DOTTYPE},{".Int",DOTINT},{".Real",DOTREAL},{".Char",DOTCHAR},
		{".Bool",DOTBOOL},{".dclr",DOTDCLR},{"push",PUSH},{"pushl",PUSHL},{"pushlabs",PUSHLABS},
		{"pushb",PUSHB},{"pushw",PUSHW},{"pop",POP},{"popl",POPL},{"poplabs",POPLABS},{"popb",POPB},
		{"popw",POPW},{"mov",MOV},{"movl",MOVL},{"movb",MOVB},{"movw",MOVW},{"movlabs",MOVLABS},
		{"leal",LEAL},{"inc",INC},{"neg",NEG},{"dec",DEC},{"not",NOT},{"addl",ADDL},{"addlabs",ADDLABS},
		{"addb",ADDB},{"addw",ADDW},{"subl",SUBL},{"subl",SUBLABS},{"subb",SUBB},{"subw",SUBW},{"mull",MULL},
		{"mullabs",MULLABS},{"mulb",MULB},{"mulw",MULW},{"divl",DIVL},{"divlabs",DIVLABS},{"divb",DIVB},{"divw",DIVW},
		{"cmpl",CMPL},{"je",JE},{"jne",JNE},{"jg",JG},{"jl",JL},{"jge",JGE},{"jle",JLE},{"call",CALL},{"ret",RET},
		{"leave",LEAVE},{"in",IN},{"inl",INL},{"inlabs",INLABS},{"inb",INB},{"inw",INW},
		{"out",OUT},{"outl",OUTL},{"outlabs",OUTLABS},{"outb",OUTB},{"outw",OUTW},{"outp",OUTP}
	};
char *clearSpace(char *str){
	int length=strlen(str);
	if(str[length-1]=='\n')
		return substr(str,0,length-2);
	return str;
}
/* look up some register by using name */
RegisterType registerLookup(char *name){
	int i;
	name=clearSpace(name);
	for(i=0;i<MAXREGISTER;i++)
		if(!strcmp(name,registers[i].str))
			return registers[i].rType;
	return NO;
}
char *lookupRegister(RegisterType rType){
	int i;
	for(i=0;i<MAXREGISTER;i++)
		if(rType==registers[i].rType)
			return registers[i].str;
	return NULL;
}
/* look up some instuction */
TokenType tokenLookup(char *name){
	int i;
	name=clearSpace(name);
	for(i=0;i<MAXINSTRUCTION;i++)
		if(!strcmp(name,instructionnames[i].name))
			return instructionnames[i].tok;
	return ADDRESS;
}
char *lookupToken(TokenType tok){
	int i;
	for(i=0;i<MAXINSTRUCTION;i++)
		if(tok==instructionnames[i].tok)
			return instructionnames[i].name;
	return NULL;
}
/* code file buffer */
static PBufferList pbuffer=NULL;
/* create a new list to save each line buffer*/
static PBufferList createList(){
	PBufferList p=(PBufferList)malloc(sizeof(struct BufferList));
	return p;
}
/* insert a line buffer */
static void insertList(PBufferList p){
	if(pbuffer==NULL){
		pbuffer=p;
	}else{
		PBufferList temp=pbuffer;
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next=p;
	}
}
/* free buffer space */
static void freeList(){
	PBufferList temp=NULL;
	while(pbuffer!=NULL){
		temp=pbuffer->next;
		free(pbuffer->string);
		free(pbuffer);
		pbuffer=temp;
	}
	pbuffer=NULL;
}
/* print buffer lines to file */
void printToFile(){
	PBufferList temp=pbuffer;
	while(temp!=NULL){
		fprintf(code,"%s",temp->string);
		temp=temp->next;
	}
	pbuffer=NULL;
	freeList();
}
/* cut string */
char* substr(char *source,unsigned int start,unsigned int end){
	int i;
	int length=end-start+2;
	char *result=(char*)malloc(sizeof(char)*length);
	for(i=0;i<length-1;i++)
		result[i]=source[start+i];
	result[length-1]='\0';
	return result;
}
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
char *strplus(char *first,char *second){
	int length=strlen(first)+strlen(second)+1;
	char *result=(char*)malloc(length);
	memset(result,'\0',length);
	strcat(result,first);
	strcat(result,second);
	return result;
}
/* pow */
int pow_num(int num1,int num2){
	int result=1;
	int i=0;
	for(i=0;i<num2;i++)
		result*=num1;
	return result;
}
/* Function getHex compute number to Hex */
char *getHex(ExprType t,void*num){
	char *result;
	int i;
	unsigned char *start=(unsigned char*)num;
	switch(t){
		case Int:
			result=(char*)malloc(sizeof(char)*11);
			memset(result,'\0',11);
			sprintf(result,"0x");
			for(i=3;i>=0;i--){
				sprintf(result,"%s%.2x",result,start[i]);
			}
			break;
		case Real:
			result=(char*)malloc(sizeof(char)*11);
			memset(result,'\0',19);
			sprintf(result,"0x");
			for(i=3;i>=0;i--){
				sprintf(result,"%s%.2x",result,start[i]);
			}
		 	break;
		case Char:
		case Bool:
			result=(char*)malloc(sizeof(char)*5);
			memset(result,'\0',5);
			sprintf(result,"0x");
			sprintf(result,"%s%.2x",result,start[i]);
		 	break;
		 default:
		 	break;
	}
	return result;
}
/* translate hex to number */
int hexToNum(char *str){
	int length=strlen(str);
	int result=0;
	int i;
	int position=0;
	for(i=2;i<length;i++){
		if(str[i]!='0'){
			position=i;
			break;
		}
	}
	if(position==0){
		return 0;
	}else{
		for(i=length-1;i>=position;i--){
			if(isdigit((int)str[i])){
				result+=((int)str[i]-48)*((int)pow_num(16,length-i-1));
			}else if(isalpha((int)str[i])){
				result+=((int)str[i]-97+10)*((int)pow_num(16,length-i-1));
			}
		}
	}
	return result;
}
static char buffer[BUFFER_SIZE];
/* translate number to string */
char *numToStr(int num){
	memset(buffer,'\0',BUFFER_SIZE);
	sprintf(buffer,"%d",num);
	char *result=(char *)malloc(strlen(buffer)+1);
	strcpy(result,buffer);
	return result;
}
/* search some header file*/
int searchHeader(char *name){
	return access(name,F_OK);
}
/* eight kinds of address way */
char *getStyle(PAddress address){
	int size;
	char *result;
	char *p=address->position;
	RegisterType r1=address->r1;
	RegisterType r2=address->r2;
	char *i=address->times;
	AddressType aType=address->aType;
	switch(aType){
		case NUM_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%s",p);
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case REG_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%s",lookupRegister(r1));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case DIRC_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%s",p);
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case INDIRC_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"(%s)",lookupRegister(r1));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case EXCUR_A:
			memset(buffer,'\0',BUFFER_SIZE);
			size=sprintf(buffer,"%s(%s)",p,lookupRegister(r1));
			result=(char*)malloc(sizeof(char)*size+1);
			strcpy(result,buffer);
			break;
		case CHANGE_A:
			memset(buffer,'\0',BUFFER_SIZE);
			if(p!=NULL){
				size=sprintf(buffer,"%s(%s,%s)",p,lookupRegister(r1),lookupRegister(r2));
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}else{
				size=sprintf(buffer,"(%s,%s)",lookupRegister(r1),lookupRegister(r2));
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}
			break;
		case RATIO_A:
			memset(buffer,'\0',BUFFER_SIZE);
			if(p!=NULL){
				size=sprintf(buffer,"%s(%s,%s,%s)",p,lookupRegister(r1),lookupRegister(r2),i);
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}else{
				size=sprintf(buffer,"(%s,%s,%s)",lookupRegister(r1),lookupRegister(r2),i);
				result=(char*)malloc(sizeof(char)*size+1);
				strcpy(result,buffer);
			}
			break;
	}
	return result;
}
/* print file information */
void printFile(PInstruction instructions,FILE*file){
	if(instructions!=NULL){
		if(IsPrint){
			fprintf(file,"%3d:  %5s  ",lineno,lookupToken(instructions->op));
			switch(instructions->op){
				case DOTFILE:
				case DOTINCLUDE:
				case DOTGLOBAL:
				case DOTALIGN:
				case DOTSIZE:
				case DOTINT:
				case DOTREAL:
				case DOTCHAR:
				case DOTBOOL:
				case DOTDCLR:
					fprintf(file,"%s\n",instructions->a1->position);
					break;
				case DOTTYPE:
					fprintf(file,"%s,%s\n",instructions->a1->position,instructions->a2->position);
					break;
				case MOV:
				case MOVL:
				case MOVB:
				case MOVW:
				case MOVLABS:
				case LEAL:
				case ADDL:
				case ADDLABS:
				case ADDB:
				case ADDW:
				case SUBL:
				case SUBLABS:
				case SUBB:
				case SUBW:
				case MULL:
				case MULLABS:
				case MULB:
				case MULW:
				case DIVL:
				case DIVLABS:
				case DIVB:
				case DIVW:
				case CMPL:
				case JE:
				case JNE:
				case JG:
				case JL:
				case JGE:
				case JLE:
					fprintf(file,"%s,%s\n",getStyle(instructions->a1),getStyle(instructions->a2));
					break;
				case RET:
				case LEAVE:
					fprintf(file,"\n");
					break;
				case PUSH:
				case PUSHL:
				case PUSHLABS:
				case PUSHB:
				case PUSHW:
				case POP:
				case POPL:
				case POPLABS:
				case POPB:
				case POPW:
				case IN:
				case INL:
				case INLABS:
				case INB:
				case INW:
				case INC:
				case NEG:
				case DEC:
				case NOT:
				case OUT:
				case OUTL:
				case OUTLABS:
				case OUTB:
				case OUTW:
				case OUTP:
				case CALL:
					fprintf(file,"%s\n",getStyle(instructions->a1));
					break;
			}
		}else{
			PBufferList p=createList();
			char temp[BUFFER_SIZE];
			memset(temp,'\0',BUFFER_SIZE);
			memset(buffer,'\0',BUFFER_SIZE);
			int size=0;
			size=sprintf(temp,"%3d:  %5s  ",lineno,lookupToken(instructions->op));
			switch(instructions->op){
				case DOTFILE:
				case DOTINCLUDE:
				case DOTGLOBAL:
				case DOTALIGN:
				case DOTSIZE:
				case DOTINT:
				case DOTREAL:
				case DOTCHAR:
				case DOTBOOL:
				case DOTDCLR:
					size=sprintf(buffer,"%s%s\n",temp,instructions->a1->position);
					break;
				case DOTTYPE:
					size=sprintf(buffer,"%s%s,%s\n",temp,instructions->a1->position,instructions->a2->position);
					break;
				case MOV:
				case MOVL:
				case MOVB:
				case MOVW:
				case MOVLABS:
				case LEAL:
				case ADDL:
				case ADDLABS:
				case ADDB:
				case ADDW:
				case SUBL:
				case SUBLABS:
				case SUBB:
				case SUBW:
				case MULL:
				case MULLABS:
				case MULB:
				case MULW:
				case DIVL:
				case DIVLABS:
				case DIVB:
				case DIVW:
				case CMPL:
				case JE:
				case JNE:
				case JG:
				case JL:
				case JGE:
				case JLE:
					size=sprintf(buffer,"%s%s,%s\n",temp,getStyle(instructions->a1),getStyle(instructions->a2));
					break;
				case RET:
				case LEAVE:
					size=sprintf(buffer,"%s\n",temp);
					break;
				case PUSH:
				case PUSHL:
				case PUSHLABS:
				case PUSHB:
				case PUSHW:
				case POP:
				case POPL:
				case POPLABS:
				case POPB:
				case POPW:
				case IN:
				case INL:
				case INLABS:
				case INB:
				case INW:
				case INC:
				case NEG:
				case DEC:
				case NOT:
				case OUT:
				case OUTL:
				case OUTLABS:
				case OUTB:
				case OUTW:
				case OUTP:
				case CALL:
					size=sprintf(buffer,"%s%s\n",temp,getStyle(instructions->a1));
					break;
			}
			p->string=(char*)malloc(sizeof(char)*(size+1));
			strcpy(p->string,buffer);
			insertList(p);
		}	
		lineno++;
	}
}
