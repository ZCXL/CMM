/***************************************************/
/* File:parse.h                                    */
/* The parser interface for the CMM compiler       */
/* Compiler Construction: Principles and Practice  */
/* Author :zhuchao 2015-10-9                       */
/***************************************************/

#ifndef _PARSE_H_
#define _PARSE_H_

#include "globals.h"
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void);
/* Function freeSyntaxTree is used to free syntax
 * tree
 */
int freeSyntaxTree(TreeNode *t);
#endif