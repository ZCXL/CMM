/***************************************************/
/* File:parse.h                                    */
/* The parser interface for the CMM compiler       */
/* Compiler Construction: Principles and Practice  */
/* Author :zhuchao 2015-10-9                       */
/***************************************************/
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /*hold current token */

/*function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * single_stmt(void);
static TreeNode * multi_stmt(void);
static TreeNode * block(void);
/*function decleration*/
static TreeNode * function_decleration(TreeNode *t);
/* single expression*/
static TreeNode * assign_expr(void);
static TreeNode * declr_expr(void);
static TreeNode * function_expr(TreeNode *t);
static TreeNode * read(void);
static TreeNode * write(void);
static TreeNode * include(void);
static TreeNode * return_expr(void);
/*multi expression */
static TreeNode * while_expr(void);
static TreeNode * if_expr(void);
/*base expression*/
static TreeNode * expr(void);
static TreeNode * simple_expr(void);
static TreeNode * term(void);
static TreeNode * prefix_expr(void);
static TreeNode * factor(void);
static void syntaxError(char *message){
	fprintf(listing,"\n>>>");
	fprintf(listing,"Syntax error at line %d:%s",lineno,message);
	Error=TRUE;
}
static void match(TokenType expected){
	if(token==expected)token=getToken();
	else{
		syntaxError("unexpected token->");
		printToken(token,tokenString,lineno);
	}
}
TreeNode * stmt_sequence(void){
	TreeNode * t=statement();
	TreeNode * p=t;
	while(token!=ENDFILE&&token!=ERROR&&token!=RBRACE){
		TreeNode *q;
		q=statement();
		if(q!=NULL){
			if(t==NULL)t=p=q;
			else{
				p->sibling=q;
				p=q;
			}
		}else{
			break;
		}
	}
	return t;
}
TreeNode * statement(void){
	TreeNode *t=NULL;
	switch(token){
		/*decleration expression*/
		case INT:
		case VOID:
		case REAL:
		case CHAR:
		case STRING:
		case BOOLTYPE:
		/*io expression*/
		case READ:
		case WRITE:
		case RETURN:
		case ID:
		case DBLMINUS:
		case DBLPLUS:
		case INCLUDE:
			t=single_stmt();
			break;
		case WHILE:
		case IF:
			t=multi_stmt();
			break;
		case RBRACE:
			break;
		default:
			syntaxError("unexpected token -> ");
			printToken(token,tokenString,lineno);
			token=getToken();
			break;
	}
	return t;
}
TreeNode * single_stmt(void){
	TreeNode *t=NULL;
	switch(token){
		case INT:
		case VOID:
		case REAL:
		case CHAR:
		case STRING:
		case BOOLTYPE:
			t=declr_expr();
			break;
		case READ:
			t=read();
			break;
		case WRITE:
			t=write();
			break;
		case INCLUDE:
			t=include();
			break;
		case ID:
		/*prefix expression.But I don't know how to deal with it......*/
		case DBLMINUS:
		case DBLPLUS:
			t=assign_expr();
			break;
		case RETURN:
			t=return_expr();
			break;
		default:
			syntaxError("unexpected token -> ");
			printToken(token,tokenString,lineno);
			token=getToken();
			break;
	}
	return t;
}
TreeNode * multi_stmt(void){
	TreeNode * t=NULL;
	if(token==IF)
		t=if_expr();
	else if(token==WHILE)
		t=while_expr();
	else{
		syntaxError("unexpected token -> ");
		printToken(token,tokenString,lineno);
		token=getToken();
	}
	return t;
}
TreeNode * block(void){
	TreeNode * t=NULL;
	if(token==LBRACE){
		match(LBRACE);
		t=stmt_sequence();
		match(RBRACE);
	}else
		t=single_stmt();
	return t;
}
/*assign expression*/
TreeNode * assign_expr(void){
	TreeNode * t=newSingleEpxrNode(AssignStat);
	if(t!=NULL&&token==ID){
		t->attr.name=copyString(tokenString);
		t->type=NO;
	}
	match(ID);
	if(token==LBRACKET){
		/*identify array,for example a[10]*/
		match(LBRACKET);
		t->params[0]=expr();
		match(RBRACKET);
	}else if(token==LPAREN){
		t=function_expr(t);
	}
	if(token==ASSIGN){
		t->attr.token=token;
		match(ASSIGN);
		if(t!=NULL)t->child[0]=expr();
	}else if(token==DBLPLUS||token==DBLMINUS){
		t->attr.token=token;
		match(token);
	}else if(token==PLUSEQ||token==MINUSEQ||token==MULTIEQ||token==DIVIDEEQ){
		t->attr.token=token;
		match(token);
		if(t!=NULL)t->child[0]=expr();
	}
	match(SEMI);
	return t;
}
/*decleration expression*/
TreeNode * declr_expr(void){
	TreeNode * t=newSingleEpxrNode(DeclrStat);
	if(t!=NULL){
		switch(token){
			case INT:
				t->type=Int;
				break;
			case VOID:
				t->type=Void;
				break;
			case REAL:
				t->type=Real;
				break;
			case CHAR:
				t->type=Char;
				break;
			case STRING:
				t->type=String;
				break;
			case BOOLTYPE:
				t->type=Bool;
				break;
		}
	}
	token=getToken();//get next token of ID.
	if(t!=NULL&&token==ID){
		t->attr.name=copyString(tokenString);
	}
	match(ID);
	if(token==ASSIGN){
		match(ASSIGN);
		t->attr.token=ASSIGN;
		if(t!=NULL)t->child[0]=expr();
		match(SEMI);
	}else if(token==LPAREN){
		t=function_decleration(t);
	}else if(token==LBRACKET){
		match(LBRACKET);
		t->params[0]=expr();
		match(RBRACKET);
		if(token==ASSIGN){
			match(ASSIGN);
			if(t!=NULL)t->child[0]=expr();
		}
		match(SEMI);
	}else{
		match(SEMI);
	}
	return t;
}
/*read expression*/
TreeNode * read(void){
	TreeNode * t=newSingleEpxrNode(Read);
	match(READ);
	if(t!=NULL&&token==ID){
		t->attr.name=copyString(tokenString);
		t->type=NO;
	}
	match(ID);
	if(token==LBRACKET){
		/*identify array,for example a[10]*/
		match(LBRACKET);
		t->params[0]=expr();
		match(RBRACKET);
	}
	match(SEMI);
	return t;
}
/*write expression*/
TreeNode * write(void){
	TreeNode * t=newSingleEpxrNode(Write);
	match(WRITE);
	if(t!=NULL)
		t->child[0]=expr();
	match(SEMI);
	return t;
}
TreeNode * include(void){
	TreeNode *t=newSingleEpxrNode(IncludeStat);
	match(INCLUDE);
	if(t!=NULL&&token==STRINGVALUE){
		t->attr.name=copyString(tokenString);
		t->type=ConstString;
	}
	match(STRINGVALUE);
	return t;
}
/*return expression*/
TreeNode * return_expr(void){
	TreeNode * t=newSingleEpxrNode(ReturnStat);
	match(RETURN);
	if(t!=NULL)
		t->child[0]=expr();
	match(SEMI);
	return t;
}
/*function expression*/
TreeNode * function_expr(TreeNode *t){
	TreeNode * p=newSingleEpxrNode(FunctionStat);
	int i=0;
	if(p!=NULL){
		p->attr.name=copyString(t->attr.name);
		p->type=NO;
		free(t);
		match(LPAREN);
		while(token!=RPAREN&&i<MAXPARAMS){
			TreeNode * l=expr();
			p->params[i++]=l;
			if(token==RPAREN)
				break;
			else
				match(COMMA);
		}
		match(RPAREN);
		t=p;
	}
	return t;
}
/*while expression*/
TreeNode * while_expr(void){
	TreeNode * t=newMultiExprNode(WhileExpr);
	match(WHILE);
	if(t!=NULL)t->child[0]=expr();
	t->child[1]=block();
	return t;
}
/*if expression*/
TreeNode * if_expr(void){
	TreeNode *t=newMultiExprNode(IfExpr);
	int i;
	match(IF);
	if(t!=NULL)t->child[0]=expr();
	if(t!=NULL)t->child[1]=block();
	i=1;
	if(token==ELSE){
		match(ELSE);
		while(token==IF&&i<MAXCHILDREN-1){
			match(IF);
			if(t!=NULL){
				i++;
				t->child[i]=expr();
				i++;
				t->child[i]=block();
			}
			if(token!=ELSE){
				return t;
			}
			match(ELSE);
		}
		i++;
		if(t!=NULL)t->child[i]=block();
	}
	return t;
}
/*function decleration
 *shit,it's so difficult to process function.
 */
