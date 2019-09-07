#include <distorm.h>
#include <stego.h>
#include <windows.h>
#include "VR_error.h"
#include "pe-file.h"

void main(int argc, char* argv[])
{
	VR_ERROR result;
	VR_PE_FILE pe_file;
	PVR_SECTION  iterator;
	PVR_SECTION code_section =NULL;
	char information[0x100];
	unsigned long long key =0;
 
	memset(&pe_file, 0, sizeof(VR_PE_FILE));
	memset(information, 0, 0x100);
	
	switch(argc)
	{
		case 4:
		{
                        build_dictionary_list(argv[2]);   

			result =pe_file_readerA(argv[1], &pe_file);
			if(VR_NO_ERROR != result) break;

			set_stego_key(argv[3]);

			result =get_information(&pe_file, information);
			if(VR_NO_ERROR != result) break;

			result =pe_file_closeA(NULL, &pe_file);
			if(VR_NO_ERROR != result) break;

			printf("%s\n", information);

		}break;

		case 6:
		{
                        build_dictionary_list(argv[2]);   

			if(!strcmp("obj", argv[3])) 
			{
				result =coff_file_readerA(argv[1], &code_section);
				if(VR_NO_ERROR != result) break;
	
				result =attach_information(NULL, argv[4], atoi(argv[5]), code_section, &key);
				if(VR_NO_ERROR != result) break;

				result =coff_file_writerA(argv[1], code_section);
				if(VR_NO_ERROR != result) break;

				printf("Stego key: %#llx\n", key);
			} 

			if(!strcmp("pe", argv[3]))
			{
				result =pe_file_readerA(argv[1], &pe_file);
				if(VR_NO_ERROR != result) break;
				
				result =attach_information(&pe_file, argv[4], atoi(argv[5]), NULL, &key);
				if(VR_NO_ERROR != result) break;

				result =pe_file_closeA(argv[1], &pe_file);
				if(VR_NO_ERROR != result) break;
		
				printf("Stego key: %#llx\n", key);
			}

		} break;

		default:
			printf("usage:  <input file> <dictionary> <type> <attach info> <quantity>\r\n");
			printf("        <input file> <dictionary> <stego key>\r\n");
			printf("        dictionary: xor\r\n");
			printf("        dictionary: jz\r\n");
			printf("        dictionary: all\r\n");
			printf("        type: obj\r\n");
			printf("        type: pe\r\n");
	}

	if(VR_NO_ERROR != result)
	{
		if(ERROR_INSUFFICIENT_BUFFER ==result)  printf("This string cannot be nested\n");
		else printf("Error %u\n", result);
	}

	ExitProcess(0);
}
