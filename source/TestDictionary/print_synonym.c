#include <windows.h>
#include "dictionary.h"

VR_ERROR _stdcall file_to_buffer_internal(HANDLE file, unsigned char** buffer, DWORD* size)
{
	VR_ERROR result =VR_NO_ERROR;

	DWORD size_high;
	DWORD bytes;


	do
	{	
		*size =GetFileSize(file, &size_high);
		if(size_high)
		{
			result =VR_FILE_TOO_LARGE;
			break;
		}
		*buffer =HeapAlloc(GetProcessHeap(), 0, *size);
        	if(!*buffer)
		{
			result =VR_NO_MEMORY;
			break;
		}
		SetFilePointer(file, 0, NULL, FILE_BEGIN);

        	if(!ReadFile(file, *buffer, *size, &bytes, NULL))
		{
			result = (VR_WINDOWS_ERROR | GetLastError());
			break;
		}

		if(bytes != *size)
		{
			result =VR_PARTIAL_READ;
			break;
		}


	}while(0);

	return result;
}

VR_ERROR _stdcall file_to_bufferW(wchar_t* file_name, unsigned char** buffer, DWORD* size)
{

        VR_ERROR result =VR_NO_ERROR;
	HANDLE file;


	file= CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE != file)
	{
		result= file_to_buffer_internal(file,buffer,size);
	}

        if(file)CloseHandle(file);

	return result;

}


void entry(void)
{
	VR_ERROR result=VR_NO_ERROR;
	int argc;
	LPWSTR* argv;
	unsigned char* buffer;
	DWORD size;
	PVR_SYNONYM synonym;


	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	switch(argc)
	{
		case 2:
		{ 
			do
			{
				result=file_to_bufferW(argv[1],&buffer,&size);
				if(VR_NO_ERROR!=result)
				{
					break;
				}
				result =from_buffer_to_list(buffer);
				if(VR_NO_ERROR != result)
				{
					break;
				}

                               	for(synonym =synonym_root; synonym; synonym =synonym->next) print_synonym("print_synonym" ,synonym);

				printf("Sinonym number= %#02x\r\n", (buffer[12]));

			}while(0);			

		}break;

		default:

			usage:
			printf("usage: print_synonym <file_name>");
	}

	HeapFree(GetProcessHeap(), 0, buffer);

	ExitProcess(0);

}


 