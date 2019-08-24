#include <windows.h>

// working on file read to memory and loaded image
//
PIMAGE_NT_HEADERS __stdcall get_PE_header(void* buffer)
{
	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS nt =NULL;

	dos_header =(PIMAGE_DOS_HEADER) buffer;
	if (dos_header->e_magic == IMAGE_DOS_SIGNATURE)
	{
		nt =(PIMAGE_NT_HEADERS)((unsigned char*)buffer + dos_header->e_lfanew);

        	if(nt->Signature == IMAGE_NT_SIGNATURE)
		{
			return nt;
		}
	}
	return NULL;	
}

// working on file read to memory and loaded image
//
PIMAGE_SECTION_HEADER __stdcall get_section_header(void* buffer)
{
	PIMAGE_NT_HEADERS nt =get_PE_header(buffer);
        unsigned char* ptr =(unsigned char*) nt;

	return ptr ? (PIMAGE_SECTION_HEADER) (ptr +sizeof(DWORD) +sizeof(IMAGE_FILE_HEADER) +nt->FileHeader.SizeOfOptionalHeader) : NULL;
}

// align address to value
//
DWORD __stdcall alignment(DWORD size, DWORD alignment)
{
	return (size % alignment) ? (size/alignment +1) * alignment : (size/alignment) * alignment;
}

// working on file read to memory
//
DWORD __stdcall RVA_to_FVA(void* buffer, DWORD rva)
{
	PIMAGE_NT_HEADERS nt =get_PE_header(buffer);
	PIMAGE_SECTION_HEADER section =get_section_header(buffer);
	int i;

	if(section)
	{
		for(i =0; i <nt->FileHeader.NumberOfSections; i++, section++)
		{
			if((rva >= section->VirtualAddress) &&
				(rva < alignment(section->VirtualAddress + section->Misc.VirtualSize, nt->OptionalHeader.SectionAlignment)))
			{
				return rva -section->VirtualAddress +section->PointerToRawData;
			}
		}
	}
	return 0;
}
