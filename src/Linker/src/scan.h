#ifndef _SCAN_H_
#define _SCAN_H_
#include "globals.h"
/*The token string that stores the lexme of each token*/
extern char tokenString[MAXTOKENLEN+1];
/*The function of returning next token in source file*/
extern PInstruction getInstructions(void);
#endif