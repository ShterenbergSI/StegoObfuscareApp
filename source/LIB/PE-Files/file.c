#include "common.h"

#include <imagehlp.h>

VR_ERROR pe_file_write_checksum_internal(HANDLE file, PVR_PE_FILE pe_file, DWORD checksum)
{
        VR_ERROR result =VR_NO_ERROR;
        DWORD bytes;

	do
	{
		if(0==SetFilePointer(file,(pe_file->MZ_header_size +sizeof(DWORD) +sizeof(IMAGE_FILE_HEADER) +0x40), NULL, FILE_BEGIN))
		{
			break;
		} 
     	
		if(!WriteFile(file,&checksum,sizeof(DWORD),&bytes,NULL))
		{      
			result =(VR_WINDOWS_ERROR | GetLastError());
			break;
		}
		if(bytes!=sizeof(DWORD))
		{
			result =VR_PARTIAL_WRITE;
			break;
		}

	} while(0);

       	if(INVALID_HANDLE_VALUE!=file) CloseHandle(file);
	
	return result;

}

VR_ERROR pe_file_write_internal(HANDLE file,PVR_PE_FILE pe_file)
{
	VR_ERROR result=VR_NO_ERROR;
	DWORD  bytes;
	unsigned int PE = 'EP';
	PIMAGE_OPTIONAL_HEADER32 opt_header32;
	PIMAGE_OPTIONAL_HEADER64 opt_header64;

	do
	{
		if(!pe_file->MZ_header)
		{
			result =VR_NOT_MEMORY_STRUCTURE;
			break;
		}

              	if(!WriteFile(file,pe_file->MZ_header,pe_file->MZ_header_size,&bytes,NULL))
		{      
			result =(VR_WINDOWS_ERROR | GetLastError());
			break;
		}
		if(bytes!=pe_file->MZ_header_size)
		{
			result =VR_PARTIAL_WRITE;
			break;
		}
              	if(!WriteFile(file,&PE,sizeof(DWORD),&bytes,NULL))
		{      
			result =(VR_WINDOWS_ERROR | GetLastError());
			break;
		}
		if(bytes!=sizeof(DWORD))
		{                       
			result =VR_PARTIAL_WRITE;
			break;
		}

		if(!&pe_file->FileHeader)
		{
			result =VR_NOT_MEMORY_STRUCTURE;
			break;
		}

              	if(!WriteFile(file,&pe_file->FileHeader,sizeof(IMAGE_FILE_HEADER),&bytes,NULL))
		{      
			result =(VR_WINDOWS_ERROR | GetLastError());
			break;
		}

		if(bytes!=sizeof(IMAGE_FILE_HEADER))
		{
			result =VR_PARTIAL_WRITE;
			break;
		}

		if(!pe_file->OptionalHeader)
		{
			result =VR_NOT_MEMORY_STRUCTURE;
			break;
		}

		if(pe_file->flags & VR_FLAG_SECURITY_FIND)
		{
			switch(pe_file->machine)
			{
				case IMAGE_FILE_MACHINE_I386:
				{
					opt_header32 =(PIMAGE_OPTIONAL_HEADER32) pe_file->OptionalHeader;

					opt_header32->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress =0;
					opt_header32->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size =0;
				} break;

				case IMAGE_FILE_MACHINE_AMD64:
				{
					opt_header64 =(PIMAGE_OPTIONAL_HEADER64) pe_file->OptionalHeader;

					opt_header64->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress =0;
					opt_header64->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size =0;
				} break;

				default: result =VR_INVALID_OPTIONAL_HEADER_SIZE;
			}

			if(VR_NO_ERROR != result) break;
		}

              	if(!WriteFile(file,pe_file->OptionalHeader, pe_file->FileHeader.SizeOfOptionalHeader, &bytes, NULL))
		{      
			result =(VR_WINDOWS_ERROR | GetLastError());
			break;
		}

		if(bytes!=pe_file->FileHeader.SizeOfOptionalHeader)
		{
			result =VR_PARTIAL_WRITE;
			break;
		}

		if(!pe_file->section_list)
		{
			result =VR_NOT_MEMORY_STRUCTURE;
			break;
		}

		result=write_section(file,pe_file);
		if(VR_NO_ERROR!=result)
		{
			break;
		}

	} while(0);

       	if(INVALID_HANDLE_VALUE!=file) CloseHandle(file);


 	return result;
}


