#include "common.h"

void* ptr_to_rva(PVR_PE_FILE pe_file, void* _ptr)
{
        PVR_SECTION iterator;
	void* rva;
	unsigned char* ptr;
	
	ptr=(unsigned char*)_ptr;

	for(iterator=pe_file->section_list; iterator; iterator=iterator->next)
	{
		rva=(void*)(ptr-iterator->data_buffer+iterator->VirtualAddress);
		if((iterator->VirtualAddress +iterator->data_buffer_size > (DWORD)rva) && ((DWORD)rva >= iterator->VirtualAddress)) return rva;
	}

	return 0;
}



void* rva_to_ptr(PVR_PE_FILE pe_file, DWORD rva)
{
        PVR_SECTION iterator;
	void* buffer;

	for(iterator=pe_file->section_list; iterator; iterator=iterator->next)
	{
		if((iterator->VirtualAddress +iterator->data_buffer_size > rva) && (rva >= iterator->VirtualAddress))
		{
			buffer=iterator->data_buffer-iterator->VirtualAddress +rva;
			return buffer;
	        }
	}

	return NULL;
}


VR_ERROR get_imp_table(unsigned char* buffer, PVR_PE_FILE pe_file)
{
        VR_ERROR result =VR_NO_ERROR;

	PIMAGE_IMPORT_DESCRIPTOR table;
	PVR_IMPORT vr_imp=NULL;
	PVR_IMPORT prev =pe_file->import_list;
	PIMAGE_THUNK_DATA name_thunk;
	PIMAGE_THUNK_DATA thunk;
	PIMAGE_IMPORT_BY_NAME import_by_name;
	char* module_name;

	for(table=(PIMAGE_IMPORT_DESCRIPTOR)(buffer); table; table++)
	{
							
		if(0==table->OriginalFirstThunk) break;

		name_thunk=(PIMAGE_THUNK_DATA) rva_to_ptr(pe_file, table->OriginalFirstThunk);
		thunk=(PIMAGE_THUNK_DATA) rva_to_ptr(pe_file,table->FirstThunk);

		do
		{
			if(0 == name_thunk->u1.Ordinal) break; // normal exit

			vr_imp =(PVR_IMPORT) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_IMPORT));
			if(NULL == vr_imp)
			{
				result =VR_NO_MEMORY;
				break;
			}

			module_name =rva_to_ptr(pe_file, table->Name);

			vr_imp->name_library =(char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(module_name) +1);
			if(NULL==vr_imp->name_library)
			{
				result =VR_NO_MEMORY;
				break;
			}

			strcpy(vr_imp->name_library, module_name);

			if(IMAGE_SNAP_BY_ORDINAL32(name_thunk->u1.Ordinal))
			{
				vr_imp->ordinal =IMAGE_ORDINAL32(name_thunk->u1.Ordinal);
			} else
			{
				import_by_name =(PIMAGE_IMPORT_BY_NAME) rva_to_ptr(pe_file,name_thunk->u1.AddressOfData);
				if(import_by_name)
				{
					vr_imp->name_function =(char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(import_by_name->Name) +1);
					if(NULL==vr_imp->name_function)
					{
						result =VR_NO_MEMORY;
						break;
					}

					strcpy(vr_imp->name_function, import_by_name->Name);
					
				} else
				{
					result = VR_NULL_PTR;
					break;
				}
			}

			vr_imp->rva =(DWORD)ptr_to_rva(pe_file, thunk);

			if(prev) prev->next =vr_imp;
			else pe_file->import_list=vr_imp;

			prev =vr_imp;
			vr_imp =NULL;

			name_thunk++;
			thunk++;

		} while(1);

		if(VR_NO_ERROR!=result) break;						      
	}

	if(vr_imp)
	{
		if(vr_imp->name_library) HeapFree(GetProcessHeap(), 0, vr_imp->name_library);
		if(vr_imp->name_function) HeapFree(GetProcessHeap(), 0, vr_imp->name_function);

		HeapFree(GetProcessHeap(), 0, vr_imp);
	}



	return result;

}

VR_ERROR read_import(unsigned char* not_used, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	PIMAGE_DATA_DIRECTORY imp=NULL;
	unsigned char* buffer;
	int i;

	do
	{
		if(sizeof(IMAGE_OPTIONAL_HEADER32)!=pe_file->FileHeader.SizeOfOptionalHeader)
		{
			result =VR_INVALID_OPTIONAL_HEADER_SIZE;
			break;
		}

		imp=(PIMAGE_DATA_DIRECTORY)(pe_file->OptionalHeader->DataDirectory +IMAGE_DIRECTORY_ENTRY_IMPORT);
		if((imp->VirtualAddress == 0) || (imp->Size ==0))
		{
		//	printf("IMAGE_DIRECTORY_ENTRY_IMPORT not found!\r\n");
			break;
		}

                buffer =rva_to_ptr(pe_file,imp->VirtualAddress);
		if(NULL == buffer)
		{
			result =VR_EMPTY_SECTION_LIST;
			break;
		}

		result =get_imp_table(buffer, pe_file);
		if(VR_NO_ERROR != result)
		{
			break;
		}

	} while(0);

	return result;	
}
