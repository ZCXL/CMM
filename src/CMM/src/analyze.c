/*************************************************/
/* File: analyze.h                               */
/* Semantic analyzer interface for CMM compiler  */
/* Compiler Construction:Principles and Practice */
/* Author: zhuchao 2015-10-11                    */
/*************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location=0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t 
 */
static void traverse(TreeNode * t,
		void (* preProc)(TreeNode *),
		void (* postProc)(TreeNode *)){
	if(t!=NULL){
		preProc(t);
		if(t->nodeKind==MultiExpr||t->nodeKind==FunctionDeclr){
			getNextHashTable(t->lineno);
			int temp=location;
			location=0;
			int i;
			for(i=0;i<MAXPARAMS;i++)
				traverse(t->params[i],preProc,postProc);
			for(i=0;i<MAXPARAMS&&t->params[i]!=NULL;i++)
				insert_params(t->attr.name,t->params[i]->attr.name,t->lineno,t->params[i]->type);
			for(i=0;i<MAXCHILDREN;i++)
				traverse(t->child[i],preProc,postProc);
			//printSymTab(listing);
			location=temp;
			getLastHashTable();
		}else{
			int i;
			for(i=0;i<MAXPARAMS;i++)
				traverse(t->params[i],preProc,postProc);
			for(i=0;i<MAXCHILDREN&&t->child[i]!=NULL;i++){
				traverse(t->child[i],preProc,postProc);
			}
		}
		postProc(t);
		traverse(t->sibling,preProc,postProc);
	}
}
/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t){
	if(t==NULL)
		return;
	else
		return;
}

