#include <dictionary.h>

PVR_SECTION find_section(PVR_PE_FILE pe_file)
{
	VR_ERROR error=VR_NO_ERROR;
	int i;
	PVR_SECTION iterator;

	for(iterator=pe_file->section_list; iterator; iterator =iterator->next)
	{
		if(IMAGE_SCN_CNT_CODE & iterator->Characteristics)
		{
	  		//if(strcmp(iterator->Name, ".text")) break;
			return iterator;
		}
	}

	return iterator;
}

PVR_SYNONYM find_synonym(unsigned char* buffer, DWORD buffer_size)
{
	PVR_SYNONYM result =NULL;

	for(result =synonym_root; result; result =result->next)
	{
		if(buffer_size >= result->src.size)
		{
			if(!memcmp((buffer), result->src.body, result->src.size))
			{
				return result;
			}
		}
	}
		
	return result;
}
