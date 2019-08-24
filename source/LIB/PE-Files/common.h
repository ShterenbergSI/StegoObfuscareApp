#include <pe-file.h>
#include <windows.h>

typedef union
{
	char _char[4];
	unsigned int _int;

} VR_SMART_UNION;

VR_ERROR read_section(unsigned char* buffer, PVR_PE_FILE pe_file);
VR_ERROR write_section(HANDLE file,PVR_PE_FILE pe_file);
DWORD add_section(PVR_PE_FILE pe_file);

VR_ERROR read_import(unsigned char* buffer, PVR_PE_FILE pe_file);

