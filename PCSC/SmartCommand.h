#ifndef __SMARTCOMMAND_h__
#define __SMARTCOMMAND_h__


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "wintypes.h"
#include "winscard.h"
#include <strings.h>
#include <stdint.h>
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define BUFF  4096

BYTE hex[BUFF];
typedef struct cmdList
{
	char *cmd;
	struct cmdList *next;
}cmd_list;


LPCBYTE StrToHex(char *);
int8_t readFileSize(char*, struct stat *info); /*To read the size of the file dynamically*/
void CreateCommandList(cmd_list **,char *);
void freeCommandList(cmd_list *freehead);

#endif
