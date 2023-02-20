/* homework for course c-2023-01 third lesson  
 *author: Oleg Videnin
 *date:   13/02/2023
 * zipjpg read file list implementation
 * 
*/

#include "zip_lib.h"
#include <string.h>

void pri_short_help_msg(void){
printf("Usage:  hw_01 file \n");
}

int main (int argc, char **argv){

	if (argc < 2){ 
		pri_short_help_msg();
		return 0;
	}


	switch(zip_lib_file_open_and_list(argv[1])){
	 case ZL_ERR_FILE_NOT_FOUND:
	 	printf("Can not open file \n");
		return 0;
	 break;

	 case ZL_ERR_FILE_NOT_ZIP:
	 	printf("the file does not contain an arj \n");
		return 0;
	 break;
	 case ZL_ERR_FILE_NOT_JOINED:
	 	printf("the file is original .zip file, not joined \n");
         break;
	}

	 return 0;
}

