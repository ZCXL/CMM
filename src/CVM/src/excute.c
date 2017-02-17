#include "globals.h"
#include "excute.h"
#include "util.h"
/* memory space */
static unsigned char memory[MAXMEMORY];
/* registers*/
static unsigned char registers[REGISTERS][4];
/* global instructions */
static PInstruction globalInstructions;
/* integer value */
static int intVal;
/* real value */
static float realVal;
/* character value */
static char charVal;
/* boolean value */
static char boolVal;
static int intTmp;
static float realTmp;
static char charTmp;
static char boolTmp;
/* get index of some register */
static int getRegisterNum(RegisterType r){
	switch(r){
		case EAX:
		case AX:
		case AL:
		case AH:
			return 0;
			break;
		case ECX:
		case CX:
		case CL:
		case CH:
			return 2;
			break;
		case EDX:
		case DX:
		case DL:
		case DH:
			return 3;
			break;
		case EBX:
		case BX:
		case BL:
		case BH:
			return 1;
			break;
		case EIP:
		case IP:
			return 4;
			break;
		case EDI:
		case DI:
			return 5;
			break;
		case ESP:
		case SP:
			return 6;
			break;
		case EBP:
		case BP:
			return 7;
			break;
	}
}
/* get register value */
static char* getRegisterVal(RegisterType r){
	return registers[getRegisterNum(r)];
}
/* save value into register */
static void saveRegisterVal(ExprType eType,RegisterType r,void* val){
	int i;
	int reg=getRegisterNum(r);
	switch(eType){
		case Real:
		case Int:
			for(i=0;i<4;i++)
				registers[reg][i]=*((unsigned char*)val+i);
		break;
		case Char:
		case Bool:
			for(i=0;i<1;i++)
				registers[reg][i]=*((unsigned char*)val+i);
			break;
	}
}
/* get value in memory */
static char* getMemoryVal(int position){
	return &memory[position];
}
/* save value into memory */
static void saveMemoryVal(ExprType eType,int position,void *val){
	int i;
	switch(eType){
		case Real:
		case Int:
			for(i=0;i<4;i++)
				memory[position+i]=*((unsigned char*)val+i);
			break;
		case Char:
		case Bool:
			for(i=0;i<1;i++)
				memory[position+i]=*((unsigned char*)val+i);
			break;
	}
}
/* get value from memory or register */
static void* getVal(ExprType aType,PAddress a){
	int position;
	char *pos=NULL;
	switch(a->aType){
		case NUM_A:
			pos=substr(a->position,1,strlen(a->position));
			break;
		case REG_A:
			break;
		case DIRC_A:
			position=hexToNum(a->position);
			break;
		case INDIRC_A:
			/* don't need this way */
			pos=getRegisterVal(a->r1);
			position=*((int*)pos);
			break;
		case EXCUR_A:
			position=atoi(a->position);
			pos=getRegisterVal(a->r1);
			position+=*((int*)pos);
			break;
		case CHANGE_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}
			break;
		case RATIO_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}
			break;
	}
	if(a->aType==NUM_A){
		pos=pos;
		switch(aType){
			case Int:
				intVal=strToInt(pos);
				return &intVal;
				break;
			case Real:
				realVal=strToFloat(pos);
				return &realVal;
				break;
			case Char:
				charVal=strToChar(pos);
				return &charVal;
				break;
			case Bool:
				boolVal=strToChar(pos);
				return &boolVal;
				break;
		}
	}else if(a->aType==REG_A){
		pos=getRegisterVal(a->r1);
	}else{
		pos=getMemoryVal(position);
	}
	switch(aType){
		case Int:
			intVal=*((int*)pos);
			return &intVal;
			break;
		case Real:
			realVal=*((float*)pos);
			return &realVal;
			break;
		case Char:
			charVal=*((char*)pos);
			return &charVal;
			break;
		case Bool:
			boolVal=*((char*)pos);
			return &boolVal;
			break;
	}
}
/* save value into memory or register */
static void saveVal(ExprType eType,PAddress a,void*val){
	int position;
	char *pos=NULL;
	switch(a->aType){
		case NUM_A:
			pos=substr(a->position,1,strlen(a->position));
			break;
		case REG_A:
			break;
		case DIRC_A:
			position=hexToNum(a->position);
			break;
		case INDIRC_A:
			/* don't need this way */
			pos=getRegisterVal(a->r1);
			position=*((int*)pos);
			break;
		case EXCUR_A:
			position=atoi(a->position);
			pos=getRegisterVal(a->r1);
			position+=*((int*)pos);
			break;
		case CHANGE_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}
			break;
		case RATIO_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}
			break;
	}
	if(a->aType==NUM_A){
		
	}else if(a->aType==REG_A){
		saveRegisterVal(eType,a->r1,val);
	}else{
		saveMemoryVal(eType,position,val);
	}
}
static int getAddress(PAddress a){
	int position;
	char *pos=NULL;
	switch(a->aType){
		case NUM_A:
			pos=substr(a->position,1,strlen(a->position));
			break;
		case REG_A:
			break;
		case DIRC_A:
			position=hexToNum(a->position);
			break;
		case INDIRC_A:
			/* don't need this way */
			pos=getRegisterVal(a->r1);
			position=*((int*)pos);
			break;
		case EXCUR_A:
			position=atoi(a->position);
			pos=getRegisterVal(a->r1);
			position+=*((int*)pos);
			break;
		case CHANGE_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}
			break;
		case RATIO_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}
			break;
	}
	if(a->aType==NUM_A){
		
	}else if(a->aType==REG_A){
		position=*((int*)getRegisterVal(a->r1));
		return position;
	}else{
		return position;
	}
	return 0;
}
static void saveAddress(PAddress a,int val){
	int position;
	char *pos=NULL;
	switch(a->aType){
		case NUM_A:
			pos=substr(a->position,1,strlen(a->position));
			break;
		case REG_A:
			break;
		case DIRC_A:
			position=hexToNum(a->position);
			break;
		case INDIRC_A:
			/* don't need this way */
			pos=getRegisterVal(a->r1);
			position=*((int*)pos);
			break;
		case EXCUR_A:
			position=atoi(a->position);
			pos=getRegisterVal(a->r1);
			position+=*((int*)pos);
			break;
		case CHANGE_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos);
			}
			break;
		case RATIO_A:
			if(a->position!=NULL){
				position=atoi(a->position);
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}else{
				position=0;
				pos=getRegisterVal(a->r1);
				position+=*((int*)pos);
				pos=getRegisterVal(a->r2);
				position+=*((int*)pos)*atoi(a->times);
			}
			break;
	}
	if(a->aType==NUM_A){
		
	}else if(a->aType==REG_A){
		saveRegisterVal(Int,a->r1,&val);
	}else{
		*((int*)(&memory[position]))=val;
	}
}
/* excute some instruction */
static void excuteInstruction(PInstruction instructions){
	PAddress a1=instructions->a1;
	PAddress a2=instructions->a2;
	int pos;
	switch(instructions->op){
			case MOV:
			case MOVL:
				saveVal(Int,a2,getVal(Int,a1));
				break;
			case MOVB:
				saveVal(Char,a2,getVal(Char,a1));
				break;
			case MOVW:
				break;
			case MOVLABS:
				saveVal(Real,a2,getVal(Real,a1));
				break;
			case LEAL:
				intTmp=getAddress(a1);
				saveAddress(a2,getAddress(a1));
				break;
			case ADDL:
				intTmp=*((int*)getVal(Int,a1));
				intTmp+=*((int*)getVal(Int,a2));
				saveVal(Int,a2,&intTmp);
				break;
			case ADDLABS:
				realTmp=*((float*)getVal(Real,a1));
				realTmp+=*((float*)getVal(Real,a2));
				saveVal(Real,a2,&realTmp);
				break;
			case ADDB:
				charTmp=*((char*)getVal(Char,a1));
				charTmp+=*((char*)getVal(Char,a2));
				saveVal(Char,a2,&charTmp);
				break;
			case ADDW:
				break;
			case SUBL:
				intTmp=*((int*)getVal(Int,a1));
				intTmp=*((int*)getVal(Int,a2))-intTmp;
				saveVal(Int,a2,&intTmp);
				break;
			case SUBLABS:
				realTmp=*((float*)getVal(Real,a1));
				realTmp=*((float*)getVal(Real,a2))-realTmp;
				saveVal(Real,a2,&realTmp);
				break;
			case SUBB:
				charTmp=*((char*)getVal(Char,a1));
				charTmp=*((char*)getVal(Char,a2))-charTmp;
				saveVal(Char,a2,&charTmp);
				break;
			case SUBW:
				break;
			case MULL:
				intTmp=*((int*)getVal(Int,a1));
				intTmp=*((int*)getVal(Int,a2))*intTmp;
				saveVal(Int,a2,&intTmp);
				break;
			case MULLABS:
				realTmp=*((float*)getVal(Real,a1));
				realTmp=*((float*)getVal(Real,a2))*realTmp;
				saveVal(Real,a2,&realTmp);
				break;
			case MULB:
				charTmp=*((char*)getVal(Char,a1));
				charTmp=*((char*)getVal(Char,a2))*charTmp;
				saveVal(Char,a2,&charTmp);
				break;
			case MULW:
				break;
			case DIVL:
				intTmp=*((int*)getVal(Int,a1));
				intTmp=*((int*)getVal(Int,a2))/intTmp;
				saveVal(Int,a2,&intTmp);
				break;
			case DIVLABS:
				realTmp=*((float*)getVal(Real,a1));
				realTmp=*((float*)getVal(Real,a2))/realTmp;
				saveVal(Real,a2,&realTmp);
				break;
			case DIVB:
				charTmp=*((char*)getVal(Char,a1));
				charTmp=*((char*)getVal(Char,a2))/charTmp;
				saveVal(Char,a2,&charTmp);
				break;
			case DIVW:
				break;
			case CMPL:
				intTmp=*((int*)getVal(Int,a1));
				intTmp=*((int*)getVal(Int,a2))-intTmp;
				saveVal(Int,a2,&intTmp);
				break;
			case JE:
				intTmp=*((int*)getVal(Int,a1));
				if(intTmp==0){
					int pos=atoi(a2->position);
					intTmp=*((int*)getRegisterVal(EIP));
					intTmp+=pos;
					saveRegisterVal(Int,EIP,&intTmp);
				}
				break;
			case JNE:
				intTmp=*((int*)getVal(Int,a1));
				if(intTmp!=0){
					int pos=atoi(a2->position);
					intTmp=*((int*)getRegisterVal(EIP));
					intTmp+=pos;
					saveRegisterVal(Int,EIP,&intTmp);
				}
				break;
			case JG:
				intTmp=*((int*)getVal(Int,a1));
				if(intTmp>0){
					int pos=atoi(a2->position);
					intTmp=*((int*)getRegisterVal(EIP));
					intTmp+=pos;
					saveRegisterVal(Int,EIP,&intTmp);
				}
				break;
			case JL:
				intTmp=*((int*)getVal(Int,a1));
				if(intTmp<0){
					int pos=atoi(a2->position);
					intTmp=*((int*)getRegisterVal(EIP));
					intTmp+=pos;
					saveRegisterVal(Int,EIP,&intTmp);
				}
				break;
			case JGE:
				intTmp=*((int*)getVal(Int,a1));
				if(intTmp>=0){
					int pos=atoi(a2->position);
					intTmp=*((int*)getRegisterVal(EIP));
					intTmp+=pos;
					saveRegisterVal(Int,EIP,&intTmp);
				}
				break;
			case JLE:
				intTmp=*((int*)getVal(Int,a1));
				if(intTmp<=0){
					int pos=atoi(a2->position);
					intTmp=*((int*)getRegisterVal(EIP));
					intTmp+=pos;
					saveRegisterVal(Int,EIP,&intTmp);
				}
				break;
			case RET:
				intTmp=*((int*)getRegisterVal(ESP));
				saveRegisterVal(Int,EIP,getMemoryVal(intTmp));
				intTmp+=4;
				saveRegisterVal(Int,ESP,&intTmp);
				break;
			case LEAVE:
				/* movl %ebp,%esp; */
				intTmp=*((int*)getRegisterVal(EBP));
				saveRegisterVal(Int,ESP,&intTmp);
				/* popl %ebp; */
				saveRegisterVal(Int,EBP,getMemoryVal(intTmp));
				intTmp+=4;
				saveRegisterVal(Int,ESP,&intTmp);
				break;
			case PUSH:
			case PUSHL:
				intTmp=*((int*)getRegisterVal(ESP))-4;
				saveRegisterVal(Int,ESP,&intTmp);
				saveMemoryVal(Int,intTmp,getVal(Int,a1));
				break;
			case PUSHLABS:
				intTmp=*((int*)getRegisterVal(ESP))-4;
				saveRegisterVal(Int,ESP,&intTmp);
				saveMemoryVal(Real,intTmp,getVal(Real,a1));
				break;
			case PUSHB:
				intTmp=*((int*)getRegisterVal(ESP))-4;
				saveRegisterVal(Int,ESP,&intTmp);
				saveMemoryVal(Char,intTmp,getVal(Char,a1));
				break;
			case PUSHW:
				break;
			case POP:
			case POPL:
				intTmp=*((int*)getRegisterVal(ESP));
				saveVal(Int,a1,getMemoryVal(intTmp));
				intTmp+=4;
				saveRegisterVal(Int,ESP,&intTmp);
				break;
			case POPLABS:
				intTmp=*((int*)getRegisterVal(ESP));
				saveVal(Real,a1,getMemoryVal(intTmp));
				intTmp+=4;
				saveRegisterVal(Int,ESP,&intTmp);
				break;
			case POPB:
				intTmp=*((int*)getRegisterVal(ESP));
				saveVal(Char,a1,getMemoryVal(intTmp));
				intTmp+=4;
				saveRegisterVal(Int,ESP,&intTmp);
				break;
			case POPW:
				break;
			case IN:
			case INL:
				scanf("%d",&intTmp);
				saveVal(Int,a1,&intTmp);
				break;
			case INLABS:
				scanf("%f",&realTmp);
				saveVal(Real,a1,&realTmp);
				break;
			case INB:
				scanf("%s",&charTmp);
				saveVal(Char,a1,&charTmp);
				break;
			case INW:
				break;
			case OUT:
			case OUTL:
				intTmp=*((int*)getVal(Int,a1));
				printf("%d",intTmp);
				break;
			case OUTLABS:
				realTmp=*((float*)getVal(Real,a1));
				printf("%lf",realTmp);
				break;
			case OUTB:
				charTmp=*((char*)getVal(Char,a1));
				printf("%c",charTmp);
			case OUTW:
				break;
			case OUTP:
				intTmp=getAddress(a1);
				printf("%s",getMemoryVal(intTmp));
				break;
			case INC:
				intTmp=*((int*)getVal(Int,a1));
				intTmp++;
				saveVal(Int,a1,&intTmp);
				break;
			case NEG:
				intTmp=*((int*)getVal(Int,a1));
				intTmp=-intTmp;
				saveVal(Int,a1,&intTmp);
				break;
			case DEC:
				intTmp=*((int*)getVal(Int,a1));
				intTmp--;
				saveVal(Int,a1,&intTmp);
				break;
			case NOT:
				intTmp=*((int*)getVal(Int,a1));
				intTmp=(intTmp==0)?1:0;
				saveVal(Int,a1,&intTmp);
				break;
			case CALL:
				intTmp=*((int*)getRegisterVal(ESP))-4;
				/* pushl %eip */
				pos=*((int*)getRegisterVal(EIP))+1;
				saveRegisterVal(Int,ESP,&intTmp);
				saveMemoryVal(Int,intTmp,&pos);
				intTmp=getAddress(a1);
				saveRegisterVal(Int,EIP,&intTmp);
				break;
		}
}
/* jump to some instruction */
static PInstruction jmpTo(int position){
	PInstruction tmp=globalInstructions;
	while(tmp!=NULL){
		if(tmp->position==position)
			return tmp;
		tmp=tmp->next;
	}
	return NULL;
}
void excute(PInstruction instructions){
	int i=0,pc=0,cpc=0;
	globalInstructions=instructions;
	memset(memory,'\0',MAXMEMORY);
	for(i=0;i<8;i++)
		memset(registers[i],'\0',4);
	saveRegisterVal(Int,EIP,&pc);
	while(instructions!=NULL){
		excuteInstruction(instructions);
		pc++;
		cpc=*((int*)getRegisterVal(EIP));
		if(cpc==pc-1){
			saveRegisterVal(Int,EIP,&pc);
			instructions=instructions->next;
		}else{
			pc=cpc;
			instructions=jmpTo(cpc);
		}
	}
}