/**************************************************/
/*File:scan.c                                     */
/*The scanner implementation for the CMM compiler */
/*Compiler Construction:Principles and Practice   */
/*Author:zhuchao 2015-9-30                        */
/**************************************************/
#include "scan.h"
#include "globals.h"
#include "util.h"
typedef enum{
	/*states in scanner DFA*/
	START,INMULTICOMMENT,INMULTICOMMENTEND,INSINGLECOMMENT,INNUM,INREAL,INID,
	INEQ,INNOEQ,INSLASH,INPLUS,INMINUS,INMULTIPLY,INDIVIDE,
	INCHAR,INSTRING,DONE
}StateType;

/*lexme of identifier or reserved work*/
char tokenString[MAXTOKENLEN+1];

/*the maximum of the input buffer for source code lines*/
#define BUFLEN 256

static char lineBuf[BUFLEN];/*hold the current line*/
static int linepos=0;/*the position of character that the scanner is scanning now.*/
static int bufsize=0;/*current size of buffer string*/
static int EOF_Flag=FALSE;

/*get next char from file stream*/
static int getNextChar(void){
	if(!(linepos<bufsize)){
		lineno++;
		if(fgets(lineBuf,BUFLEN-1,source)){
			if(EchoSource)fprintf(listing,"%4d:%s\n",lineno,lineBuf);
			bufsize=strlen(lineBuf);
			linepos=0;
			return lineBuf[linepos++];
		}else{
			EOF_Flag=TRUE;
			return EOF;
		}
	}else{
		return lineBuf[linepos++];
	}
}

/*ungetNextChar backtracks one character in lineBuf*/
static void ungetNextChar(void){
	if(!EOF_Flag)
		linepos--;
}

/*reserved word*/
static struct {
	char*str;
	TokenType tok;
}reservedWords[MAXRESERVED]
	={
		{"if",IF},{"else",ELSE},{"while",WHILE},{"read",READ},{"write",WRITE},{"return",RETURN},
		{"int",INT},{"void",VOID},{"real",REAL},{"char",CHAR},{"string",STRING},{"bool",BOOLTYPE},
		{"false",BOOLVALUE},{"true",BOOLVALUE},{"#include",INCLUDE}
	};
