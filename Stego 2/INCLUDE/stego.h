#include <windows.h>
#include <pe-file.h>


int get_information(PVR_PE_FILE pe_file, char* info);
int attach_information(PVR_PE_FILE pe_file, char* info, int number_dw, PVR_SECTION section, unsigned long long* key);
unsigned long long set_stego_key(unsigned char* string);
void build_dictionary_list(char* use_dictionary);




