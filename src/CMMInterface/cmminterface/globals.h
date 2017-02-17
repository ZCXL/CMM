#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
/* file list */
typedef struct FileList{
	char *name;
	struct FileList *next;
}FileList,*PFileList;
#endif