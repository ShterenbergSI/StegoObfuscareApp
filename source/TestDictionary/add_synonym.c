#include <windows.h>
#include "dictionary.h"

unsigned int letter_to_halfbyte(wchar_t letter)
{
	switch(letter)
	{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a':
		case 'A': return 0xa;
		case 'b':
		case 'B': return 0xb;
		case 'c':
		case 'C': return 0xc;
		case 'd':
		case 'D': return 0xd;
		case 'e':
		case 'E': return 0xe;
		case 'f':
		case 'F': return 0xf;

		default: return 256;
	}
}

unsigned int string_to_byte(wchar_t* string)
{
	unsigned int result;
	unsigned int byte;

	byte =letter_to_halfbyte(string[1]);
	if(byte > 255)
	{
		printf("invalid character %C\r\n", string[1]);
		return byte;
	}

	result =byte;

	byte =letter_to_halfbyte(string[0]);
	if(byte > 255)
	{
		printf("invalid character %C\r\n", string[0]);
		return byte;
	}

	result |= (byte <<4);

	return result;
}

PVR_SYNONYM_RECORD_BODY string_to_record_bodyW(wchar_t* string)
{
	unsigned char bin[VR_MAX_SYNONYM_SIZE];
	unsigned char size;
	PVR_SYNONYM_RECORD_BODY result;
	wchar_t *substring;
	wchar_t separators[] =L" ";
	unsigned int byte;
	unsigned int i;

	substring =wcstok(string, separators);
	for(size =0; substring; size++)
	{
	//	printf("%u) %S\r\n", size, substring);

		if(size >= VR_MAX_SYNONYM_SIZE)
		{
			printf("buffer too large\r\n");
			return NULL;
		}

		if(2 == wcslen(substring))
		{
			byte =string_to_byte(substring);
			if(byte > 255)
			{
				return NULL;
			}

		//	printf("%u) %02x\r\n", size, byte);

			bin[size] =byte;

		} else return NULL;

		substring =wcstok(NULL, separators);
	}

	//printf("total bytes:%u ", size);
	//for(i =0; i <size; i++) printf("%02x ", bin[i]); printf("\r\n");

	result =(PVR_SYNONYM_RECORD_BODY) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SYNONYM_RECORD_BODY) +(size -sizeof(unsigned char)));
	if(result)
	{
		result->size =size;
		memcpy(result->body, bin, size);
	}

	return result;
}


void entry(void)
{
	
	VR_ERROR result;
	PVR_SYNONYM_RECORD_BODY rb_src =NULL;
	PVR_SYNONYM_RECORD_BODY rb_dst =NULL;
	PVR_SYNONYM synonym;

	int argc;
	LPWSTR* argv;
	int i;

	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	switch(argc)
	{
		case 4:
		{
			rb_src =string_to_record_bodyW(argv[2]);
			rb_dst =string_to_record_bodyW(argv[3]);
			if(rb_src && rb_dst)
			{
				synonym =(PVR_SYNONYM) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SYNONYM));
				if(synonym)
				{
					synonym->src.size =rb_src->size;
					synonym->dst.size =rb_dst->size;

					memcpy(synonym->src.body, rb_src->body, rb_src->size);
					memcpy(synonym->dst.body, rb_dst->body, rb_dst->size);

					result =add_synonymW(argv[1], synonym);

					HeapFree(GetProcessHeap(), 0, synonym);

				} else printf("malloc failed\r\n");
			}

			if(rb_src) HeapFree(GetProcessHeap(), 0, rb_src);
			if(rb_dst) HeapFree(GetProcessHeap(), 0, rb_dst);

		} break;

		default:

		usage:
		printf("usage: add_synonym.exe <file_name> <src> <dst>");

	}

	ExitProcess(0);
}
