/**************************************************/
/* File:util.c                                    */
/* Utility functions for the CMM compiler         */
/* Compiler Construction: Principles and Practise */
/* Author:zhuchao 2015-10-8                       */
/**************************************************/
#include <unistd.h>
#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexme to the listing file
 */
 void printToken(TokenType token,const char*tokenString,int lineno){
	 switch(token){
		 case IF:
		 case ELSE:
		 case WHILE:
		 case READ:
		 case WRITE:
		 case RETURN:
		 case VOID:
		 case INT:
		 case REAL:
		 case CHAR:
		 case STRING:
		 case BOOLTYPE:
		 case INCLUDE:
		 	fprintf(listing,"\treserved word:\t%s\n",tokenString);
		 	break;
		 case ASSIGN:
		 case EQ:
		 case PLUS:
		 case MINUS:
		 case MULTIPLY:
		 case DIVIDE:
		 case LPAREN:
		 case RPAREN:
		 case LBRACE:
		 case RBRACE:
		 case LBRACKET:
		 case RBRACKET:
		 case LESSTHAN:
		 case MORETHAN:
		 case NOEQ:
		 case SEMI:
		 case COMMA:
		 case PLUSEQ:
		 case MINUSEQ:
		 case MULTIEQ:
		 case DIVIDEEQ:
		 case DBLPLUS:
		 case DBLMINUS:
		 case NOT:
		 	fprintf(listing,"\tspecial symbol:\t%s\n",tokenString);
			break;
		 case ENDFILE:
		 	fprintf(listing,"\tEOF\n");
			break;
		 case NUMREAL:
		 case NUMINT:
		 	fprintf(listing,"\tnumber,value=\t%s\n",tokenString);
			break;
		 case CHARVALUE:
		 	fprintf(listing,"\tchar,value=\t\'%s\'\n",tokenString);
			break;
		 case STRINGVALUE:
		 	fprintf(listing,"\tstring,value=\t\"%s\"\n",tokenString);
			break;
		 case ID:
		 	fprintf(listing,"\tID,name=\t%s\n",tokenString);
			break;
		 case BOOLVALUE:
		 	fprintf(listing,"\tbool value:\t%s\n",tokenString);
		 	break;
		 case ERROR:
		 	fprintf(listing,"\tScanner Error At line %d before \'%s\'.\n",lineno,tokenString);
		 	break;
	 }
 }
 
  void printToken(TokenType,const char *,int);
/* Function newSingleEpxrNode creates a new statement node 
 * for syntax tree construction 
 */
 TreeNode *newSingleEpxrNode(SingleExprKind singleExprKind){
	 TreeNode *t = (TreeNode*)malloc(sizeof(TreeNode));
	 int i=0;
	 if(t==NULL)
	 	fprintf(listing,"Out of memory error at line %d\n",lineno);
	 else{
		 for(i=0;i<MAXCHILDREN;i++) t->child[i]=NULL;
		 t->sibling=NULL;
		 t->nodeKind=SingleExpr;
		 t->kind.singleExprKind=singleExprKind;
		 t->lineno=lineno;
		 t->type=NO;
	 }
	 return t;
 }
 /* Function newMultiExprNode creates a new statement node 
 * for syntax tree construction 
 */
 TreeNode *newMultiExprNode(MultiExprKind multiExprKind){
	 TreeNode *t = (TreeNode*)malloc(sizeof(TreeNode));
	 int i=0;
	 if(t==NULL)
	 	fprintf(listing,"Out of memory error at line %d\n",lineno);
	 else{
		 for(i=0;i<MAXCHILDREN;i++) t->child[i]=NULL;
		 t->sibling=NULL;
		 t->nodeKind=MultiExpr;
		 t->kind.multiExprKind=multiExprKind;
		 t->lineno=lineno;
	 }
	 return t;
 }
 /* Function newFunctionDeclrNode creates a new statement 
 * for syntax tree construction
 */
 TreeNode *newFunctionDeclrNode(FunctionDeclrKind functionDeclrKind){
	 TreeNode *t = (TreeNode*)malloc(sizeof(TreeNode));
	 int i=0;
	 if(t==NULL)
	 	fprintf(listing,"Out of memory error at line %d\n",lineno);
	 else{
		 for(i=0;i<MAXCHILDREN;i++) t->child[i]=NULL;
		 t->sibling=NULL;
		 t->nodeKind=FunctionDeclr;
		 t->kind.functionDeclrKind=functionDeclrKind;
		 t->lineno=lineno;
		 t->type=NO;
	 }
	 return t;
 }
 /* Function newFunctionDeclrNode creates a new statement 
 * for syntax tree construction
 */
 TreeNode *newExprNode(ExprKind exprKind){
	 TreeNode * t=(TreeNode*)malloc(sizeof(TreeNode));
	 int i;
	 if(t==NULL)fprintf(listing,"Out of memory error at line %d\n",lineno);
	 else {
		 for(i=0;i<MAXCHILDREN;i++)t->child[i]=NULL;
		 t->sibling=NULL;
		 t->nodeKind=Expr;
		 t->kind.exprKind=exprKind;
		 t->lineno=lineno;
		 t->type=NO;
	 }
	 return t;
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
	if(t==NULL)
		fprintf(listing,"Out of memory error at line%d\n",lineno);
	else
		strcpy(t,s);
	return t;
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
 /* change meaning */
 char *changeMean(char *src){
	 int length=strlen(src)+1;
	 char *result=(char*)malloc(length);
	 memset(result,'\0',length);
	 int i,j;
	 for(i=0,j=0;i<length;i++,j++){
		 if(src[i]=='\\'){
			 if(src[i+1]=='n'){
				 result[j]='\n';
				 i++;
			 }else if(src[i+1]=='t'){
				 result[j]='\t';
				 i++;
			 }else if(src[i+1]=='v'){
				 result[j]='\v';
				 i++;
			 }
		 }else{
			 result[j]=src[i];
		 }
	 }
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
			sprintf(result,"%s%.2x",result,*start);
		 	break;
		 default:
		 	break;
	 }
	 return result;
 }
