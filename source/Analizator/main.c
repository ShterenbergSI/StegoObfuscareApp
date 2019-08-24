#define UNICODE

#include <windows.h>


int sum=0;
DWORD MaxFreeSpaceOnDisk = 0L;
DWORD MaxFreeSpaceInMem=0L;


/*BOOL read_date_section(DWORD section->VirtualAddress, DWORD section->PointerToRawData,DWORD nt->OptionalHeader.ImageBase)
{

}
*/

PIMAGE_NT_HEADERS get_PE_header(unsigned char* buffer)
{
	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS nt;
        WORD NumberOfSections;

	dos_header =(PIMAGE_DOS_HEADER) buffer;

	if (dos_header->e_magic == IMAGE_DOS_SIGNATURE)
    	{   	
	//	printf("This file is PE-file!\r\n");
		printf("%x", dos_header->e_magic);


		nt =(PIMAGE_NT_HEADERS)(buffer + dos_header->e_lfanew);		
		
		if(nt->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
		{
		      //  printf("Architecture type = x86\r\n");

        		if(nt->Signature == IMAGE_NT_SIGNATURE)
			{
			//	printf("PE header correct!\r\n");
			//	NumberOfSections=nt->FileHeader.NumberOfSections;
			//	printf("Number of sections: %u\r\n",NumberOfSections);
				sum++;

                               	return nt;
			}
		}	

	} 

	return NULL;	
}



void print_section(PIMAGE_SECTION_HEADER section, PIMAGE_NT_HEADERS nt)
{
	BYTE name[IMAGE_SIZEOF_SHORT_NAME +1];
	DWORD FreeSpaceOnDisk=0;
        DWORD FreeSpaceInMem=0;

	memset(name, 0, IMAGE_SIZEOF_SHORT_NAME +1);
	memcpy(name, section->Name, IMAGE_SIZEOF_SHORT_NAME);

	FreeSpaceOnDisk = (section->Misc.VirtualSize % nt->OptionalHeader.FileAlignment);
	if(FreeSpaceOnDisk > MaxFreeSpaceOnDisk)
	{	
		MaxFreeSpaceOnDisk = FreeSpaceOnDisk;	
	}

	FreeSpaceInMem = (section->Misc.VirtualSize % nt->OptionalHeader.SectionAlignment);
 	if(FreeSpaceInMem > MaxFreeSpaceInMem)
	{	
		MaxFreeSpaceInMem = FreeSpaceInMem;	
	}
        printf("%s SizeOnDisk =%#x; FreeSpaceOnDisk =%#x; SizeInMem =%#x; FreeSpaceInMem =%#x;\r\n",
		name,
		section->SizeOfRawData,
                FreeSpaceOnDisk,
		section->Misc.VirtualSize,
		FreeSpaceInMem);


}

void open_file(wchar_t *name_file)
{
	HANDLE file;
	unsigned long size_file;
	DWORD size_low;
	DWORD size_high;
	unsigned char* buffer;
	DWORD bytes;
	HANDLE hMapping;
	HANDLE hMap;
	int i;
	PIMAGE_NT_HEADERS nt;
	PIMAGE_SECTION_HEADER section;
	unsigned char* ptr;

	file=CreateFile(name_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == file)
	{
		printf("Error in open_file, %u",GetLastError());
		return;
	}

	size_low=GetFileSize(file, &size_high);
	if(0==size_high)	
	{
		buffer=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size_low);
		if( NULL==buffer)
		{	 printf("Error!Inpossible to share %u bytes\r\n", size_low);
		} else
		{ 
			if(!ReadFile(file,buffer,size_low,&bytes,NULL))
			{      
				 printf("Error! File not read!\r\n");
			}
			if(bytes==size_low)
			{
	                        nt = get_PE_header(buffer);
	                        if(nt)
				{
					printf("\"%S\"\r\n", name_file);

					ptr =(unsigned char*) nt;
					ptr += (sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + nt->FileHeader.SizeOfOptionalHeader);
					section =(PIMAGE_SECTION_HEADER) ptr;

					for(i =0; i < nt->FileHeader.NumberOfSections; i++)
					{
						print_section(section, nt);
						section++;
					}
					
                                        
					printf("\r\n");

				} 
			}

		}	
			HeapFree(GetProcessHeap(),0,buffer );
	}	

	CloseHandle(file);
}

DWORD dir_calc(wchar_t *path_original)
{       
	WIN32_FIND_DATA Data;
	wchar_t path[MAX_PATH];
	HANDLE hFind;
	DWORD Type;
	DWORD error =ERROR_SUCCESS;

	
        wcscpy(path, path_original);
	wcscat(path,L"\\*");

        hFind= FindFirstFile(path, &Data);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if(MAX_PATH <= (wcslen(path_original) + wcslen(L"\\\\") + wcslen(Data.cFileName)))
			{
				printf("Path overflow! \"%S\" + \"%S\" + \"%S\"\r\n", path_original, L"\\\\", Data.cFileName);
			} else
			{
				if((wcscmp(Data. cFileName, L".")) && (wcscmp(Data.cFileName, L"..")))
				{
					wcscpy(path, path_original);
					wcscat(path, L"\\");
					wcscat(path, Data.cFileName);

					if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						error = dir_calc(path);
						if(ERROR_SUCCESS != error)
						{
						//	printf("Error %u analyzing directory \"%S\"\r\n", error, path);
						}
					} else
					{
						if(GetBinaryType(path,&Type))
						{
							switch(Type)
							{
								case SCS_32BIT_BINARY:
								{
									open_file(path);
								}break;	
				                        }
						}
	                       		}
				}
			}
		
			if(FALSE == FindNextFile(hFind,&Data))
			{
				break;
			}

		} while(TRUE);
	
		FindClose(hFind);
	  
	} else error =GetLastError();

	return error;
}


DWORD comp_calc(void)
{
	DWORD error =ERROR_SUCCESS;

	wchar_t drive[3];

	drive[2]=0;
	drive[1]=L':';

	for(drive[0]=L'A';drive[0]<=L'Z';drive[0]++)
	{
		if(DRIVE_FIXED == GetDriveType(drive))
		{
			error =dir_calc(drive);

			if(ERROR_SUCCESS != error)
			{
				printf("Error %u in dir_calc(\"%S\")\r\n", error, drive);
			}
		}
	}

	return error;
}

void entry(void)
{
	comp_calc();
	printf("Total files %u; Max Free Space In Mem = %#x, Max Free Space On Disk = %#x\r\n",sum,MaxFreeSpaceInMem,MaxFreeSpaceOnDisk);

	ExitProcess(0);
}
