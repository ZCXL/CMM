/****************************************************/
/* File: cgen.c                                     */
/* The code generator interface to the CMM compiler */
/* compiler Construction: Principles and Practice   */
/* Author:zhuchao 2015-10-26                        */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"
#include "util.h"
#include "limits.h"

static int length=0;
static char *str1=NULL,*str2=NULL;
static void cGen(TreeNode *tree);
static void genExpr(TreeNode *tree);
static void compileError(TreeNode *t,char *message){
	fprintf(listing,"Compile error at line %d: %s\n",t->lineno,message);
	Error=TRUE;
}
static void freeStr(){
	if(str1!=NULL)
		free(str1);
	if(str2!=NULL)
		free(str2);
	str1=NULL;
	str2=NULL;
}
static void saveValue(TreeNode *tree){
	BucketList loc=NULL;
	TreeNode * p1;
	loc=lookup(tree->attr.name,tree->lineno);
	if(loc->level==1){
		int position=lookupIdentifier(loc->name);
		if(position==-1){
			compileError(tree,"No Decleration");
		}
		if(tree->params[0]!=NULL){
			str1=getStyle(0,EAX,0,0,REG_A);
			str2=getStyle(0,EDX,0,0,REG_A);
			emitDouble("movl",str1,str2);
			freeStr();
			p1=tree->params[0];
			cGen(p1);
			str1=combineString(loc->name,"(%edi)");
			str2=getStyle(0,ECX,0,0,REG_A);
			emitDouble("leal",str1,str2);
			freeStr();
			switch(tree->type){
				case Int:
					str1=getStyle(0,EDX,0,0,REG_A);
					str2=getStyle(INT_MIN,ECX,EAX,4,RATIO_A);
					emitDouble("movl",str1,str2);
					freeStr();
					break;
				case Real:
					str1=getStyle(0,EDX,0,0,REG_A);
					str2=getStyle(INT_MIN,ECX,EAX,4,RATIO_A);
					emitDouble("movlabs",str1,str2);
					freeStr();
					break;
				case Bool:
				case Char:
					str1=getStyle(0,EDX,0,0,REG_A);
					str2=getStyle(INT_MIN,ECX,EAX,1,RATIO_A);
					emitDouble("movb",str1,str2);
					freeStr();
					break;
			}
		}else{
			switch(tree->type){
				case Int:
					str1=getStyle(0,EAX,0,0,REG_A);
					str2=combineString(loc->name,"(%edi)");
					emitDouble("movl",str1,str2);
					freeStr();
					break;
				case Real:
					str1=getStyle(0,EAX,0,0,REG_A);
					str2=combineString(loc->name,"(%edi)");
					emitDouble("movlabs",str1,str2);
					freeStr();
					break;
				case Bool:
				case Char:
					str1=getStyle(0,EAX,0,0,REG_A);
					str2=combineString(loc->name,"(%edi)");
					emitDouble("movb",str1,str2);
					freeStr();
					break;
			}
		}	
	}else{
		if(tree->params[0]!=NULL){
			str1=getStyle(0,EAX,0,0,REG_A);
			str2=getStyle(0,EDX,0,0,REG_A);
			emitDouble("movl",str1,str2);
			freeStr();
			p1=tree->params[0];
			cGen(p1);
			str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
			str2=getStyle(0,ECX,0,0,REG_A);
			emitDouble("leal",str1,str2);
			freeStr();
			switch(tree->type){
				case Int:
					str1=getStyle(0,EDX,0,0,REG_A);
					str2=getStyle(INT_MIN,ECX,EAX,4,RATIO_A);
					emitDouble("movl",str1,str2);
					freeStr();
					break;
				case Real:
					str1=getStyle(0,EDX,0,0,REG_A);
					str2=getStyle(INT_MIN,ECX,EAX,4,RATIO_A);
					emitDouble("movlabs",str1,str2);
					freeStr();
					break;
				case Bool:
				case Char:
					str1=getStyle(0,EDX,0,0,REG_A);
					str2=getStyle(INT_MIN,ECX,EAX,1,RATIO_A);
					emitDouble("movb",str1,str2);
					freeStr();
					break;
			}
		}else{
			switch(tree->type){
				case Int:
					str1=getStyle(0,EAX,0,0,REG_A);
					str2=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
					emitDouble("movl",str1,str2);
					freeStr();
					break;
				case Real:
					str1=getStyle(0,EAX,0,0,REG_A);
					str2=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
					emitDouble("movlabs",str1,str2);
					freeStr();
					break;
				case Bool:
				case Char:
					str1=getStyle(0,EAX,0,0,REG_A);
					str2=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
					emitDouble("movb",str1,str2);
					freeStr();
					break;
			}
		}	
	}
}
static void genStmt(TreeNode *tree){
	BucketList loc=NULL;
	int i,j;
	int savedLoc1=0,savedLoc2=0,currentLoc;
	int functionLoc;
	int position;
	TreeNode * p1,*p2,*p3;
	switch(tree->nodeKind){
		case SingleExpr:
			switch(tree->kind.singleExprKind){
				case AssignStat:
					loc=lookup(tree->attr.name,tree->lineno);
					if(loc->level==1){
						position=lookupIdentifier(loc->name);
						if(position==-1){
							compileError(tree,"No Decleration");
						}
						if(tree->attr.token==DBLMINUS||tree->attr.token==DBLPLUS){
							if(tree->params[0]!=NULL){
								p1=tree->params[0];
								cGen(p1);
								str1=combineString(loc->name,"(%edi)");
								str2=getStyle(0,EDX,0,0,REG_A);
								emitDouble("leal",str1,str2);
								freeStr();
								switch(tree->type){
									case Int:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										break;
									case Real:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										break;
									case Bool:
									case Char:
										str1=getStyle(INT_MIN,EDX,EAX,1,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
									break;
								}
							}else{
								switch(tree->type){
									case Int:
										str1=combineString(loc->name,"(%edi)");
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										break;
									case Real:
										str1=combineString(loc->name,"(%edi)");
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										break;
									case Char:
									case Bool:
										str1=combineString(loc->name,"(%edi)");
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
										break;
								}
							}
							if(tree->attr.token=DBLMINUS){
								str1=getStyle(0,EAX,0,0,REG_A);
								emitSingle("inc",str1);
								freeStr();
							}else{
								str1=getStyle(0,EAX,0,0,REG_A);
								emitSingle("dec",str1);
								freeStr();
							}
							saveValue(tree);
						}else if(tree->attr.token==PLUSEQ||tree->attr.token==MINUSEQ||tree->attr.token==MULTIEQ||tree->attr.token==DIVIDEEQ){
							if(tree->params[0]!=NULL){
								p1=tree->params[0];
								cGen(p1);
								str1=combineString(loc->name,"(%edi)");
								str2=getStyle(0,EDX,0,0,REG_A);
								emitDouble("leal",str1,str2);
								freeStr();
								switch(tree->type){
									case Int:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushl",str1);
										freeStr();
										break;
									case Real:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushlabs",str1);
										freeStr();
										break;
									case Bool:
									case Char:
										str1=getStyle(INT_MIN,EDX,EAX,1,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushb",str1);
										freeStr();
										break;
								}
							}else{
								switch(tree->type){
									case Int:
										str1=combineString(loc->name,"(%edi)");
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushl",str1);
										freeStr();
										break;
									case Real:
										str1=combineString(loc->name,"(%edi)");
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushlabs",str1);
										freeStr();
										break;
									case Char:
									case Bool:
										str1=combineString(loc->name,"(%edi)");
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushb",str1);
										freeStr();
										break;
								}
							}
							cGen(tree->child[0]);
							str1=getStyle(0,EDX,0,0,REG_A);
							emitSingle("popl",str1);
							freeStr();
							switch(tree->attr.token){
								case PLUSEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("addl",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("addlabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("addb",str1,str2);
											freeStr();
											break;
									}
									break;
								case MINUSEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("subl",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("sublabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("subb",str1,str2);
											freeStr();
											break;
									}
									break;
								case MULTIEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("mull",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("mullabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("mulb",str1,str2);
											freeStr();
											break;
									}
									break;
								case DIVIDEEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("divl",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("divlabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("divb",str1,str2);
											freeStr();
											break;
									}
									break;
							}
							saveValue(tree);
						}else{
							cGen(tree->child[0]);
							saveValue(tree);
						}
					}else{
						if(tree->attr.token==DBLMINUS||tree->attr.token==DBLPLUS){
							if(tree->params[0]!=NULL){
								p1=tree->params[0];
								cGen(p1);
								str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
								str2=getStyle(0,EDX,0,0,REG_A);
								emitDouble("leal",str1,str2);
								freeStr();
								switch(tree->type){
									case Int:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										break;
									case Real:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										break;
									case Bool:
									case Char:
										str1=getStyle(INT_MIN,EDX,EAX,1,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
									break;
								}
							}else{
								switch(tree->type){
									case Int:
										str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										break;
									case Real:
										str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										break;
									case Char:
									case Bool:
										str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
										break;
								}
							}
							if(tree->attr.token=DBLMINUS){
								str1=getStyle(0,EAX,0,0,REG_A);
								emitSingle("inc",str1);
								freeStr();
							}else{
								str1=getStyle(0,EAX,0,0,REG_A);
								emitSingle("dec",str1);
								freeStr();
							}
							saveValue(tree);
						}else if(tree->attr.token==PLUSEQ||tree->attr.token==MINUSEQ||tree->attr.token==MULTIEQ||tree->attr.token==DIVIDEEQ){
							if(tree->params[0]!=NULL){
								p1=tree->params[0];
								cGen(p1);
								str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
								str2=getStyle(0,EDX,0,0,REG_A);
								emitDouble("leal",str1,str2);
								freeStr();
								switch(tree->type){
									case Int:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushl",str1);
										freeStr();
										break;
									case Real:
										str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushlabs",str1);
										freeStr();
										break;
									case Bool:
									case Char:
										str1=getStyle(INT_MIN,EDX,EAX,1,RATIO_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushb",str1);
										freeStr();
										break;
								}
							}else{
								switch(tree->type){
									case Int:
										str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movl",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushl",str1);
										freeStr();
										break;
									case Real:
										str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movlabs",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushlabs",str1);
										freeStr();
										break;
									case Char:
									case Bool:
										str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
										str2=getStyle(0,EAX,0,0,REG_A);
										emitDouble("movb",str1,str2);
										freeStr();
										str1=getStyle(0,EAX,0,0,REG_A);
										emitSingle("pushb",str1);
										freeStr();
										break;
								}
							}
							cGen(tree->child[0]);
							str1=getStyle(0,EDX,0,0,REG_A);
							emitSingle("popl",str1);
							freeStr();
							switch(tree->attr.token){
								case PLUSEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("addl",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("addlabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("addb",str1,str2);
											freeStr();
											break;
									}
									break;
								case MINUSEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("subl",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("sublabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("subb",str1,str2);
											freeStr();
											break;
									}
									break;
								case MULTIEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("mull",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("mullabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("mulb",str1,str2);
											freeStr();
											break;
									}
									break;
								case DIVIDEEQ:
									switch(tree->type){
										case Int:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("divl",str1,str2);
											freeStr();
											break;
										case Real:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("divlabs",str1,str2);
											freeStr();
											break;
										case Char:
										case Bool:
											str1=getStyle(0,EDX,0,0,REG_A);
											str2=getStyle(0,EAX,0,0,REG_A);
											emitDouble("divb",str1,str2);
											freeStr();
											break;
									}
									break;
							}
							saveValue(tree);
						}else{
							cGen(tree->child[0]);
							saveValue(tree);
						}
					}
					break;
				case DeclrStat:
					if(tree->child[0]!=NULL){
						cGen(tree->child[0]);
						saveValue(tree);
					}
					break;
				case Read:
					switch(tree->type){
						case Int:
							str1=getStyle(0,EAX,0,0,REG_A);
							emitSingle("inl",str1);
							freeStr();
							break;
						case Real:
							str1=getStyle(0,EAX,0,0,REG_A);
							emitSingle("inlabs",str1);
							freeStr();
							break;
						case Char:
						case Bool:
							str1=getStyle(0,EAX,0,0,REG_A);
							emitSingle("inb",str1);
							freeStr();
							break;
					}
					saveValue(tree);
					break;
				case Write:
					cGen(tree->child[0]);
					switch(tree->child[0]->type){
						case Int:
							str1=getStyle(0,EAX,0,0,REG_A);
							emitSingle("outl",str1);
							freeStr();
							break;
						case Real:
							str1=getStyle(0,EAX,0,0,REG_A);
							emitSingle("outlabs",str1);
							freeStr();
							break;
						case Bool:
						case Char:
							str1=getStyle(0,EAX,0,0,REG_A);
							emitSingle("outb",str1);
							freeStr();
							break;
						case ConstString:
							str1=getStyle(0,ESP,0,0,REG_A);
							emitSingle("outp",str1);
							freeStr();
							str2=getHex(Int,&length);
							str1=combineString("$",str2);
							free(str2);
							str2=getStyle(0,ESP,0,0,REG_A);
							emitDouble("addl",str1,str2);
							freeStr();
							break;
					}
					break;
				case ReturnStat:
					cGen(tree->child[0]);
					emit("leave");
					emit("ret");
					break;
				case FunctionStat:
					loc=lookup(tree->attr.name,tree->lineno);
					if(loc!=NULL){
						for(i=loc->params_count-1;loc!=NULL&&i>=0&&tree->params[i]!=NULL;i--){
							cGen(tree->params[i]);
							switch(loc->params[i].type){
								case Int:
									str1=getStyle(0,EAX,0,0,REG_A);
									emitSingle("pushl",str1);
									freeStr();
									break;
								case Real:
									str1=getStyle(0,EAX,0,0,REG_A);
									emitSingle("pushlabs",str1);
									freeStr();
									break;
								case Char:
								case Bool:
									str1=getStyle(0,EAX,0,0,REG_A);
									emitSingle("pushb",str1);
									freeStr();
									break;
							}
						}
					}else{
						/*No decleration.*/
						for(i=MAXPARAMS-1;i>=0;i--){
							if(tree->params[i]!=NULL){
								cGen(tree->params[i]);
								switch(tree->params[i]->type){
									case Int:
									str1=getStyle(0,EAX,0,0,REG_A);
									emitSingle("pushl",str1);
									freeStr();
									break;
								case Real:
									str1=getStyle(0,EAX,0,0,REG_A);
									emitSingle("pushlabs",str1);
									freeStr();
									break;
								case Char:
								case Bool:
									str1=getStyle(0,EAX,0,0,REG_A);
									emitSingle("pushb",str1);
									freeStr();
									break;
								}
							}
						}
					}
					functionLoc=lookupFunc(tree->attr.name);
					if(functionLoc!=-1)
						emitSingle("call",tree->attr.name);
					else
						emitSingle("call",tree->attr.name);
						//compileError(tree,"Function No Decleration.");
					break;
			}
			break;
		case MultiExpr:
			getNextHashTable(tree->lineno);
			if(tree->kind.multiExprKind==WhileExpr){
				p1=tree->child[0];
				p2=tree->child[1];
				savedLoc1=emitSkip(0);
				cGen(p1);
				savedLoc2=emitSkip(1);
				IsPrint=FALSE;
				cGen(p2);
				currentLoc=emitSkip(0);
				emitBackup(savedLoc2);
				IsPrint=TRUE;
				emitAbs("jne",EAX,currentLoc+2);
				emitRestore();
				printToFile();
				str1=getStyle(savedLoc1,0,0,0,DIRC_A);
				str2=getStyle(0,EIP,0,0,REG_A);
				emitDouble("leal",str1,str2);
				freeStr();
			}else if(tree->kind.multiExprKind==IfExpr){
				int i=0;
				while(tree->child[i]!=NULL&&tree->child[i+1]!=NULL&&i<MAXCHILDREN){
					p1=tree->child[i];
					p2=tree->child[i+1];
					cGen(p1);
					savedLoc1=emitSkip(1);
					IsPrint=FALSE;
					cGen(p2);
					currentLoc=emitSkip(0);
					emitBackup(savedLoc1);
					IsPrint=TRUE;
					emitAbs("jne",EAX,currentLoc+1);
					emitRestore();
					printToFile();
					i+=2;
				}
				if(tree->child[i]!=NULL){
					savedLoc2=emitSkip(1);
					p3=tree->child[i];
					IsPrint=FALSE;
					cGen(p3);
					currentLoc=emitSkip(0);
					emitBackup(savedLoc2);
					IsPrint=TRUE;
					//emitDouble("leal",getStyle(currentLoc,0,0,0,DIRC_A),getStyle(0,EIP,0,0,REG_A));
					emitRestore();
					printToFile();
				}
			}
			getLastHashTable();
			break;
		case FunctionDeclr:
			break;
		case Expr:
			genExpr(tree);
			break;
	}
}
static void genExpr(TreeNode *tree){
	BucketList loc=NULL;
	BucketList temp=NULL;
	int functionLoc,i,j;
	TreeNode *p1,*p2;
	char buffer[256];
	int position;
	char tmp;
	switch(tree->kind.exprKind){
		case Id:
			loc=lookup(tree->attr.name,tree->lineno);
			if(loc->level==1){
				position=lookupIdentifier(loc->name);
				if(position==-1){
					compileError(tree,"No Decleration");
				}
				switch(loc->type){
					case Int:
						str1=combineString(loc->name,"(%edi)");
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movl",str1,str2);
						freeStr();
						break;
					case Real:
						str1=combineString(loc->name,"(%edi)");
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movlabs",str1,str2);
						freeStr();
						break;
					case Char:
					case Bool:
						str1=combineString(loc->name,"(%edi)");
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movb",str1,str2);
						freeStr();
						break;
				}
			}else{
				switch(loc->type){
					case Int:
						str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movl",str1,str2);
						freeStr();
						break;
					case Real:
						str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movlabs",str1,str2);
						freeStr();
						break;
					case Char:
					case Bool:
						str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movb",str1,str2);
						freeStr();
						break;
				}
			}
			break;
		case ConstInt:
			memset(buffer,'\0',256);
			str1=getHex(Int,&(tree->attr.val.integer));
			sprintf(buffer,"$%s",str1);
			str2=getStyle(0,EAX,0,0,REG_A);
			emitDouble("movl",buffer,str2);
			freeStr();
			break;
		case ConstReal:
			memset(buffer,'\0',256);
			str1=getHex(Int,&(tree->attr.val.real));
			sprintf(buffer,"$%s",str1);
			str2=getStyle(0,EAX,0,0,REG_A);
			emitDouble("movlabs",buffer,str2);
			freeStr();
			break;
		case ConstChar:
			memset(buffer,'\0',256);
			str1=getHex(Char,&(tree->attr.val.character));
			sprintf(buffer,"$%s",str1);
			str2=getStyle(0,EAX,0,0,REG_A);
			emitDouble("movb",buffer,str2);
			freeStr();
			break;
		case ConstString:
			length=strlen(tree->attr.val.string)+1;
			str2=getHex(Int,&length);
			str1=combineString("$",str2);
			free(str2);
			str2=getStyle(0,ESP,0,0,REG_A);
			emitDouble("subl",str1,str2);
			freeStr();
			str1=getStyle(0,ESP,0,0,REG_A);
			str2=getStyle(0,ECX,0,0,REG_A);
			emitDouble("movl",str1,str2);
			freeStr();
			for(i=0;i<length-1;i++){
				str2=getHex(Int,&i);
				str1=combineString("$",str2);
				free(str2);
				str2=getStyle(0,EAX,0,0,REG_A);
				emitDouble("movl",str1,str2);
				freeStr();
				str2=getHex(Char,&(tree->attr.val.string[i]));
				str1=combineString("$",str2);
				free(str2);
				str2=getStyle(INT_MIN,ECX,EAX,1,RATIO_A);
				emitDouble("movb",str1,str2);
				freeStr();
			}
			i=length-1;
			tmp='\0';
			str1=combineString("$",getHex(Int,&i));
			str2=getStyle(0,EAX,0,0,REG_A);
			emitDouble("movl",str1,str2);
			freeStr();
			str2=getHex(Char,&tmp);
			str1=combineString("$",str1);
			free(str2);
			str2=getStyle(INT_MIN,ECX,EAX,1,RATIO_A);
			emitDouble("movb",str1,str2);
			freeStr();
			break;
		case ConstBool:
			memset(buffer,'\0',256);
			str1=getHex(Bool,&(tree->attr.val.b));
			sprintf(buffer,"$%s",str1);
			str2=getStyle(0,EAX,0,0,REG_A);
			emitDouble("movb",buffer,str2);
			freeStr();
			break;
		case Array:
			p1=tree->params[0];
			loc=lookup(tree->attr.name,tree->lineno);
			cGen(p1);
			if(loc->level==1){
				position=lookupIdentifier(loc->name);
				if(position==-1){
					compileError(tree,"No Decleration");
				}
				str1=combineString(loc->name,"(%edi)");
				str2=getStyle(0,EDX,0,0,REG_A);
				emitDouble("leal",str1,str2);
				freeStr();
				switch(tree->type){
					case Int:
						str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movl",str1,str2);
						freeStr();
						break;
					case Real:
						str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movlabs",str1,str2);
						freeStr();
						break;
					case Bool:
					case Char:
						str1=getStyle(INT_MIN,EDX,EAX,1,RATIO_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movb",str1,str2);
						freeStr();
						break;
				}
			}else{
				str1=getStyle(-loc->memloc,EBP,0,0,EXCUR_A);
				str2=getStyle(0,EDX,0,0,REG_A);
				emitDouble("leal",str1,str2);
				freeStr();
				switch(tree->type){
					case Int:
						str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movl",str1,str2);
						freeStr();
						break;
					case Real:
						str1=getStyle(INT_MIN,EDX,EAX,4,RATIO_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movlabs",str1,str2);
						freeStr();
						break;
					case Bool:
					case Char:
						str1=getStyle(INT_MIN,EDX,EAX,1,RATIO_A);
						str2=getStyle(0,EAX,0,0,REG_A);
						emitDouble("movb",str1,str2);
						freeStr();
						break;
				}
			}
			
			break;
		case PrefixExpr:
			switch(tree->attr.token){
				case MINUS:
					p1=tree->child[0];
					cGen(p1);
					str1=getStyle(0,EAX,0,0,REG_A);
					emitSingle("neg",str1);
					freeStr();
					break;
				case NOT:
					p1=tree->child[0];
					cGen(p1);
					str1=getStyle(0,EAX,0,0,REG_A);
					emitSingle("not",str1);
					freeStr();
					break;
				case DBLPLUS:
				case DBLMINUS:
					p1=tree->child[0];
					cGen(p1);
					loc=lookup(p1->attr.name,p1->lineno);
					if(tree->attr.token==DBLPLUS){
						str1=getStyle(0,EAX,0,0,REG_A);
						emitSingle("inc",str1);
						freeStr();
					}else{
						str1=getStyle(0,EAX,0,0,REG_A);
						emitSingle("dec",str1);
						freeStr();
					}
					break;
			}
			break;
		case SuffixExpr:
			p1=tree->child[0];
			cGen(p1);
			loc=lookup(p1->attr.name,p1->lineno);
			if(tree->attr.token==DBLPLUS){
				str1=getStyle(0,EAX,0,0,REG_A);
				emitSingle("inc",str1);
				freeStr();
			}else{
				str1=getStyle(0,EAX,0,0,REG_A);
				emitSingle("dec",str1);
				freeStr();
			}
			break;
		case Operation:
			p1=tree->child[0];
			p2=tree->child[1];
			cGen(p2);
			str1=getStyle(0,EAX,0,0,REG_A);
			emitSingle("pushl",str1);
			freeStr();
			cGen(p1);
			str1=getStyle(0,EDX,0,0,REG_A);
			emitSingle("popl",str1);
			freeStr();
			switch(tree->attr.token){
				case PLUS:
					switch(tree->type){
						case Int:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("addl",str1,str2);
							freeStr();
							break;
						case Real:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("addlabs",str1,str2);
							freeStr();
							break;
						case Char:
						case Bool:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("addb",str1,str2);
							freeStr();
							break;
					}
					break;
				case MINUS:
					switch(tree->type){
						case Int:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("subl",str1,str2);
							freeStr();
							break;
						case Real:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("sublabs",str1,str2);
							freeStr();
							break;
						case Char:
						case Bool:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("subb",str1,str2);
							freeStr();
							break;
					}
					break;
				case DIVIDE:
					switch(tree->type){
						case Int:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("divl",str1,str2);
							freeStr();
							break;
						case Real:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("divlabs",str1,str2);
							freeStr();
							break;
						case Char:
						case Bool:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("divb",str1,str2);
							freeStr();
							break;
					}
					break;
				case MULTIPLY:
					switch(tree->type){
						case Int:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("mull",str1,str2);
							freeStr();
							break;
						case Real:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("mullabs",str1,str2);
							freeStr();
							break;
						case Char:
						case Bool:
							str1=getStyle(0,EDX,0,0,REG_A);
							str2=getStyle(0,EAX,0,0,REG_A);
							emitDouble("mulb",str1,str2);
							freeStr();
							break;
					}
					break;
				case EQ:
					emitDouble("cmpl",(str1=getStyle(0,EDX,0,0,REG_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("je",(str1=getStyle(0,EAX,0,0,REG_A)),(str2=getStyle(3,EIP,0,0,EXCUR_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(1,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("leal",(str1=getStyle(2,EIP,0,0,EXCUR_A)),(str2=getStyle(INT_MIN,EIP,0,0,REG_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(0,0,0,0,NUM_A)),(getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					break;
				case LESSTHAN:
					emitDouble("cmpl",(str1=getStyle(0,EDX,0,0,REG_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("jl",(str1=getStyle(0,EAX,0,0,REG_A)),(str2=getStyle(3,EIP,0,0,EXCUR_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(1,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("leal",(str1=getStyle(2,EIP,0,0,EXCUR_A)),(str2=getStyle(INT_MIN,EIP,0,0,REG_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(0,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					break;
				case MORETHAN:
					emitDouble("cmpl",(str1=getStyle(0,EDX,0,0,REG_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("jg",(str1=getStyle(0,EAX,0,0,REG_A)),(str2=getStyle(3,EIP,0,0,EXCUR_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(1,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("leal",(str1=getStyle(2,EIP,0,0,EXCUR_A)),(str2=getStyle(INT_MIN,EIP,0,0,REG_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(0,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					break;
				case NOEQ:
					emitDouble("cmpl",(str1=getStyle(0,EDX,0,0,REG_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("jne",(str1=getStyle(0,EAX,0,0,REG_A)),(str2=getStyle(3,EIP,0,0,EXCUR_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(1,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					emitDouble("leal",(str1=getStyle(2,EIP,0,0,EXCUR_A)),(str2=getStyle(INT_MIN,EIP,0,0,REG_A)));
					freeStr();
					emitDouble("movl",(str1=getStyle(0,0,0,0,NUM_A)),(str2=getStyle(0,EAX,0,0,REG_A)));
					freeStr();
					break;
			}
			break;
		case Function:
			loc=lookup(tree->attr.name,tree->lineno);
			if(loc!=NULL){
				for(i=loc->params_count-1;loc!=NULL&&i>=0&&tree->params[i]!=NULL;i--){
					cGen(tree->params[i]);
					switch(loc->params[i].type){
						case Int:
							emitSingle("pushl",(str1=getStyle(0,EAX,0,0,REG_A)));
							freeStr();
							break;
						case Real:
							emitSingle("pushlabs",(str1=getStyle(0,EAX,0,0,REG_A)));
							freeStr();
							break;
						case Bool:
							emitSingle("pushb",(str1=getStyle(0,EAX,0,0,REG_A)));
							freeStr();
							break;
						case Char:
							emitSingle("pushb",(str1=getStyle(0,EAX,0,0,REG_A)));
							freeStr();
							break;
					}
				}
			}else{
				/*No decleration.*/
				for(i=MAXPARAMS-1;i>=0;i--){
					if(tree->params[i]!=NULL){
						cGen(tree->params[i]);
						switch(tree->params[i]->type){
							case Int:
								emitSingle("pushl",(str1=getStyle(0,EAX,0,0,REG_A)));
								freeStr();
								break;
							case Real:
								emitSingle("pushlabs",(str1=getStyle(0,EAX,0,0,REG_A)));
								freeStr();
								break;
							case Bool:
								emitSingle("pushb",(str1=getStyle(0,EAX,0,0,REG_A)));
								freeStr();
								break;
							case Char:
								emitSingle("pushb",(str1=getStyle(0,EAX,0,0,REG_A)));
								freeStr();
								break;
						}
					}
				}
			}
			
			functionLoc=lookupFunc(tree->attr.name);
			if(functionLoc!=-1)
				emitSingle("call",tree->attr.name);
			else
				emitSingle("call",tree->attr.name);
				//compileError(tree,"Function No Decleration.");
			break;
	}
}
static void cGen(TreeNode * tree){
	if(tree!=NULL){
		genStmt(tree);
		cGen(tree->sibling);
	}
}
static void cGenGlobal(TreeNode *tree){
	BucketList loc=NULL;
	int i,j;
	if(tree!=NULL){
		switch(tree->nodeKind){
			case SingleExpr:
				switch(tree->kind.singleExprKind){
					case AssignStat:
						compileError(tree,"Assign statement shouldn't be global.");
						break;
					case DeclrStat:
						loc=lookup(tree->attr.name,tree->lineno);
						if(loc!=NULL){
							insertIdentifier(tree->attr.name,loc->memloc);
							emitSingle(".global",tree->attr.name);
							emitSingle(".align",(str1=getHex(Int,&loc->memloc)));
							freeStr();
							switch(tree->type){
								case Int:
								case Real:
									emitSingle(".size","4");
									break;
								case Bool:
									emitSingle(".size","1");
								case Char:
									emitSingle(".size","1");
									break;
							}
							if(tree->params[0]!=NULL){
								emitDouble(".type",tree->attr.name,"@array");
							}else{
								emitDouble(".type",tree->attr.name,"@object");	
							}
							if(tree->child[0]!=NULL)
								cGenGlobal(tree->child[0]);
						}
						break;
					case IncludeStat:
						emitSingle(".include",(str1=cmmToCmc(tree->attr.name)));
						freeStr();
						break;
					case Read:
						compileError(tree,"Read statement shouldn't be global.");
						break;
					case Write:
						compileError(tree,"Write statement shouldn't be global.");
						break;
					case ReturnStat:
						compileError(tree,"Return statement shouldn't be global.");	
						break;
					case FunctionStat:
						compileError(tree,"Function statement shouldn't be global.");
						break;
				}
				break;
			case MultiExpr:
				switch(tree->kind.multiExprKind){
					case IfExpr:
						compileError(tree,"If statement shouldn't be global.");
						break;
					case WhileExpr:
						compileError(tree,"While statement shouldn't be global.");
						break;
				}
				break;
			case FunctionDeclr:
				loc=lookup(tree->attr.name,tree->lineno);
				getNextHashTable(tree->lineno);
				emitSingle(".global",tree->attr.name);
				emitFunction(".dclr",tree->attr.name);
				emitDouble(".type",tree->attr.name,"@function");
				emitSingle("pushl",(str1=getStyle(0,EBP,0,0,REG_A)));
				freeStr();
				emitDouble("movl",(str1=getStyle(0,ESP,0,0,REG_A)),(str2=getStyle(0,EBP,0,0,REG_A)));
				freeStr();
				emitDouble("subl",(str1=getStyle(128,0,0,0,NUM_A)),(str2=getStyle(0,ESP,0,0,REG_A)));
				freeStr();
				for(i=0,j=0;loc!=NULL&&i<loc->params_count&&tree->params[i]!=NULL;i++){
					switch(loc->params[i].type){
						case Int:
							j-=4;
							emitDouble("movl",(str1=getStyle(-j+4,EBP,0,0,EXCUR_A)),(str2=getStyle(j,EBP,0,0,EXCUR_A)));
							freeStr();
							break;
						case Real:
							j-=4;
							emitDouble("movlabs",(str1=getStyle(-j+4,EBP,0,0,EXCUR_A)),(str2=getStyle(j,EBP,0,0,EXCUR_A)));
							freeStr();
							break;
						case Bool:
						case Char:
							j-=1;
							emitDouble("movb",(str1=getStyle(-j+4,EBP,0,0,EXCUR_A)),(str2=getStyle(j,EBP,0,0,EXCUR_A)));
							freeStr();
							break;
					}
				}
				cGen(tree->child[0]);
				if(loc->type==Void){
					emit("leave");
					emit("ret");
				}
				getLastHashTable();
				break;
			case Expr:
				switch(tree->kind.exprKind){
					case ConstInt:
						emitSingle(".Int",(str1=getHex(Int,&(tree->attr.val.integer))));
						freeStr();
						break;
					case ConstChar:
						emitSingle(".Char",(str1=getHex(Char,&(tree->attr.val.integer))));
						freeStr();
						break;
					case ConstBool:
						emitSingle(".Bool",(str1=getHex(Bool,&(tree->attr.val.integer))));
						freeStr();
						break;
					case ConstReal:
						emitSingle(".Real",(str1=getHex(Real,&(tree->attr.val.integer))));
						freeStr();
						break;
					default:
						compileError(tree,"Expression shouldn't be global");
						break;
				}
				break;
		}
		cGenGlobal(tree->sibling);
	}
}
void codeGen(TreeNode *syntaxTree,char *codefile){
	char *s=malloc(strlen(codefile)+7);
	strcpy(s,"File: ");
	strcpy(s,codefile);
	emitComment("CMM Compilation to CMM code");
	emitComment(s);
	free(s);
	s=NULL;
	s=getProjectName(codefile);
	emitSingle(".file",s);
	free(s);
	s=NULL;
	printf("He\n");
	cGenGlobal(syntaxTree);
	freeIdentifier();
	freeFunc();
}