//#include <v_common.h>
#include <pe-file.h>

#include <windows.h>


#define RECORD_MAGIC 0x53525140
#define FILE_MAGIC 0x52565520

#define VR_MAX_SYNONYM_SIZE 0x10

typedef struct VR_SYNONYM_BODY
{
	struct VR_SYNONYM_BODY* next;

	unsigned char size;
	unsigned char body[VR_MAX_SYNONYM_SIZE];

} VR_SYNONYM_BODY, *PVR_SYNONYM_BODY;


typedef struct VR_SYNONYM
{
	struct VR_SYNONYM* next;

	unsigned short flags;

	struct VR_SYNONYM_BODY src;
	struct VR_SYNONYM_BODY dst;

} VR_SYNONYM, *PVR_SYNONYM;


VR_ERROR add_synonymA(char* file_name, PVR_SYNONYM synonym);
VR_ERROR add_synonymW(wchar_t* file_name, PVR_SYNONYM synonym);

PVR_SYNONYM find_synonym(unsigned char* buffer,DWORD buffer_size);

#pragma pack(push, 1)

typedef struct
{
	unsigned int magic;
	unsigned short fla;
	unsigned char cpu;
	unsigned char reserved;
	unsigned int total_size;
	unsigned int synonym_number;

} VR_SYNONYM_FILE_HEADER, *PVR_SYNONYM_FILE_HEADER;

typedef struct
{
	unsigned int magic;
	unsigned short flags;
	unsigned char src_number;
	unsigned char dst_number;

} VR_SYNONYM_RECORD_HEADER, *PVR_SYNONYM_RECORD_HEADER;

typedef struct
{
	unsigned char size;
	unsigned char body[VR_MAX_SYNONYM_SIZE];

} VR_SYNONYM_RECORD_BODY, *PVR_SYNONYM_RECORD_BODY;

#pragma pack(pop)

PVR_SYNONYM synonym_root;

VR_ERROR from_buffer_to_list(unsigned char *buffer);
PVR_SYNONYM_FILE_HEADER from_list_to_buffer(void);

VR_ERROR add_synonymA(char* file_name, PVR_SYNONYM synonym);
VR_ERROR add_synonymW(wchar_t* file_name, PVR_SYNONYM synonym);

void load_dictionaryA(void);
void load_dictionaryW(void);

PVR_SECTION find_section(PVR_PE_FILE pe_file);
PVR_SYNONYM find_synonym(unsigned char* buffer, DWORD buffer_size);

void print_synonym(char* comment, PVR_SYNONYM synonym);
