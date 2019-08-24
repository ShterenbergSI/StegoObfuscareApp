#ifndef __PE_FILE_H
#define __PE_FILE_H

#include <windows.h>
#include <VR_error.h>

typedef struct _VR_SECTION
{
	struct _VR_SECTION* next;

	BYTE  Name[IMAGE_SIZEOF_SHORT_NAME+1];
        DWORD VirtualSize;
	DWORD VirtualAddress;
  	DWORD FileSize;
	DWORD FileAddress;
	DWORD Characteristics;

	unsigned char* data_buffer;
	unsigned int data_buffer_size;

} VR_SECTION, *PVR_SECTION;

#define VR_FLAG_CHECKSUM		1
#define VR_FLAG_SECTION_TABLE_FULL	2
#define VR_FLAG_SECURITY_FIND		4
#define VR_FLAG_CAN_WRITE		8
#define VR_FLAG_VR_RELOC		0x10

typedef struct _VR_IMPORT
{
	struct _VR_IMPORT* next;

	WORD  ordinal;
	char* name_library;
	char* name_function;
	DWORD rva;

} VR_IMPORT, *PVR_IMPORT;

typedef struct VR_PE_FILE
{
	unsigned char*  MZ_header;
	unsigned int	MZ_header_size;
	IMAGE_FILE_HEADER FileHeader;
	PIMAGE_OPTIONAL_HEADER OptionalHeader;
	WORD machine;
	DWORD total_size;
	DWORD min_RVA;
	DWORD max_RVA;
	
	PVR_SECTION section_list;
	PVR_IMPORT import_list;

	unsigned int flags;

}VR_PE_FILE, *PVR_PE_FILE;


VR_ERROR __stdcall pe_file_readerA(char* full_path,PVR_PE_FILE pe_file);
VR_ERROR __stdcall pe_file_readerW(wchar_t* full_path,PVR_PE_FILE pe_file);

VR_ERROR __stdcall pe_file_closeA(char* full_path,PVR_PE_FILE pe_file);
VR_ERROR __stdcall pe_file_closeW(wchar_t* full_path,PVR_PE_FILE pe_file);

PIMAGE_NT_HEADERS __stdcall get_PE_header(void* buffer);
PIMAGE_SECTION_HEADER __stdcall get_section_header(void* buffer);
DWORD __stdcall RVA_to_FVA(void* buffer, DWORD rva);

//
// utils
//
DWORD __stdcall alignment(DWORD size, DWORD alignment);

#endif




