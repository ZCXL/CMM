#ifndef _UTIL_H_
#define _UTIL_H_
/* Function copyString allocates and makes a new 
 * copy of an existing copyString
 */
extern char *copyString(char *s);
/* add two strings together */
extern char *strplus(const char *first,const char *second);
/* check file wheather is cmm file*/
extern int isCMM(char *name);
/* string cut */
extern char* substr(const char *source,unsigned int start,unsigned int end);
/* change file name */
extern char *cmmToCmc(char *filename);
/* searche some header file */
extern int searchHeader(char *name);
/* get project's name */
extern char *getProjectName(const char *name);
#endif