/* Procudure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
static void insertNode(TreeNode * t){
	switch(t->nodeKind){
		case SingleExpr:
			switch(t->kind.singleExprKind){
				case DeclrStat:
					if(t->params[0]!=NULL){
						if(st_lookup(t->attr.name)==-1){
							switch(t->type){
								case Int:
									location+=4*t->params[0]->attr.val.integer;
									break;
								case Real:
									location+=4*t->params[0]->attr.val.integer;
									break;
								case Bool:
									location+=1*t->params[0]->attr.val.integer;
									break;
								case Char:
									location+=1*t->params[0]->attr.val.integer;
									break;
								case NO:
									location+=1;//throw out exception
									break;
							}
							st_insert(t->attr.name,t->lineno,location,t->type,ArrayType);
						}else
							st_insert(t->attr.name,t->lineno,0,t->type,ArrayType);
					}else{
						if(st_lookup(t->attr.name)==-1){
							switch(t->type){
								case Int:
									location+=4;
									break;
								case Real:
									location+=4;
									break;
								case Bool:
									location+=1;
									break;
								case Char:
									location+=1;
									break;
								case NO:
									location+=1;
									break;
							}
							st_insert(t->attr.name,t->lineno,location,t->type,Identifier);
						}else
							st_insert(t->attr.name,t->lineno,0,t->type,Identifier);
					}
					break;
				case AssignStat:
				case Read:
					if(t->params[0]!=NULL){
						st_insert(t->attr.name,t->lineno,0,t->type,ArrayType);
					}else{
						st_insert(t->attr.name,t->lineno,0,t->type,Identifier);
					}
					break;
				case FunctionStat:
					st_insert(t->attr.name,t->lineno,0,t->type,Fidentifier);
					break;
				/*There is no name in write or return statement*/
				case Write:
				case ReturnStat:
				case IncludeStat:
					break;
				default:
					break;
			}
			break;
		case MultiExpr:
			switch(t->kind.multiExprKind){
				/*There is no name in while or if statement*/
				case WhileExpr:
					break;
				case IfExpr:
					break;
				default:
					break;
			}
			break;
		case FunctionDeclr:
			switch(t->kind.functionDeclrKind){
				case FunctionExpr:
					st_insert(t->attr.name,t->lineno,0,t->type,Fidentifier);
					break;
				default:
					break;
			}
			break;
		case AnotationExpr:
			break;
		case Expr:
			switch(t->kind.exprKind){
				case Id:
					st_insert(t->attr.name,t->lineno,0,t->type,Identifier);
					break;
				case Array:
					st_insert(t->attr.name,t->lineno,0,t->type,ArrayType);
					break;
				case Function:
					st_insert(t->attr.name,t->lineno,0,t->type,Fidentifier);
					break;
				case ConstInt:
					break;
				case ConstReal:
					break;
				case ConstChar:
					break;
				case ConstString:
					break;
				case ConstBool:
					break;
				case PrefixExpr:
					break;
				case SuffixExpr:
					break;
				case Operation:
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

/* Function buildSymtab constructed the symbol
 * table by preorder traversal of the sybtax tree
 */
void buildSymTab(TreeNode *t){
	createFieldList();
	traverse(t,insertNode,nullProc);
	if(TraceAnalyze){
		fprintf(listing,"\nSymbol table:\n\n");
		printSymTab(listing);
	}
}

static void typeError(TreeNode *t,char *message){
	fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
	Error=TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree nodeKind
 */
static void checkNode(TreeNode *t){
	BucketList l=NULL;
	int i;
	switch(t->nodeKind){
		case Expr:
			switch(t->kind.exprKind){
				case Id:
					l=lookup(t->attr.name,t->lineno);
					if(l!=NULL){
						if(l->type==NO)
							typeError(t,"No Declaration.");
						if(l->sType==Fidentifier)
							typeError(t,"Identifier must be diffirent from Function.");
						if(l->sType==ArrayType&&t->params[0]==NULL)
							typeError(t,"Array must be used with index.");
						if(l->sType==Identifier&&t->params[0]!=NULL)
							typeError(t,"Identifier don't have index.");
						t->type=l->type;
					}else{
						typeError(t,"No Declaration.");
					}
					break;
				case Function:
					l=lookup(t->attr.name,t->lineno);
					if(l!=NULL){
						if(l->type==NO)
							typeError(t,"No Declaration.");
						if(l->sType==Identifier)
							typeError(t,"Function must be diffirent from Identifier.");
						t->type=l->type;
						int i;
						for(i=0;i<l->params_count;i++){
							if(t->params[i]!=NULL){
								if(t->params[i]->type!=l->params[i].type)
									typeError(t,"Pamam's type don't match with each other.");
							}else{
								typeError(t,"Too few params.");
								break;
							}
						}
						if(t->params[i]!=NULL){
							typeError(t,"Too many params");
						}
					}else{
						//typeError(t,"No Declaration.");
						t->type=Int;
						/* function decleration don't need checked */
					}
					break;
				case Array:
					l=lookup(t->attr.name,t->lineno);
					if(l!=NULL){
						if(l->type==NO)
							typeError(t,"No Declaration.");
						if(l->sType==Identifier)
							typeError(t,"Array must be diffirent from Identifier.");
						t->type=l->type;
						if(t->params[0]->type!=Int)
							typeError(t,"Array index must be Integer.");
					}else{
						typeError(t,"No Declaration.");
					}
					break;
				case ConstInt:
					t->type=Int;
					break;
				case ConstReal:
					t->type=Real;
					break;
				case ConstChar:
					t->type=Char;
					break;
				case ConstString:
					t->type=String;
					break;
				case ConstBool:
					t->type=Bool;
					break;
				case PrefixExpr:
					if(t->attr.token==NOT){
						if(t->child[0]->type!=Bool)
							typeError(t,"Not expression must be Boolean.");
						else
							t->type=Bool;
						break;
					}
					if(t->child[0]->nodeKind==Expr&&t->child[0]->kind.exprKind!=Id&&t->child[0]->kind.exprKind!=Array){
						typeError(t,"Prefix expression must be Identifier or Array.");
					}else{
						if(t->child[0]->type!=Int)
							typeError(t,"Prefix expression's type must be Integer.");
					}
					t->type=Int;
					break;
				case SuffixExpr:
					if(t->child[0]->nodeKind!=Expr&&t->child[0]->kind.exprKind!=Id&&t->child[0]->kind.exprKind!=Array){
						typeError(t,"Suffix expression must be Identifier or Array.");
					}else{
						if(t->child[0]->type!=Int)
							typeError(t,"Suffix expression's type must be Integer.");
					}
					t->type=Int;
					break;
				case Operation:
					if(t->child[0]->type==NO||t->child[1]->type==NO)
						typeError(t,"Operation applied to non-declaration.");
					if(t->attr.token==LESSTHAN||t->attr.token==MORETHAN||t->attr.token==EQ||t->attr.token==NOEQ){
						if(t->child[0]->type==String||t->child[1]->type==String){
							typeError(t,"String can't be compared with number.");
						}else
							t->type=Bool;
					}else{
						switch(t->child[0]->type){
							case Real:
								switch(t->child[1]->type){
									case Real:
									case Int:
									case Char:
									case Bool:
										t->type=Real;
										break;
									case String:
										typeError(t,"String can't compute with Real number.");
										break;
								}
								break;
							case Int:
								switch(t->child[1]->type){
									case Real:
										t->type=Real;
										break;
									case Int:
									case Char:
									case Bool:
										t->type=Int;
										break;
									case String:
										typeError(t,"String can't compute with Integer.");
										break;
								}
								break;
							case Char:
								switch(t->child[1]->type){
									case Real:
										t->type=Real;
										break;
									case Int:
									case Char:
									case Bool:
										t->type=Char;
										break;
									case String:
										typeError(t,"String can't compute with Character.");
										break;
								}
								break;
							case String:
								switch(t->child[1]->type){
									case Real:
										typeError(t,"String can't compute with Real number.");
										break;
									case Int:
										typeError(t,"String can't compute with Integer.");
										break;
									case Char:
										typeError(t,"String can't compute with Character.");
										break;
									case Bool:
										typeError(t,"String can't compute with Bool.");
										break;
									case String:
										if(t->attr.token!=PLUS)
											typeError(t,"String can't be computed.");
										else
											t->type=String;
										break;
								}
								break;
							case Bool:
								switch(t->child[1]->type){
									case Real:
										t->type=Real;
										break;
									case Int:
									case Char:
									case Bool:
										t->type=Int;
										break;
									case String:
										typeError(t,"String can't compute with Bool.");
										break;
								}
								break;
						}
					}
					break;
				default:
					break;
			}
			break;
		case SingleExpr:
			switch(t->kind.singleExprKind){
				case DeclrStat:
					if(t->params[0]!=NULL){
						if(t->params[0]->type!=Int)
							typeError(t,"Array's index must be integer.");
						else{
							insert_size(t->attr.name,t->lineno,t->params[0]->attr.val.integer);
						}
					}
					if(t->attr.token==ASSIGN){
						if(t->type!=t->child[0]->type)
							typeError(t,"Type not match.");
					}
					break;
				case AssignStat:
					l=lookup(t->attr.name,t->lineno);
					if(l!=NULL){
						if(l->type==NO){
							typeError(t,"No Declaration.");
						}
						if(l->sType==Fidentifier)
							typeError(t,"Identifier must be diffirent from Function.");
						t->type=l->type;
						if(t->params[0]!=NULL){
							if(t->params[0]->type!=Int)
								typeError(t,"Array's index must be integer.");
							else{
								if(t->params[0]->attr.val.integer>=l->size||t->params[0]->attr.val.integer<0)
									typeError(t,"Out of Array Size.");
							}
						}
						if(t->attr.token==ASSIGN){
							if(t->type!=t->child[0]->type)
								typeError(t,"Type not match.");;
						}else if(t->attr.token==DBLPLUS||t->attr.token==DBLMINUS){
							if(t->type!=Int)
								typeError(t,"Variable must be integer.");
						}else if(t->attr.token==PLUSEQ||t->attr.token==MINUSEQ||t->attr.token==MULTIEQ||t->attr.token==DIVIDEEQ){
							switch(t->type){
							case Real:
								switch(t->child[0]->type){
									case Real:
									case Int:
									case Char:
									case Bool:
										break;
									case String:
										typeError(t,"String can't compute with Real number.");
										break;
								}
								break;
							case Int:
								switch(t->child[0]->type){
									case Real:
									case Int:
									case Char:
									case Bool:
										break;
									case String:
										typeError(t,"String can't compute with Integer.");
										break;
								}
								break;
							case Char:
								switch(t->child[0]->type){
									case Real:
									case Int:
									case Char:
									case Bool:
										break;
									case String:
										typeError(t,"String can't compute with Character.");
										break;
								}
								break;
							case String:
								switch(t->child[0]->type){
									case Real:
										typeError(t,"String can't compute with Real number.");
										break;
									case Int:
										typeError(t,"String can't compute with Integer.");
										break;
									case Char:
										typeError(t,"String can't compute with Character.");
										break;
									case Bool:
										typeError(t,"String can't compute with Bool.");
										break;
									case String:
										if(t->attr.token!=PLUSEQ)
											typeError(t,"String can't be computed.");
										break;
								}
								break;
						}
						//	
						}
					}else{
						typeError(t,"No Declaration.");
					}
					break;
				case Read:
					l=lookup(t->attr.name,t->lineno);
					if(l!=NULL){
						if(l->type==NO){
							typeError(t,"No Declaration.");
						}
						if(l->sType==Fidentifier)
							typeError(t,"Identifier must be diffirent from Function.");
						t->type=l->type;
						if(t->params[0]!=NULL){
							if(t->params[0]->type!=Int)
								typeError(t,"Array's index must be integer.");
						}
					}else{
						typeError(t,"No Declaration.");
					}
					break;
				case FunctionStat:
					l=lookup(t->attr.name,t->lineno);
					if(l!=NULL){
						if(l->type==NO)
							typeError(t,"No Declaration.");
						if(l->sType==Identifier)
							typeError(t,"Function must be diffirent from Identifier.");
						t->type=l->type;
						int i;
						for(i=0;i<l->params_count;i++){
							if(t->params[i]!=NULL){
								if(t->params[i]->type!=l->params[i].type)
									typeError(t,"Pamam's type don't match with each other.");
							}else{
								typeError(t,"Too few params.");
								break;
							}
						}
						if(t->params[i]!=NULL){
							typeError(t,"Too many params");
						}
					}else{
						//typeError(t,"No Declaration.");
						t->type=Int;
					}
					break;
				/*There is no name in write or return statement*/
				case Write:
				case ReturnStat:
					break;
				case IncludeStat:
					//if(searchHeader(t->attr.name)==-1)
					//	typeError(t,"No such file or directory.");
					break;
				default:
					break;
			}
			break;
		case MultiExpr:
			switch(t->kind.multiExprKind){
				/*There is no name in while or if statement*/
				case WhileExpr:
					if(t->child[0]->type!=Bool)
						typeError(t,"while expression's test is not Boolean");
					break;
				case IfExpr:
					i=0;
					while(t->child[i]!=NULL&&i<MAXCHILDREN){
						if(t->child[i]->type!=Bool&&t->child[i+1]!=NULL)
							typeError(t,"if expression's test is not Boolean");
						i+=2;
					}
					break;
				default:
					break;
			}
			break;
		case FunctionDeclr:
			if(t->type!=Void){
				i=0;
				TreeNode *n=t->child[0];
				while(n->sibling!=NULL)n=n->sibling;
				if(n==NULL)
					typeError(t,"block is empty.");
				if(n->nodeKind!=SingleExpr&&n->kind.singleExprKind!=ReturnStat)
					typeError(t,"function must have return value.");
			}
			break;
		default:
			break;
	}
}

/* Procedure typeCheck performs type checking
 * by a postOrder syntax tree ttraversal
 */
void typeCheck(TreeNode * syntaxTree){
	traverse(syntaxTree,nullProc,checkNode);
}