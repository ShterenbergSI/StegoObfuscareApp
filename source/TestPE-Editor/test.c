#include <distorm.h>
#include <dictionary.h>

#include <windows.h>
#include "VR_error.h"

#include "pe-file.h"
//#include "translator.h"

#define MAX_INSTRUCTIONS 100

#define VR_MAX_SIZE_OPCODE 16


typedef struct VR_DISASM_BODY
{       
	struct VR_DISASM_BODY* next;

	unsigned char size;
	unsigned char body[VR_MAX_SIZE_OPCODE];
	DWORD rva;	

} VR_DISASM_BODY, *PVR_DISASM_BODY;

typedef struct VR_DISASM_FUNC
{
	struct VR_DISASM_FUNC* next;

	unsigned int size_function;
	DWORD rva;
	DWORD end;

	PVR_DISASM_BODY opcode;  
	unsigned char* retn;
//	unsigned int section_size;

}VR_DISASM_FUNC, *PVR_DISASM_FUNC;

PVR_DISASM_FUNC disasm_root=NULL;

void* __stdcall z0_malloc(size_t size) { return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size); }
void __stdcall z0_free(void* ptr) { HeapFree(GetProcessHeap(), 0, ptr); }


BOOL __stdcall find_import_by_name(PVR_PE_FILE pe_file, char* name)
{
	PVR_IMPORT iterator;

	for(iterator =pe_file->import_list; iterator; iterator =iterator->next)
	{
		if(iterator->name_function)
		{
			if(0 == (strcmp(iterator->name_function, name)))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL __stdcall find_imp_by_rva(DWORD rva, PVR_PE_FILE pe_file)
{
	PVR_IMPORT iterator;

	for(iterator =pe_file->import_list; iterator; iterator =iterator->next) { if(iterator->rva == rva) return TRUE; }
	return FALSE;
}




void printf_disasm(void)
{
	PVR_DISASM_FUNC disasm=NULL;
        PVR_DISASM_BODY opcode=NULL;
	int j;

	for(disasm=disasm_root;disasm;disasm=disasm->next)
	{
	//	printf("__________________________________________________ \r\n\r\n");

		for(opcode=disasm->opcode;opcode;opcode=opcode->next)
		{
			printf("%#2x	",opcode->rva);
                	for(j =0; j < opcode->size; j++) printf("%02x ", opcode->body[j]); printf("\r\n"); 
		}
	}	

}

BOOL check_rva_in_disasm(DWORD rva)
{
	PVR_DISASM_FUNC disasm=NULL;
        PVR_DISASM_BODY opcode=NULL;

	for(disasm=disasm_root;disasm;disasm=disasm->next)
	{
		for(opcode=disasm->opcode;opcode;opcode=opcode->next)
		{
			if(rva==opcode->rva) return TRUE;
		}
	}
	
	return FALSE;
}



VR_ERROR get_opcode(_DInst* _di, _CodeInfo* ci, unsigned int instsCount, PVR_PE_FILE pe_file) 
{
	VR_ERROR result =VR_NO_ERROR;
        PVR_DISASM_BODY ptr=NULL;
        uint8_t* code;
	_DInst (*di)[MAX_INSTRUCTIONS] =NULL;


	int j,i;
	
	di =(_DInst (*)[MAX_INSTRUCTIONS])_di;

	code=(uint8_t*)(ci->code);

	for (i = 0; i < instsCount; i++) 
	{                                                                                        
		ptr =(PVR_DISASM_BODY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_DISASM_BODY));
        	if(ptr)
		{
			ptr->rva=(DWORD)ptr_to_rva(pe_file, code);

			if(check_rva_in_disasm(ptr->rva)) 
			{       
				HeapFree(GetProcessHeap(),0,ptr);
				return VR_ERROR_REPEAT_RVA;
			}
		
	       		ptr->size=(*di)[i].size;
			memcpy(ptr->body,code,ptr->size);

			printf("%#2x	",ptr->rva);
     			for(j =0; j < ptr->size; j++) printf("%02x ", ptr->body[j]); printf("\r\n"); 

			while(disasm_root->opcode) disasm_root->opcode =disasm_root->opcode->next;
			disasm_root->opcode=ptr;

                        ci->codeLen -=(*di)[i].size; 

			code+=(*di)[i].size;


		}else result =VR_NO_MEMORY;
	}	
	return result;
}

unsigned int section_size=0;

VR_ERROR  decoder(_CodeInfo* ci, PVR_DISASM_FUNC disasm, PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	_DInst (*di)[MAX_INSTRUCTIONS] =NULL;
	unsigned int instsCount = 0;
	int i;
        uint8_t* code;


	do
	{
              // if((section_size!=ci->codeLen) && (!disasm->next)) break;
                if(!ci->codeLen)break;

		di =(_DInst (*)[MAX_INSTRUCTIONS]) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (sizeof(_DInst) * MAX_INSTRUCTIONS));
        	if(!di)
		{
			result =VR_NO_MEMORY;
			break;
		}
		
       		result = distorm_decompose32(ci, (_DInst*) di, MAX_INSTRUCTIONS, &instsCount);
		if(result==DECRES_SUCCESS) 
		{
		        printf("________________________\r\n");
			printf("%u \r\n\r\n",(*di)[instsCount-1].opcode);


			if(325==(*di)[instsCount-1].opcode) 
			{   
        			result=get_opcode((_DInst*) di, ci,instsCount,pe_file);
				if(disasm->retn) ci->code=disasm->retn;

				disasm =(PVR_DISASM_FUNC)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_DISASM_FUNC));
        			if(!disasm)
				{
					result =VR_NO_MEMORY;
					return result;
				}
                                disasm->next=disasm_root;
                                disasm_root=disasm;

       				result = decoder(ci,disasm,pe_file);   //
			}
			
			if((*di)[instsCount-1].imm.dword)
			{
				if(check_rva_in_disasm((DWORD)ptr_to_rva(pe_file,(DWORD)((uint8_t*)ci->code+ci->nextOffset)) +(*di)[instsCount-1].imm.dword)) 
				{
					result=get_opcode((_DInst*) di, ci,instsCount,pe_file);
					ci->code+=ci->nextOffset; 
			        }
				else 
				{
					disasm =(PVR_DISASM_FUNC)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_DISASM_FUNC));
        				if(!disasm)
					{
						result =VR_NO_MEMORY;
						return result;
					}

                                	disasm->next=disasm_root;
                                	disasm_root=disasm;

					result=get_opcode((_DInst*) di, ci,instsCount,pe_file);
					disasm->retn =((uint8_t*)(ci->code)) +ci->nextOffset;

					ci->code =disasm->retn +(*di)[instsCount-1].imm.dword;
				}

       				result = decoder(ci, disasm,pe_file);
       					
			}
			
  			if(((*di)[instsCount-1].disp) && (462==(*di)[instsCount-1].opcode))
			{
                                if(find_imp_by_rva((((*di)[instsCount-1].disp) -pe_file->OptionalHeader->ImageBase), pe_file)) 
				{
					if(disasm->retn) ci->code=disasm->retn;

                                        if(check_rva_in_disasm((DWORD)ptr_to_rva(pe_file,(ci->code)))) ci->code+=5;

					disasm_root=disasm->next;
					disasm=disasm_root;

                                        ci->codeLen -=(*di)[instsCount-1].size;
       					result = decoder(ci,disasm,pe_file);
				} 

			} 
			if(check_rva_in_disasm((DWORD)ptr_to_rva(pe_file,(DWORD)((uint8_t*)ci->code)))) ci->code+=ci->nextOffset;
			result=get_opcode((_DInst*) di, ci,instsCount,pe_file);

       			ci->code+=ci->nextOffset;

       			result = decoder(ci,disasm,pe_file);


		
		} 

	} while(0);

 //	printf_disasm();

	if(di) HeapFree(GetProcessHeap(),0,di);

	return result;
}

