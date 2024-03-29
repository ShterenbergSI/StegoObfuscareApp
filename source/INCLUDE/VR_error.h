#ifndef __VR_ERROR_H
#define __VR_ERROR_H

#define VR_WINDOWS_ERROR 0x80000000

typedef enum
{
	VR_NO_ERROR =0,

	VR_FILE_TOO_SMALL,
	VR_FILE_TOO_LARGE,

	VR_PARTIAL_READ,
	VR_PARTIAL_WRITE,

	VR_CPU_NOT_SUPPORTED,
	VR_NOT_MZ_FILE,
	VR_MSDOS_EXECUTABLE,
	VR_OS2_16BIT_EXECUTABLE,
	VR_OS2_32BIT_EXECUTABLE,

	VR_UNKNOWN_SIGNATURE,

	VR_EMPTY_SECTION_LIST,
	VR_SYMBOLS_NOT_SUPPORTED,
	VR_INVALID_OPTIONAL_HEADER_SIZE,

	VR_NO_MEMORY,
	VR_NOT_MEMORY_STRUCTURE,

	VR_NO_CHANGE_EP,
	VR_NULL_PTR,

	VR_ERROR_CHECKSUM,
	VR_ERROR_SECTION_TABLE_FULL,

	VR_PE_FILE_NOT_EXECUTABLE,

	VR_ERROR_DISASM,
	VR_ERROR_INVALID_TYPE,
	VR_ERROR_INVALID_SIZE,

	VR_RELOC,

	VR_ALREADY_EXISTS,
	VR_NOT_VR_FILE,
	VR_ERROR_DUPLICATE_SYNONYM,

	VR_PACKED_FILE,

	VR_EXIT_FIND,
	VR_ERROR_REPEAT_RVA,

	VR_ERROR_LARGE_MSG,

	VR_ERROR_NOT_WATERMARK,

	VR_ERROR_NOT_ACCESS,
	
	VR_FILE_END,

	VR_NULL_FILE,

	VR_ERROR_LAST

} VR_ERROR, *PVR_ERROR;

#endif

