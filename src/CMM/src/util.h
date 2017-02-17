/**************************************************/
/* File:util.h                                    */
/* Utility functions for the CMM compiler         */
/* Compiler Construction: Principles and Practise */
/* Author:zhuchao 2015-10-8                       */
/**************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_
#include "globals.h"

/* Procedure printToken prints a token 
 * and its lexme to the listing file
 */
 void printToken(TokenType,const char *,int);
/* Function newSingleEpxrNode creates a new statement node 
 * for syntax tree construction 
 */
 TreeNode *newSingleEpxrNode(SingleExprKind);
/* Function newMultiExprNode creates a new statement node 
 * for syntax tree construction 
 */
 TreeNode *newMultiExprNode(MultiExprKind);
/* Function newFunctionDeclrNode creates a new statement 
 * for syntax tree construction
 */
 TreeNode *newFunctionDeclrNode(FunctionDeclrKind);
/* Function newFunctionDeclrNode creates a new statement 
 * for syntax tree construction
 */
 TreeNode *newExprNode(ExprKind);
/* Function copyString allocates and makes a new 
 * copy of an existing copyString
 */
 char *copyString(char *);
 /* change file name */
 char *cmmToCmc(char *filename);
 /* Function getHex compute number to Hex */
 char *getHex(ExprType t,void*num);
 /* change meaning */
 char *changeMean(char *src);
/* Procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
 void printTree(TreeNode *);
 /* searche some header file */
 int searchHeader(char *name);
 /* combine two strings */
 char *combineString(char *arg1,char *arg2);
 /* string cut */
 char* substr(const char *source,unsigned int start,unsigned int end);
 /* get project's name */
 char *getProjectName(char *name);
#endif