VR_ERROR __stdcall change_instructon(PVR_PE_FILE pe_file)
{
	VR_ERROR result =VR_NO_ERROR;
	PVR_SECTION section;
	_CodeInfo ci;
	PVR_DISASM_FUNC disasm=NULL;

	section =find_section(pe_file);
	if(section)
	{
		section_size=section->data_buffer_size;

		disasm =(PVR_DISASM_FUNC)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VR_DISASM_FUNC));
        	if(!disasm)
		{
			result =VR_NO_MEMORY;
			return result;
		}
		
	
                disasm->next=disasm_root;
                disasm_root=disasm;

		memset(&ci, 0, sizeof(_CodeInfo));

		ci.code =section->data_buffer+pe_file->OptionalHeader->AddressOfEntryPoint - pe_file->OptionalHeader->BaseOfCode;
		ci.codeLen =section->data_buffer_size;
		ci.dt = Decode32Bits;
		ci.features =DF_STOP_ON_FLOW_CONTROL;

		result =decoder(&ci,disasm,pe_file);
		
	}

	return result;
}


void entry(void)
{
	VR_ERROR result;
	int argc;
	LPWSTR* argv;
	VR_PE_FILE pe_file;
	PVR_SECTION  iterator;
	PVR_PE_FILE temp;
	PVR_SYNONYM synonym;
	int position=0;
	
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	switch(argc)
	{
		case 2:
		{
		//	recurseW(argv[1], callbackW, 0);
		} break;

		case 3:
		{
			memset(&pe_file, 0, sizeof(VR_PE_FILE));

			result =pe_file_readerW(argv[1], &pe_file);
			if(VR_NO_ERROR == result)
			{     
				result=change_instructon(&pe_file);

				result =pe_file_closeW(argv[2], &pe_file);
				if(VR_NO_ERROR != result)
				{
					printf("writer error %u\r\n",result);
				}

			} else printf("reader error %s\r\n", result);
			
		} break;

		default:
			printf("usage:  <directory or file to scan>\r\n");
			printf("        <input file> <output file>\r\n");
	}

	ExitProcess(0);
}
