#include <windows.h>
#include <dictionary.h>

#define BUFFER_SIZE_STEP 0x1000

VR_ERROR from_buffer_to_list(unsigned char *buffer)
{
	VR_ERROR result=VR_NO_ERROR;

	PVR_SYNONYM_FILE_HEADER	fheader;

	PVR_SYNONYM_RECORD_HEADER record;
	PVR_SYNONYM_RECORD_BODY src_body;
	PVR_SYNONYM_RECORD_BODY dst_body;

	PVR_SYNONYM synonym;
	PVR_SYNONYM_BODY body;

	int i;
	int j;

	do
	{
                fheader =(PVR_SYNONYM_FILE_HEADER) buffer;

		if(fheader->magic != FILE_MAGIC)
		{	
			result =VR_NOT_VR_FILE;
			break;
		}

		if(!fheader->total_size)
		{
			result=VR_ERROR_INVALID_SIZE;
			break;
		}

                record= (PVR_SYNONYM_RECORD_HEADER) (fheader +1);
		if(record->magic != RECORD_MAGIC)
		{	
			result =VR_NOT_VR_FILE;
			break;
		}

		for(i=0; i< fheader->synonym_number; i++)
		{
			synonym =HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SYNONYM));
        		if(!synonym)
			{
				result =VR_NO_MEMORY;
				break;
			}

			src_body =(PVR_SYNONYM_RECORD_BODY) (record +1);
			synonym->src.size = src_body->size;
			memcpy(synonym->src.body, src_body->body, VR_MAX_SYNONYM_SIZE);

			for(j =1; j < record->src_number; j++)
			{
				body =(PVR_SYNONYM_BODY) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SYNONYM_BODY));
				if(!body)
				{
					result =VR_NO_MEMORY;
					break;
				}

				body->next =synonym->src.next;
				synonym->src.next =body;

				src_body++;
				body->size = src_body->size;
				memcpy(body->body, src_body->body, VR_MAX_SYNONYM_SIZE);
			}

			if(VR_NO_ERROR != result) break;

			dst_body = src_body +1;

			synonym->dst.size = dst_body->size;
			memcpy(synonym->dst.body, dst_body->body, VR_MAX_SYNONYM_SIZE);

			for(j =1; j < record->dst_number; j++)
			{
				body =(PVR_SYNONYM_BODY) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_SYNONYM_BODY));
				if(!body)
				{
					result =VR_NO_MEMORY;
					break;
				}

				body->next =synonym->dst.next;
				synonym->dst.next =body;

				dst_body++;
				body->size = dst_body->size;
				memcpy(body->body, dst_body->body, VR_MAX_SYNONYM_SIZE);
			}

			if(VR_NO_ERROR != result) break;

	                record= (PVR_SYNONYM_RECORD_HEADER) (dst_body +1);

			synonym->next =synonym_root;
			synonym_root =synonym;
		}
		
	  } while(0);

//	  HeapFree(GetProcessHeap(), 0, fheader);

	
	return result;
}

PVR_SYNONYM_FILE_HEADER from_list_to_buffer(void)
{
	unsigned int buffer_size;
	PVR_SYNONYM_FILE_HEADER result;
	PVR_SYNONYM_FILE_HEADER tmp;

	PVR_SYNONYM_RECORD_HEADER record;
	PVR_SYNONYM_RECORD_BODY src_body;
	PVR_SYNONYM_RECORD_BODY dst_body;
	PVR_SYNONYM synonym;
	PVR_SYNONYM_BODY body;
	int size;
	int current_size =0;
	int i;
	int synonym_number =0;

	unsigned char *ptr;

	buffer_size =BUFFER_SIZE_STEP;
	
	result =(PVR_SYNONYM_FILE_HEADER) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffer_size);
	if(result)
	{
		record =(PVR_SYNONYM_RECORD_HEADER) (result +1);

		current_size =sizeof(VR_SYNONYM_FILE_HEADER);

		for(synonym =synonym_root; synonym; synonym =synonym->next)
		{
			src_body = dst_body =(PVR_SYNONYM_RECORD_BODY) (record +1);

			size =sizeof(VR_SYNONYM_RECORD_HEADER);

			for(body = &(synonym->src); body; body =body->next)
			{
				size += sizeof(VR_SYNONYM_RECORD_BODY);
				record->src_number ++;
				dst_body ++;
			}

			for(body = &(synonym->dst); body; body =body->next)
			{
				size += sizeof(VR_SYNONYM_RECORD_BODY);
				record->dst_number ++;
			}

			if((current_size +size) > buffer_size)
			{
                        	buffer_size += BUFFER_SIZE_STEP;
				tmp =result;

				result =HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, tmp, buffer_size);
				if(result != tmp )HeapFree(GetProcessHeap(), 0, tmp);
			}

			current_size += size;

			record->magic =RECORD_MAGIC;
			 //record->flags=

			for(i =0; i < record->src_number; i++)
			{
				src_body->size=synonym->src.size;
				memcpy(src_body->body,synonym->src.body,synonym->src.size);

				src_body ++;
			}

			for(i =0; i < record->dst_number; i++)
			{
				dst_body->size=synonym->dst.size;
				memcpy(dst_body->body,synonym->dst.body,synonym->dst.size);

				dst_body ++;
			}

			synonym_number ++;

			record =(PVR_SYNONYM_RECORD_HEADER) dst_body;
		}

		result->magic =FILE_MAGIC;
		//result-> flags
		result->cpu =IMAGE_FILE_MACHINE_I386;
		result->total_size = current_size;
		result->synonym_number =synonym_number;

	} else printf("malloc failed\r\n");

	return result;
}
