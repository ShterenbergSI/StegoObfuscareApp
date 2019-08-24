#include <dictionary.h>

void load_dictionaryA(void)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE find;
	HANDLE file;
	WIN32_FIND_DATA data;
	unsigned char* dictionary;
	DWORD	dictionary_size;

        find=FindFirstFileA("*.vr",&data);
	
	if(INVALID_HANDLE_VALUE != find)
	{
	 	do
		{
			result=file_to_bufferA(data.cFileName,&dictionary,&dictionary_size);
			if(VR_NO_ERROR!=result)
			{
				break;
			}

     			result=from_buffer_to_list(dictionary);
			if(VR_NO_ERROR != result)
			{
				break;
			}

			if(!FindNextFileA(find, &data))
			{
				break;
			}

		}while(1);

	}

        if(dictionary)HeapFree(GetProcessHeap(), 0, dictionary);

}



void load_dictionaryW(void)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE find;
	HANDLE file;
	WIN32_FIND_DATAW data;
	unsigned char* dictionary;
	DWORD	dictionary_size;


        find=FindFirstFileW(L"*.vr",&data);
	
	if(INVALID_HANDLE_VALUE != find)
	{

	 	do
		{
			result=file_to_bufferW(data.cFileName,&dictionary,&dictionary_size);
			if(VR_NO_ERROR!=result)
			{
				break;
			}

     			result=from_buffer_to_list(dictionary);
			if(VR_NO_ERROR != result)
			{
				break;
			}

			if(!FindNextFileW(find, &data))
			{
				break;
			}

		}while(1);

	}

        if(dictionary)HeapFree(GetProcessHeap(), 0, dictionary);

}
