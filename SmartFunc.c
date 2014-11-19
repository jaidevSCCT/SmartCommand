#include "PCSC/SmartCommand.h"

void CreateCommandList(cmd_list **root,char * buf)
{
	cmd_list *temp = NULL,*myhead = *root;
	if(*root == NULL)
	{
		temp = (cmd_list *) malloc(sizeof(cmd_list));
		temp->cmd = buf;
		temp->next = NULL;
		*root = temp;
	}
	else
	{
		while(myhead->next != NULL)
			myhead = myhead->next;
		temp = (cmd_list *) malloc(sizeof(cmd_list));
        	temp->cmd = buf;
         	temp->next = NULL;
        	myhead->next = temp;
	}

}

LPCBYTE StrToHex(char *hexStr)
{
	int i;
    	uint8_t str_len = strlen(hexStr);
	hex[0] = ((uint8_t)str_len/(uint8_t)2);
//	printf("%u\n",hex[0]);
    	for (i = 0; i < (str_len / 2); i++) 
	{
        	sscanf(hexStr + 2*i, "%02x", &hex[i+1]);
//		printf("bytearray %d: %02x\n", i, hex[i+1]);
    	}
	return hex;
}

int8_t readFileSize(char * argv,struct stat *info)
{
        int8_t ret;
        ret = stat(argv,info);
        if(ret == -1)
                return -1;
        else
                return 0;
}

void freeCommandList(cmd_list *freehead)
{
	cmd_list *temp=NULL;
	while(freehead != NULL)
	{
		temp=freehead->next;
		free(freehead);
		freehead=temp;
		
	}
}

