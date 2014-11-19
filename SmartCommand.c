/*
 * Sample program to use PC/SC API.
 *
 * Copyright (C) 2014-2015
 * Jaidev Bhattacharjee <jaidev.csit.etc@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it.
 */

#include "PCSC/SmartCommand.h"

/* PCSC error message pretty print */
#define PCSC_ERROR(retPCSC, text) \
if (retPCSC != SCARD_S_SUCCESS) \
{ \
	printf(text ": %s (0x%lX)\n", pcsc_stringify_error(retPCSC), retPCSC); \
	goto end; \
} \
else \
{ \
	printf(text ": OK\n\n"); \
}

int main(int argc, char *argv[])
{
	LONG retPCSC;
	SCARDCONTEXT hContext;
	DWORD dw_readers;
	LPSTR msz_readers = NULL;
	char *ptr, **readers = NULL;
	int ReaderCount;
	SCARDHANDLE hCard;
	DWORD dw_active_protocol,dwReaderLen, dwState, dwProt, dwAtrLen;
	BYTE pbAtr[MAX_ATR_SIZE] = "";
	char pbReader[MAX_READERNAME] = "";
	int Reader_option;
	const SCARD_IO_REQUEST *pioSendPci;
	SCARD_IO_REQUEST pioRecvPci;
	BYTE pbRecvBuffer[10];
	DWORD dwSendLength, dwRecvLength;
	LPCBYTE cmd; 
	struct stat info; 	/*to get the file size dynamically*/
	ssize_t readLen;	/*return lenght of read()*/
	size_t fileLen; 	/*3 arg of read()*/
	uint8_t fd,i,hexlen;
	char *buf,*strtk;
	int choice;
	cmd_list *head = NULL;


	if(argc < 3)
	{
		fprintf(stderr,"Usage %s <reader index> <filename.txt> \n",argv[0]);
		exit(EXIT_FAILURE);
	}

	if(readFileSize(argv[2],&info) != 0)
	{
		printf("Unable to read file size exiting program \n");
		exit(EXIT_FAILURE);
	}
	printf("Smart Command\n");
	printf("V 1.0 2014-2015 \n");
	
	printf("\nTHIS PROGRAM IS  DESIGNED AS A TESTING TOOL FOR END USERS!\n");

	printf("You must ensure that you have inserted you card properly \n");
        printf("press 1 to enter and 2 to exit \n");
 /* This is done to minimize the warning */
/* warning: ignoring return value of ‘scanf’, declared with attribute warn_unused_result*/
	 if(scanf("%d",&choice) != 1) 	
		printf("Failed to read integer \n");

        while(choice == 1)
        {

			head = NULL ; strtk = NULL;			
			fileLen = (size_t) info.st_size;
			fd = open(argv[2],O_RDONLY);
			if(fd == -1)
				perror("open");
			buf = (char *)calloc(fileLen,sizeof(char));
			readLen = read(fd,buf,fileLen);
			if(readLen == -1)
				perror("read");
			else if(readLen == 0)
				printf("Nothin to read \n");
			else
				printf("Reading of file success with  %d number of bytes\n",readLen);
	
			strtk = strtok(buf,";");
			while(strtk != NULL)
			{
				CreateCommandList(&head,strtk);
				strtk = strtok(NULL,";");
			}

		retPCSC = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
		if (retPCSC != SCARD_S_SUCCESS)
			printf("SCardEstablishContext: Cannot Connect to Resource Manager %lX\n", retPCSC);
		else
		{

			/* Retrieve the available readers list. */
			dw_readers = SCARD_AUTOALLOCATE;
			retPCSC = SCardListReaders(hContext, NULL, (LPSTR)&msz_readers, &dw_readers);
			if(retPCSC != SCARD_S_SUCCESS)
				PCSC_ERROR(retPCSC, "SCardListReaders")
			else
			{

				/* Extract readers from the null separated string and get the total
		 		* number of readers */
				ReaderCount = 0;
				ptr = msz_readers;
				while (*ptr != '\0')
				{
					ptr += strlen(ptr)+1;
					ReaderCount++;
				}

				if (ReaderCount == 0)
				{
					printf("No reader found\n");
					goto end;
				}

			/* allocate the readers table */
				readers = calloc(ReaderCount, sizeof(char *));
				if (NULL == readers)
				{
					printf("Not enough memory for readers[]\n");
					goto end;
				}

			/* fill the readers table */
				ReaderCount = 0;
				ptr = msz_readers;
				while (*ptr != '\0')
				{
					printf("%d: %s\n", ReaderCount, ptr);
					readers[ReaderCount] = ptr;
					ptr += strlen(ptr)+1;
					ReaderCount++;
				}

				if (argc > 1)
				{
					Reader_option = atoi(argv[1]);
					if (Reader_option < 0 || Reader_option >= ReaderCount)
					{
						printf("Wrong reader index: %d\n", Reader_option);
						goto end;
					}
				}	
				else
					Reader_option = 0;
			}

			/* connect to a card */
			dw_active_protocol = -1;
			retPCSC = SCardConnect(hContext, readers[Reader_option], SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,&hCard,&dw_active_protocol);
			printf(" Protocol: %ld\n", dw_active_protocol);
			PCSC_ERROR(retPCSC, "SCardConnect")

			/* get card status */
			dwAtrLen = sizeof(pbAtr);
			dwReaderLen = sizeof(pbReader);
			retPCSC = SCardStatus(hCard, /*NULL*/ pbReader, &dwReaderLen, &dwState, &dwProt,
			pbAtr, &dwAtrLen);
			printf(" Reader: %s (length %ld bytes)\n", pbReader, dwReaderLen);
			printf(" State: 0x%lX\n", dwState);
			printf(" Prot: %ld\n", dwProt);
			printf(" ATR (length %ld bytes):", dwAtrLen);
			for (i=0; i<dwAtrLen; i++)
				printf(" %02X", pbAtr[i]);
			printf("\n");
			PCSC_ERROR(retPCSC, "SCardStatus")

   			 switch(dw_active_protocol)
    			{
        			case SCARD_PROTOCOL_T0:
            				pioSendPci = SCARD_PCI_T0;
            				break;
       				case SCARD_PROTOCOL_T1:
           				pioSendPci = SCARD_PCI_T1;
            				break;
       				 default:
            				printf("Unknown protocol\n");
            				goto end;
    			}

			cmd_list *data = head;
			while(data != NULL)
			{
			//	printf("%s\n",head->cmd);
			cmd = StrToHex(data->cmd);
			hexlen = (uint8_t)cmd[0];
			/* exchange APDU */
			dwSendLength = hexlen;
			dwRecvLength = sizeof(pbRecvBuffer);
			printf("Sending: ");
			for (i=1; i<=dwSendLength; i++)
				printf("%02X ",cmd[i]);
			printf("\n");
			retPCSC = SCardTransmit(hCard, pioSendPci,cmd+1, dwSendLength,&pioRecvPci, pbRecvBuffer, &dwRecvLength);
			printf("Received: ");
			for (i=0; i<dwRecvLength; i++)
				printf("%02X ", pbRecvBuffer[i]);
			printf("\n");
			PCSC_ERROR(retPCSC, "SCardTransmit")
			data = data->next;
			}	
			/* card disconnect */
			retPCSC = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
			PCSC_ERROR(retPCSC, "SCardDisconnect")

end:
			/* free allocated memory */
			if (msz_readers)
			SCardFreeMemory(hContext, msz_readers);

			/* We try to leave things as clean as possible */
			retPCSC = SCardReleaseContext(hContext);
			if (retPCSC != SCARD_S_SUCCESS)
				printf("SCardReleaseContext: %s (0x%lX)\n", pcsc_stringify_error(retPCSC),retPCSC);
			if (readers)
				free(readers);
		}	
		freeCommandList(head);
		free(buf);

	printf("Enter 1 to send more Command or 2 to Quit\n");
        choice = 0;
        if(scanf("%d",&choice) != 1)
                printf("Failed to read integer \n");
	}
	return 0;
} /* main */