TreeNode * function_decleration(TreeNode *t){
	TreeNode *p=newFunctionDeclrNode(FunctionExpr);
	int i=0;
	if(p!=NULL){
		p->attr.name=copyString(t->attr.name);
		p->type=t->type;
		free(t);
		match(LPAREN);
		while(token!=RPAREN&&i<MAXPARAMS){
			TreeNode * t=newSingleEpxrNode(DeclrStat);
			if(t!=NULL){
				switch(token){
					case INT:
					t->type=Int;
					break;
				case VOID:
					t->type=Void;
					break;
				case REAL:
					t->type=Real;
					break;
				case CHAR:
					t->type=Char;
					break;
				case STRING:
					t->type=String;
					break;
				case BOOLTYPE:
					t->type=Bool;
					break;
				}
			}
			token=getToken();//get next token of ID.
			if(t!=NULL&&token==ID){
				t->attr.name=copyString(tokenString);
			}
			p->params[i++]=t;
			match(ID);
			if(token==LBRACKET){
				match(LBRACKET);
				t->params[0]=expr();
				match(RBRACKET);
			}
			if(token==RPAREN)
				break;
			else
				match(COMMA);
		}
		match(RPAREN);
		p->child[0]=block();
	}
	return p;
}
/* The function expr get expression 
 * >,<,==,!= (lowest priority)
 */