/* Variable indentno is used by printTree to 
 * store current number of spaces to indentation
 */
static int indentno=0;
/* macros to increase/decrease indentation */
#ifndef INDENT
#define INDENT indentno+=2
#endif
#ifndef UNINDENT
#define UNINDENT indentno-=2
#endif

/* printSpaces indents by printing spaces */
static void printSpaces(void){
	 int i;
	 for(i=0;i<indentno;i++)
	 	fprintf(listing," ");
}
 /* Procedure printTree prints a syntax tree to the 
 *listing file using indentation to indicate subtrees
 */
 void printTree(TreeNode *tree){
	 int i;
	 INDENT;
	 while(tree!=NULL){
		 printSpaces();
		 if(tree->nodeKind==SingleExpr){
			 switch(tree->kind.singleExprKind){
				 case IncludeStat:
				 	fprintf(listing,"Include File:%s\n",tree->attr.name);
				 	break;
				 case AssignStat:
				 	if(tree->params[0]!=NULL){
						fprintf(listing,"Assign to: %s(array)\t",tree->attr.name);	 
					}else{
				 		fprintf(listing,"Assign to: %s\t",tree->attr.name);
					}
					switch(tree->attr.token){
						case ASSIGN:
							fprintf(listing,"%s","=");
							break;
						case DBLMINUS:
							fprintf(listing,"%s","++");
							break;
						case DBLPLUS:
							fprintf(listing,"%s","--");
							break;
						case PLUSEQ:
							fprintf(listing,"%s","+=");
							break;
						case MINUSEQ:
							fprintf(listing,"%s","-=");
							break;
						case MULTIEQ:
							fprintf(listing,"%s","*=");
							break;
						case DIVIDEEQ:
							fprintf(listing,"%s","/=");
							break;
						default:
							fprintf(listing,"Unknown Assign token.\n");
							break;
					}
					fprintf(listing,"\n");
				 	break;
				 case DeclrStat:
				 	if(tree->params[0]!=NULL){
				 		fprintf(listing,"Decleration: %s(array)\t",tree->attr.name);
					}else{
						fprintf(listing,"Decleration: %s\t",tree->attr.name);
					}
					switch(tree->type){
						case Int:
							fprintf(listing,"Type:%s","int");
							break;
						case Void:
							fprintf(listing,"Type:%s","void");
							break;
						case Real:
							fprintf(listing,"Type:%s","real");
							break;
						case Char:
							fprintf(listing,"Type:%s\n","char");
							break;
						case String:
							fprintf(listing,"Type:%s\n","string");
							break;
						case Bool:
							fprintf(listing,"Type:%s\n","bool");
							break;
						default:
							fprintf(listing,"Unknown Expression Type.\n");
							break;
					}
					if(tree->attr.token==ASSIGN){
						fprintf(listing,"\n\t=");
					}
					fprintf(listing,"\n");
				 	break;
				 case Read:
				 	fprintf(listing,"Read:%s\n",tree->attr.name);
				 	break;
				 case Write:
				 	fprintf(listing,"Write\n");
				 	break;
				 case ReturnStat:
				 	fprintf(listing,"Return\n");
				 	break;
				 case FunctionStat:
				 	fprintf(listing,"Function Invoke:%s\n",tree->attr.name);
				 	break;
				 default:
				 	fprintf(listing,"Unknown ExprNode kind\n");
					break;
			 }
		 }
		 else if(tree->nodeKind==MultiExpr){
			 switch(tree->kind.multiExprKind){
				 case WhileExpr:
				 	fprintf(listing,"While\n");
				 	break;
				 case IfExpr:
				 	fprintf(listing,"If\n");
				 	break;
				 default:
				 	fprintf(listing,"Unknown ExprNode kind\n");
				 	break;
			 }
		 }
		 else if(tree->nodeKind==FunctionDeclr){
			 if(tree->kind.functionDeclrKind==FunctionExpr){
				 fprintf(listing,"Function Decleration:%s\t",tree->attr.name);
				 switch(tree->type){
						case Int:
							fprintf(listing,"Type:%s\n","int");
							break;
						case Void:
							fprintf(listing,"Type:%s\n","void");
							break;
						case Real:
							fprintf(listing,"Type:%s\n","real");
							break;
						case Char:
							fprintf(listing,"Type:%s\n","char");
							break;
						case String:
							fprintf(listing,"Type:%s\n","string");
							break;
						case Bool:
							fprintf(listing,"Type:%s\n","bool");
							break;
						default:
							fprintf(listing,"Unknown Expression Type.\n");
							break;
				 }
				 fprintf(listing,"\tPramaters:\n");
			 }else{
				 fprintf(listing,"Unknown ExprNode kind\n");
			 }
		 }
		 else if(tree->nodeKind==Expr){
			 switch(tree->kind.exprKind){
				 case Id:
				 	fprintf(listing,"Id: %s\n",tree->attr.name);
				 	break;
				 case ConstInt:
				 	fprintf(listing,"Const Integer: %d\n",tree->attr.val.integer);
				 	break;
				 case ConstReal:
				 	fprintf(listing,"Const Real: %lf\n",tree->attr.val.real);
					break;
				 case ConstChar:
				 	fprintf(listing,"Char: \'%c\'\n",tree->attr.val.character);
					break;
				 case ConstString:
				 	fprintf(listing,"String: \"%s\"\n",tree->attr.val.string);
					break;
				 case ConstBool:
				 	fprintf(listing,"Bool: %s\n",tree->attr.val.b==0?"false":"true");
				 	break;
				 case Array:
				 	fprintf(listing,"Array:%s\n",tree->attr.name);
				 	break;
				 case PrefixExpr:
				 	fprintf(listing,"Prefix Expression: ");
					switch(tree->attr.token){
						case MINUS:
							fprintf(listing,"%s","-");
							break;
						case DBLMINUS:
							fprintf(listing,"%s","--");
							break;
						case DBLPLUS:
							fprintf(listing,"%s","++");
							break;
						case NOT:
							fprintf(listing,"%s","!");
							break;
						default:
							fprintf(listing,"Unknown Prefix\n");
							break;
					}
					fprintf(listing,"\n");
				 	break;
				 case SuffixExpr:
				 	fprintf(listing,"Sufffix Expression: %s",tree->attr.name);
					switch(tree->attr.token){
						case DBLMINUS:
							fprintf(listing,"%s","--");
							break;
						case DBLPLUS:
							fprintf(listing,"%s","++");
							break;
						default:
							fprintf(listing,"Unknown Prefix\n");
							break;
					}
				 	break;
				 case Operation:
				 	fprintf(listing,"Operation: \n\t");
					switch(tree->attr.token){
						case MINUS:
							fprintf(listing,"%s","-");
							break;
						case PLUS:
							fprintf(listing,"%s","+");
							break;
						case DIVIDE:
							fprintf(listing,"%s","/");
							break;
						case MULTIPLY:
							fprintf(listing,"%s","*");
							break;
						case LESSTHAN:
							fprintf(listing,"%s","<");
							break;
						case MORETHAN:
							fprintf(listing,"%s",">");
							break;
						case EQ:
							fprintf(listing,"%s","==");
							break;
						case NOEQ:
							fprintf(listing,"%s","!=");
							break;
						default:
							fprintf(listing,"Unknown Operation Symbol\n");
							break;
					}
					fprintf(listing,"\n");
				 	break;
				 case Function:
				 	fprintf(listing,"Function Expression: %s\n",tree->attr.name);
				 	break;
				 default:
				 	fprintf(listing,"Unknown ExprNode kind\n");
				 	break;
			 }
		 }else{
			 fprintf(listing,"Unknown ExprNode kind\n");
		 }
		 for(i=0;i<MAXPARAMS&&tree->params[i]!=NULL;i++){
			 INDENT;
			 printTree(tree->params[i]);
			 UNINDENT;
		 }
		 for(i=0;i<MAXCHILDREN&&tree->child[i]!=NULL;i++){
			 printTree(tree->child[i]);
		 }
		 tree=tree->sibling;
	 }
	 UNINDENT;
 }
 /* search some header file*/
 int searchHeader(char *name){
	 return access(name,F_OK);
 }
 /* combine two strings */
 char *combineString(char *arg1,char *arg2){
	 char *result=(char*)malloc(sizeof(arg1)+sizeof(arg2)+1);
	 sprintf(result,"%s%s",arg1,arg2);
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
 /* get project's name */
char *getProjectName(char *name){
	int length=strlen(name);
	int i;
	int position=-1;
	for(i=length-1;i>=0;i--){
		if(name[i]=='/'){
			position=i;
			break;
		}
	}
	char *result=substr(name,position+1,length-1);
	return result;
}