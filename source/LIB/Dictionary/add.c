#include <windows.h>
#include <dictionary.h>

VR_ERROR add_increment(void)
{
	VR_ERROR result=VR_NO_ERROR;
	PVR_SYNONYM iterator;
	PVR_SYNONYM ptr;


	for(iterator =synonym_root; iterator; iterator =iterator->next)
	{
		if(iterator->src.size==iterator->dst.size)
		{

			ptr =HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SYNONYM));
        		if(!ptr)
			{
				result =VR_NO_MEMORY;
				break;
			}

			memcpy(ptr->src.body,iterator->dst.body, VR_MAX_SYNONYM_SIZE);
			memcpy(ptr->dst.body,iterator->src.body, VR_MAX_SYNONYM_SIZE);
                        ptr->src.size=iterator->dst.size;
                        ptr->dst.size=iterator->src.size;

			result=check_synonym(ptr);
			if(VR_NO_ERROR==result)
			{
				ptr->next=synonym_root;
				synonym_root=ptr;
			}

		}	
	}

	return result;
}

VR_ERROR check_synonym(PVR_SYNONYM synonym)
{
	VR_ERROR result=VR_NO_ERROR;
	PVR_SYNONYM iterator;

	print_synonym("check on entry", synonym);

	for(iterator =synonym_root; iterator; iterator =iterator->next)
	{
		do
		{
			print_synonym("check iterator", iterator);

			if(synonym->src.size != iterator->src.size) 
			{
				break;
			}

			if(memcmp((synonym->src.body), iterator->src.body, iterator->src.size))
			{
				break;
			}

			if(synonym->dst.size != iterator->dst.size) 
			{
				break;
			}
			                      
			if(!memcmp((synonym->dst.body), iterator->dst.body, iterator->dst.size))
			{
				result=VR_ERROR_DUPLICATE_SYNONYM;
				return result;
			}

			
		
		}while(0);
	}

	return result;	

}

VR_ERROR add_synonym_internal(HANDLE file, PVR_SYNONYM synonym)
{
	VR_ERROR result=VR_NO_ERROR;
	PVR_SYNONYM_FILE_HEADER file_header;
	PVR_SYNONYM increment;

	unsigned char* buffer;
	DWORD size_low;
	DWORD size_high;
	DWORD bytes;
	int i;
	int src;
	int dst;

	        
	do
	{
		print_synonym("add", synonym);

		size_low =GetFileSize(file, &size_high);
		if(size_high)
		{
			result =VR_FILE_TOO_LARGE;
			break;
		}

		if(size_low)
		{
		        buffer =HeapAlloc(GetProcessHeap(), 0, size_low);
        		if(!buffer)
			{
				result =VR_NO_MEMORY;
				break;
			}

			SetFilePointer(file, 0, NULL, FILE_BEGIN);

        	       	if(!ReadFile(file, buffer, size_low, &bytes, NULL))
			{
				result = (VR_WINDOWS_ERROR | GetLastError());
				break;
			}

			if(bytes != size_low)
			{
				result =VR_PARTIAL_READ;
				break;
			}

			result =from_buffer_to_list(buffer);
			if(VR_NO_ERROR != result)
			{
				break;
			}

			HeapFree(GetProcessHeap(), 0, buffer);
			buffer =NULL;
		}

		result =check_synonym(synonym);
		if(VR_NO_ERROR != result)
		{   
			printf("This synonym already exists\r\n");
			break;
		}

		synonym->next =synonym_root;
		synonym_root =synonym;

                result=add_increment();
		

		file_header =from_list_to_buffer();

		if(file_header)
		{
			SetFilePointer(file, 0, NULL, FILE_BEGIN);
	
        	      	if(!WriteFile(file, file_header, file_header->total_size, &bytes, NULL))
			{
				result =(VR_WINDOWS_ERROR | GetLastError());
				break;
			}

			if(bytes != file_header->total_size)
			{
				result =VR_PARTIAL_WRITE;
				break;
			}
		}

	} while(0);

	if(buffer) HeapFree(GetProcessHeap(), 0, buffer);
	if(file) CloseHandle(file);

	return result;	
}  	

VR_ERROR add_synonymA(char* file_name, PVR_SYNONYM synonym)
{
	VR_ERROR result =VR_NO_ERROR;

	HANDLE file;

	file =CreateFileA(file_name, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE != file)
	{
		add_synonym_internal(file, synonym);  	
		
	}else result = (VR_WINDOWS_ERROR | GetLastError()); 

	CloseHandle(file);

//	print_synonym(synonym);

	return result;
}

VR_ERROR add_synonymW(wchar_t* file_name, PVR_SYNONYM synonym)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE file;

	printf("add_synonymW(%S, %p)\r\n", file_name, synonym);
//	print_synonym("addW", synonym);

	file =CreateFileW(file_name, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE != file)
	{
		add_synonym_internal(file, synonym);  	
	
	}else result = (VR_WINDOWS_ERROR | GetLastError());

	CloseHandle(file);

	return result;

}
