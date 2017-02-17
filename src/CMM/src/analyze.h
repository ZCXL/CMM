/*************************************************/
/* File: analyze.h                               */
/* Semantic analyzer interface for CMM compiler  */
/* Compiler Construction:Principles and Practice */
/* Author: zhuchao 2015-10-11                    */
/*************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* Function buildSymtab constructed the symbol
 * table by preorder traversal of the sybtax tree
 */
void buildSymTab(TreeNode *);

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode *);

#endif