TreeNode * expr(void){
	TreeNode * t=simple_expr();
	if(token==LESSTHAN||token==MORETHAN||token==EQ||token==NOEQ){
		TreeNode * p=newExprNode(Operation);
		if(p!=NULL){
			p->child[0]=t;
			p->attr.token=token;
			t=p;
		}
		match(token);
		if(t!=NULL){
			t->child[1]=simple_expr();
		}
	}
	return t;
}
/* The function simple_expr get simple expression 
 * priority about +,-
 */
TreeNode * simple_expr(void){
	TreeNode * t=term();
	while(token==PLUS||token==MINUS||token==NUMINT||token==NUMREAL){
		TreeNode * p=newExprNode(Operation);
		if(p!=NULL){
			p->child[0]=t;
			if(token==NUMINT||token==NUMREAL){
				p->attr.token=PLUS;
				t=p;
			}else{
				p->attr.token=token;
				t=p;
				match(token);
			}
			p->child[1]=term();
		}
	}
	return t;
}
/* The function term get term
 * priority about *,/
 */
TreeNode * term(void){
	TreeNode * t=prefix_expr();
	while(token==MULTIPLY||token==DIVIDE){
		TreeNode * p=newExprNode(Operation);
		if(p!=NULL){
			p->child[0]=t;
			p->attr.token=token;
			t=p;
			match(token);
			p->child[1]=prefix_expr();
		}
	}
	return t;
}
/* The function prefix_expr get prefix or suffix expression 
 * priority about ++,,--,!,-
 */