VR_ERROR __stdcall pe_file_writeA(char* full_path, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE file;
	DWORD checksum;
	DWORD headersum;

	do
	{
		file=CreateFileA(full_path, GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		if(INVALID_HANDLE_VALUE == file)
		{
			result = (VR_WINDOWS_ERROR | GetLastError());
			break;
		}
		result =pe_file_write_internal(file, pe_file);
		if(VR_NO_ERROR!=result)
		{
			break;
		}


		if(VR_FLAG_CHECKSUM & pe_file->flags)
		{
			result =MapFileAndCheckSumA(full_path, &headersum, &checksum);
			if(CHECKSUM_SUCCESS != result)
			{
				result=VR_ERROR_CHECKSUM;
				break;
			}

			file=CreateFileA(full_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if(INVALID_HANDLE_VALUE == file)
			{
				result = (VR_WINDOWS_ERROR | GetLastError());
				break;
			}

			result =pe_file_write_checksum_internal(file,pe_file,checksum);
			if(VR_NO_ERROR!=result)
			{
				break;
			}
						
		}


	} while(0);
	
//       	if(INVALID_HANDLE_VALUE!=file) CloseHandle(file);


	return result;
}

VR_ERROR __stdcall pe_file_writeW(wchar_t* full_path, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE file;
	DWORD checksum;
	DWORD headersum;


	do
	{
		file=CreateFileW(full_path, GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		if(INVALID_HANDLE_VALUE == file)
		{
			result = (VR_WINDOWS_ERROR | GetLastError());
			break;
		}
		result =pe_file_write_internal(file, pe_file);
		if(VR_NO_ERROR!=result)
		{
			break;
		}

		if(VR_FLAG_CHECKSUM & pe_file->flags)
		{
			result =MapFileAndCheckSumW(full_path, &headersum, &checksum);
			if(CHECKSUM_SUCCESS != result)
			{
				printf("error in MapFileAndCheckSumW= %u %S\r\n",result,full_path);
				result=VR_ERROR_CHECKSUM;
				break;
			}

			file=CreateFileW(full_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if(INVALID_HANDLE_VALUE == file)
			{
				result = (VR_WINDOWS_ERROR | GetLastError());
				break;
			}

			result =pe_file_write_checksum_internal(file,pe_file,checksum);
			if(VR_NO_ERROR!=result)
			{
				break;
			}
						
		}


	} while(0);
	
//       	if(INVALID_HANDLE_VALUE!=file) CloseHandle(file);

	return result;
}

VR_ERROR check_reloc(unsigned char* buffer, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;

	PIMAGE_DATA_DIRECTORY reloc;
	DWORD rva;
	PIMAGE_SECTION_HEADER section;
	PVR_SECTION iterator;
	int i;


	if(sizeof(IMAGE_OPTIONAL_HEADER32)==pe_file->FileHeader.SizeOfOptionalHeader)
	{
		reloc=(PIMAGE_DATA_DIRECTORY)(buffer + pe_file->MZ_header_size + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER)+ 136); // 128-offset reloc table
                section =(PIMAGE_SECTION_HEADER)(buffer + pe_file->MZ_header_size + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + pe_file->FileHeader.SizeOfOptionalHeader);

		rva=pe_file->OptionalHeader->BaseOfCode;

                iterator =pe_file->section_list;
        do
        {
		for(i=0; i <pe_file->FileHeader.NumberOfSections; i++)
		{
			if(iterator->VirtualAddress==reloc->VirtualAddress)
			{
			
			}

		}


		iterator =iterator->next;
		  

	} while(iterator);
		
	}
	return result;
}

/*
	Verification algo

	1) VR_FILE_TOO_LARGE file size >= 0x100000000 bytes
	2) VR_FILE_TOO_SMALL
	3) VR_NOT_MZ_FILE
	4) VR_UNKNOWN_SIGNATURE
	5) VR_CPU_NOT_SUPPORTED

*/

VR_ERROR __stdcall pe_file_reader_internal(HANDLE file, PVR_PE_FILE pe_file)
{
	VR_ERROR result=VR_NO_ERROR;

	PIMAGE_DOS_HEADER dos_header =NULL;
	PIMAGE_NT_HEADERS nt =NULL;
	PIMAGE_OPTIONAL_HEADER32 opt_header32;
	PIMAGE_OPTIONAL_HEADER64 opt_header64;

	unsigned char* buffer=NULL;
	DWORD size_low;
	DWORD size_high;
	DWORD bytes;

	do
	{
		size_low =GetFileSize(file, &size_high);
		if(size_high)
		{
			result =VR_FILE_TOO_LARGE;
			break;
		}

	        if(size_low < (sizeof(IMAGE_DOS_HEADER) + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER32)))
		{
			result =VR_FILE_TOO_SMALL;
			break;
		}
		pe_file->total_size=size_low;
	
	        buffer =HeapAlloc(GetProcessHeap(), 0, size_low);
	        if(!buffer)
		{
			result =VR_NO_MEMORY;
			break;
		}

                if(!ReadFile(file,buffer, size_low, &bytes,NULL))
		{
			result = (VR_WINDOWS_ERROR | GetLastError());
			break;
		}

		if(bytes != size_low)
		{
			result =VR_PARTIAL_READ;
			break;
		}
		
		dos_header =(PIMAGE_DOS_HEADER) buffer;
		if(IMAGE_DOS_SIGNATURE != dos_header->e_magic)
		{
			result =VR_NOT_MZ_FILE;
			break;
		}


		if(size_low < dos_header->e_lfanew)
		{
			result =VR_MSDOS_EXECUTABLE;
			break;
		}

		pe_file->MZ_header_size =dos_header->e_lfanew;

	  	pe_file->MZ_header =(unsigned char*) HeapAlloc(GetProcessHeap(), 0, pe_file->MZ_header_size);
	  	if(!pe_file->MZ_header)
		{
			result =VR_NO_MEMORY;
			break;
		}
		memcpy(pe_file->MZ_header, buffer, pe_file->MZ_header_size);

		nt =(PIMAGE_NT_HEADERS) (buffer + dos_header->e_lfanew);
		switch(nt->Signature)
		{
			case IMAGE_NT_SIGNATURE: break;

			case IMAGE_OS2_SIGNATURE: result =VR_OS2_16BIT_EXECUTABLE; break;
			case IMAGE_VXD_SIGNATURE: result =VR_OS2_32BIT_EXECUTABLE; break;

			default: result =VR_UNKNOWN_SIGNATURE;
		}

		if(result != VR_NO_ERROR) break;

		pe_file->machine =nt->FileHeader.Machine;

		memcpy(&pe_file->FileHeader, &nt->FileHeader, sizeof(IMAGE_FILE_HEADER));
		
		pe_file->OptionalHeader =(PIMAGE_OPTIONAL_HEADER) HeapAlloc(GetProcessHeap(), 0, pe_file->FileHeader.SizeOfOptionalHeader);
	  	if(!pe_file->OptionalHeader)
		{
			result =VR_NO_MEMORY;
			break;
		}
		memcpy(pe_file->OptionalHeader, &(nt->OptionalHeader), pe_file->FileHeader.SizeOfOptionalHeader);

		if(!pe_file->OptionalHeader->AddressOfEntryPoint)
		{
			result=VR_PE_FILE_NOT_EXECUTABLE;
			break;
		}

        	if(0 == pe_file->FileHeader.NumberOfSections)
		{
			result =VR_EMPTY_SECTION_LIST;
			break;
		}

		if((pe_file->FileHeader.PointerToSymbolTable) || (pe_file->FileHeader.NumberOfSymbols))
		{
			// to do
		//	result =VR_SYMBOLS_NOT_SUPPORTED;
			break;
		}

		switch(pe_file->FileHeader.SizeOfOptionalHeader)
		{
			case sizeof(IMAGE_OPTIONAL_HEADER32): break;
			case sizeof(IMAGE_OPTIONAL_HEADER64): result =VR_CPU_NOT_SUPPORTED; break;

			default: result =VR_INVALID_OPTIONAL_HEADER_SIZE;
		}

		if(VR_NO_ERROR != result) break;

		if(pe_file->OptionalHeader->CheckSum!=0) pe_file->flags |=VR_FLAG_CHECKSUM;
		
		switch(pe_file->machine)
		{
			case IMAGE_FILE_MACHINE_I386:
			{
				opt_header32 =(PIMAGE_OPTIONAL_HEADER32) pe_file->OptionalHeader;

				if(opt_header32->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress && opt_header32->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size)
				{
	                        	pe_file->flags |=VR_FLAG_SECURITY_FIND;
				}
			} break;

			case IMAGE_FILE_MACHINE_AMD64:
			{
				opt_header64 =(PIMAGE_OPTIONAL_HEADER64) pe_file->OptionalHeader;

				if(opt_header64->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress && opt_header64->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size)
				{
        	                	pe_file->flags |=VR_FLAG_SECURITY_FIND;
				}
			} break;

			default: result =VR_INVALID_OPTIONAL_HEADER_SIZE;
		}

	 	if(VR_NO_ERROR != result) break;

         	result=read_section(buffer, pe_file);
	 	if(VR_NO_ERROR != result)
		{
			break;
		}

		if(pe_file->flags &VR_FLAG_SECTION_TABLE_FULL)
		{
			// to do
			// break;
		}
		
         	result=read_import(buffer, pe_file);
	 	if(VR_NO_ERROR != result)
		{
			break;
		}

		pe_file->flags |=VR_FLAG_CAN_WRITE;

	} while(0);

	if(buffer) HeapFree(GetProcessHeap(), 0, buffer);

	return result;
}

VR_ERROR __stdcall pe_file_readerA(char* full_path, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE file;

	file =CreateFileA(full_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE != file)
	{
		result =pe_file_reader_internal(file, pe_file);

		CloseHandle(file);

	} else result = (VR_WINDOWS_ERROR | GetLastError());

	return result;
}

VR_ERROR __stdcall pe_file_readerW(wchar_t* full_path, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	HANDLE file;

	file =CreateFileW(full_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE != file)
	{
		result =pe_file_reader_internal(file, pe_file);

		CloseHandle(file);

	} else  result = (VR_WINDOWS_ERROR | GetLastError());

	return result;
}

void __stdcall pe_file_close_internal(PVR_PE_FILE pe_file)
{
	PVR_SECTION temp=NULL;
	PVR_SECTION temp2=NULL;	
	PVR_IMPORT iterator;
	PVR_IMPORT ptr;


	if(pe_file->MZ_header) HeapFree(GetProcessHeap(),0,pe_file->MZ_header);
	if(pe_file->OptionalHeader) HeapFree(GetProcessHeap(),0,pe_file->OptionalHeader);

    	while(pe_file->section_list)
	{
		if(pe_file->section_list->data_buffer)
		{
		        HeapFree(GetProcessHeap(),0,pe_file->section_list->data_buffer);
		}

		temp=pe_file->section_list;
		pe_file->section_list =pe_file->section_list->next;
		HeapFree(GetProcessHeap(),0,temp);
	}

	for(iterator=pe_file->import_list; iterator;)
	{
		if(iterator->name_library) HeapFree(GetProcessHeap(),0,iterator->name_library);
		if(iterator->name_function) HeapFree(GetProcessHeap(),0,iterator->name_function);

		ptr =iterator;
		iterator=iterator->next;

		HeapFree(GetProcessHeap(), 0, ptr);
	}
	


}

unsigned int __stdcall pe_file_closeA(char* full_path, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	PVR_SECTION temp=NULL;	

	if((pe_file->flags &VR_FLAG_CAN_WRITE) && full_path)
	{
		result=pe_file_writeA(full_path, pe_file);
	}

	pe_file_close_internal(pe_file);

	return result;
}

unsigned int __stdcall pe_file_closeW(wchar_t* full_path, PVR_PE_FILE pe_file)
{
	VR_ERROR result=VR_NO_ERROR;
	PVR_SECTION temp=NULL;	

	if((pe_file->flags &VR_FLAG_CAN_WRITE) && full_path)
	{
		result=pe_file_writeW(full_path, pe_file);
	}

	pe_file_close_internal(pe_file);

	return result;
}