/*look up reserved words,and return token*/
static TokenType reservedLookup(char *s){
	int i;
	for(i=0;i<MAXRESERVED;i++)
		if(!strcmp(s,reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

/***************************************/
/* the primary function of the scanner */
/***************************************/
/* function getToken returns next token 
 * in source file
 */
 TokenType getToken(void){
	 /* index for storing into tokenString*/
	 int tokenStringIndex=0;
	 /* holds current token to be returned*/
	 TokenType currentToken;
	 /* current state - always begins at start */
	 StateType state=START;
	 /* flag to indicate save to tokenString */
	 int save;
	 while(state!=DONE){
		 int c=getNextChar();
		 save=TRUE;
		 switch(state){
			 case START:
			 	if(isdigit(c)){
					 state=INNUM;
				 }else if(isalpha(c)||c=='_'||c=='#'){
					 state=INID;
				 }else if(c=='='){
					 state=INEQ;
				 }else if(c=='!'){
					 state=INNOEQ;
				 }else if(c==' '||c=='\t'||c=='\n'){
					 save=FALSE;
				 }else if(c=='/'){
					 state=INSLASH;
				 }else if(c=='+'){
					 state=INPLUS;
				 }else if(c=='-'){
					 state=INMINUS;
				 }else if(c=='*'){
					 state=INMULTIPLY;
				 }else if(c=='\''){
					 state=INCHAR;
					 save=FALSE;
				 }else if(c=='\"'){
					 state=INSTRING;
					 save=FALSE;
				 }else{
					 state=DONE;
					 switch(c){
						case EOF:
						 	save=FALSE;
							currentToken=ENDFILE;
							break;
						case '<':
							currentToken=LESSTHAN;
							break;
						case '>':
							currentToken=MORETHAN;
							break;
						case '(':
							currentToken=LPAREN;
							break;
						case ')':
							currentToken=RPAREN;
							break;
						case '{':
							currentToken=LBRACE;
							break;
						case '}':
							currentToken=RBRACE;
							break;
						case '[':
							currentToken=LBRACKET;
							break;
						case ']':
							currentToken=RBRACKET;
							break;
						case ';':
							currentToken=SEMI;
							break;
						case ',':
							currentToken=COMMA;
							break;
						case '\t':
						case '\n':
						case '\r':
						case ' ':
							break;
						default:
							currentToken=ERROR;
							break;
					 }
				 }
			 	break;
			case INCHAR:
				if(c=='\''){
					state=DONE;
					save=FALSE;
					if(tokenStringIndex>=2){
						if(tokenString[0]=='\\'){
							if(tokenString[1]=='n'){
								memset(tokenString,'\0',MAXTOKENLEN+1);
								tokenString[0]='\n';
								currentToken=CHARVALUE;
							}else if(tokenString[1]=='t'){
								memset(tokenString,'\0',MAXTOKENLEN+1);
								tokenString[0]='\t';
								currentToken=CHARVALUE;
							}else if(tokenString[1]=='v'){
								memset(tokenString,'\0',MAXTOKENLEN+1);
								tokenString[0]='\v';
								currentToken=CHARVALUE;
							}else{
								currentToken=ERROR;
							}
						}else{
							currentToken=ERROR;
						}
					}else{
						currentToken=CHARVALUE;
					}
				}
				break;
			case INSTRING:
				if(c=='\"'){
					state=DONE;
					save=FALSE;
					currentToken=STRINGVALUE;
				}
				break;
			case INNUM:
				if(c=='.'){
					/*c is equal to '.',indicating that the number is real number,and switch to INREAL state.*/
					state=INREAL;
				}else if(!isdigit(c)){
					/**/
					if(tokenString[0]=='0'&&tokenStringIndex>1){
						state=DONE;
						currentToken=ERROR;
					}else{
						ungetNextChar();
						save=FALSE;
						state=DONE;
						currentToken=NUMINT;
					}
				}
				break;
			case INREAL:
				if(!isdigit(c)){
					if(tokenString[tokenStringIndex-1]=='.'){
						state=DONE;
						currentToken=ERROR;
					}else{
						ungetNextChar();
						save=FALSE;
						state=DONE;
						currentToken=NUMREAL;
					}
				}
				break;
			case INID:
				if(!isalpha(c)&&!isdigit(c)&&c!='_'){
					/*the id is started with alpha or '_',and match alpha,digit,'_' zero or more times*/
					ungetNextChar();
					save=FALSE;
					state=DONE;
					currentToken=ID;
				}
				break;
			case INEQ:
				if(c!='='){
					/*if c is not equal to '=' the statement is assignment */
					ungetNextChar();
					save=FALSE;
					state=DONE;
					currentToken=ASSIGN;
				}else if(c=='='){
					/*if c is equal to '=',the statement is equal statement*/
					state=DONE;
					currentToken=EQ;
				}
				break;
			case INNOEQ:
				/*not equal*/
				if(c=='='){
					state=DONE;
					currentToken=NOEQ;
				}else{
					ungetNextChar();
					save=FALSE;
					state=DONE;
					currentToken=NOT;
				}
				break;
			case INSLASH:
				/*identify /,/*,/=,//*/
				if(c=='*'){
					save=FALSE;
					state=INMULTICOMMENT;
				}else if(c=='='){
					state=DONE;
					currentToken=DIVIDEEQ;
				}else if(c=='/'){
					save=FALSE;
					state=INSINGLECOMMENT;	
				}else{
					ungetNextChar();
					save=FALSE;
					state=DONE;
					currentToken=DIVIDE;
				}
				break;
			case INSINGLECOMMENT:
				/*single comment*/
				save=FALSE;
				if(c==EOF){
					state=DONE;
					currentToken=ENDFILE;
				}else if(c=='\n'){
					tokenStringIndex=0;
					state=START;
				}
				break;
			case INMULTICOMMENT:
				/*multicomment*/
				save=FALSE;
				if(c==EOF){
					state=DONE;
					currentToken=ENDFILE;
				}else if(c=='*'){
					state=INMULTICOMMENTEND;
				}else {
					break;
				}
				break;
			case INMULTICOMMENTEND:
				save=FALSE;
				if(c==EOF){
					state=DONE;
					currentToken=ENDFILE;
				}else if(c=='*'){
					break;
				}else if(c=='/'){
					state=START;
					tokenStringIndex=0;
				}else{
					state=INMULTICOMMENT;
				}
				break;
			case INPLUS:
				/*identify +,+=,++*/
				if(c=='+'){
					state=DONE;
					currentToken=DBLPLUS;
				}else if(c=='='){
					state=DONE;
					currentToken=PLUSEQ;
				}else{
					ungetNextChar();
					save=FALSE;
					state=DONE;
					currentToken=PLUS;
				}
				break;
			case INMINUS:
				/*identify -,-=,--*/
				if(c=='-'){
					state=DONE;
					currentToken=DBLMINUS;
				}else if(c=='='){
					state=DONE;
					currentToken=MINUSEQ;
				}else if(isdigit(c)){
					state=INNUM;
				}else{
					ungetNextChar();
					save=FALSE;
					state=DONE;
					currentToken=MINUS;
				}
				break;
			case INMULTIPLY:
				/*indentify *,*=*/
				if(c=='='){
					state=DONE;
					currentToken=MULTIEQ;
				}else{
					ungetNextChar();
					state=DONE;
					save=FALSE;
					currentToken=MULTIPLY;
				}
				break;
			case DONE:
				break;
			default:
				fprintf(listing,"Scanner Error At %d: state=%d",lineno,state);
				state=DONE;
				currentToken=ERROR;
				break;
		 }
		 if(save&&tokenStringIndex<=MAXTOKENLEN)
		  	tokenString[tokenStringIndex++]=(char)c;
		 if(state==DONE){
			 tokenString[tokenStringIndex]='\0';
			 if(currentToken==ID)
			 	currentToken=reservedLookup(tokenString);
		 }
	 }
	 if(TraceScan){
		printToken(currentToken,tokenString,lineno);
	 }
	 if(currentToken==STRINGVALUE){
		 char *temp=changeMean(tokenString);
		 memset(tokenString,'\0',MAXTOKENLEN+1);
		 int length=strlen(temp);
		 int i;
		 for(i=0;i<length;i++)
		 	tokenString[i]=temp[i];
		 free(temp);
	 }
	 return currentToken;
 }