TreeNode * prefix_expr(void){
	TreeNode *t=NULL;
	if(token==MINUS||token==DBLPLUS||token==DBLMINUS){
		TreeNode *p=newExprNode(PrefixExpr);
		p->attr.token=token;
		match(token);
		t=factor();
		p->child[0]=t;
		t=p;
	}else if(token==NOT){
		TreeNode *p=newExprNode(PrefixExpr);
		p->attr.token=token;
		match(token);
		t=expr();
		p->child[0]=t;
		t=p;
	}else{
		t=factor();
		if(token==DBLPLUS||token==DBLMINUS){
			TreeNode *p=newExprNode(SuffixExpr);
			p->child[0]=t;
			p->attr.token=token;
			t=p;
			match(token);
		}
	}
	return t;
}
/* The function prefix_expr get id,const,array,brace expression 
 * priority about id,const,array,brace
 */
TreeNode * factor(void){
	TreeNode *t=NULL;
	TreeNode *p=NULL;
	int i=0;
	switch(token){
		case NUMINT:
			/*idendify integer number*/
			t=newExprNode(ConstInt);
			if(t!=NULL&&token==NUMINT)
				t->attr.val.integer=atoi(tokenString);
			match(NUMINT);
			break;
		case NUMREAL:
			/*idendify real number*/
			t=newExprNode(ConstReal);
			if(t!=NULL&&token==NUMREAL)
				t->attr.val.real=atof(tokenString);
			match(NUMREAL);
			break;
		case CHARVALUE:
			/*idendify character*/
			t=newExprNode(ConstChar);
			if(t!=NULL)
				t->attr.val.character=tokenString[0];
			match(CHARVALUE);
			break;
		case STRINGVALUE:
			/*idendify character*/
			t=newExprNode(ConstString);
			if(t!=NULL)
				t->attr.val.string=copyString(tokenString);
			match(STRINGVALUE);
			break;
		case BOOLVALUE:
			t=newExprNode(ConstBool);
			if(t!=NULL)
				t->attr.val.b=(strcmp("false",tokenString)==0)?0:1;
			match(BOOLVALUE);
			break;
		case ID:
			t=newExprNode(Id);
			if(t!=NULL&&token==ID){
				t->attr.name=copyString(tokenString);
				match(ID);
				if(t!=NULL&&token==LBRACKET){
					/*identify array,for example a[10]*/
					p=newExprNode(Array);
					match(LBRACKET);
					p->attr.name=copyString(t->attr.name);
					free(t);
					t=p;
					t->params[0]=expr();
					match(RBRACKET);
				}else if(t!=NULL&&token==LPAREN){
					p=newExprNode(Function);
					match(LPAREN);
					p->attr.name=copyString(t->attr.name);
					free(t);
					t=p;
					i=0;
					while(token!=RPAREN&&i<MAXPARAMS){
						TreeNode * l=expr();
						t->params[i]=l;
						i++;
						if(token==RPAREN)
							break;
						else
							match(COMMA);
					}
					match(RPAREN);
				}
			}
			break;
		case LPAREN:
			match(LPAREN);
			t=expr();
			match(RPAREN);
			break;
		default:
			syntaxError("unexpected token-> ");
			printToken(token,tokenString,lineno);
			token=getToken();
			break;
	}
	return t;
}
/* Function freeSyntaxTree is used to free syntax
 * tree
 */
int freeSyntaxTree(TreeNode *t){
	int i;
	if(t!=NULL){
		for(i=0;i<MAXCHILDREN&&t->child[i]!=NULL;i++)
			freeSyntaxTree(t->child[i]);
		freeSyntaxTree(t->sibling);
		for(i=0;i<MAXPARAMS&&t->params[i]!=NULL;i++)
			freeSyntaxTree(t->params[i]);
		if(t->type==String&&t->attr.val.string!=NULL){
			free(t->attr.val.string);
			t->attr.val.string=NULL;
		}
		if(t->attr.name!=NULL){
			free(t->attr.name);
			t->attr.name=NULL;
		}
		free(t);
		t=NULL;
	}
	return 0;
 }
/**/
/* the primary function of the parseer */
/***************************************/
/* Function parse returns the newly 
 * constructed syntax TreeNode
 */
TreeNode * parse(void){
	TreeNode *t;
	token=getToken();
	t=stmt_sequence();
	if(token!=ENDFILE){
		syntaxError("Code ends before file\n");
	}
	return t;
}