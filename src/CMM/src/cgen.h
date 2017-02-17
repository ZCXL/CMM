/****************************************************/
/* File: cgen.h                                     */
/* The code generator interface to the CMM compiler */
/* compiler Construction: Principles and Practice   */
/* Author:zhuchao 2015-10-26                        */
/****************************************************/
#ifndef _CGEN_H_
#define _CGEN_H_

/* Procedure codeGen generates code to a code
 * file by traveral of the syntax tree.The 
 * second parameter (code file) is the filename
 * of the code file,and is used to print the 
 * file name as a comment in the code file
 */
void codeGen(TreeNode *syntaxTree,char *codefile);

#endif