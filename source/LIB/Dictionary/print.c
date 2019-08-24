#include <windows.h>
#include <dictionary.h>

void print_synonym(char* comment, PVR_SYNONYM synonym)
{
	int i;

        if(synonym)
	{
		printf("%s:\r\n",comment);

		printf("synonym->src.body: [");
                for(i=0; i < synonym->src.size; i++) printf("%02x ", synonym->src.body[i]); printf("] ");
		printf("size= %x\r\n", synonym->src.size);	

		printf("synonym->dst.body: [");
                for(i=0; i < synonym->dst.size; i++) printf("%02x ", synonym->dst.body[i]); printf("] ");
		printf("size= %x\r\n\n", synonym->dst.size);	

	} else printf("%s: print_synonym NULL ptr\r\n", comment);

}
