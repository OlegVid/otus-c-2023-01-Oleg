/* homework for course c-2023-01 third lesson  
 *author: Oleg Videnin
 *date:   13/02/2023
 * zipjpg read file list implementation
 * 
*/

#include "zip_lib.h"
#include <string.h>

int disp_mode = 0;
void pri_short_help_msg(void){
printf("Usage:  hw_01 file [-l] \n");
}

int main (int argc, char **argv){
char buffer[500];

	if (argc < 2){ 
		pri_short_help_msg();
		return 0;
	}

	if (argc > 2){
	     if(!strcmp(argv[2], "-l")) {
		 disp_mode = 1;
	     } 
	}

	switch(zip_lib_file_open(argv[1])){
	 case ZL_ERR_FILE_NOT_FOUND:
	 	printf("Can not open file \n");
		return 0;
	 break;

	 case ZL_ERR_FILE_NOT_ZIP:
	 	printf("the file does not contain an arj \n");
		return 0;
	 break;
	}

	for(int i = 0;i < zip_lib_get_header_count(); i++){ 
		if (zip_get_header_filename(i,sizeof(buffer), buffer) >= 0){
		switch(disp_mode){		
			default:
			case 0:
				printf("%s\n",buffer);
			break;
			case 1:
				printf("File # %d, name = %s \n",i,buffer);
			break;
		}
	
		} else {
			switch(disp_mode){
				default:
				case 0:
					printf("%s\n","Filename exceed bufer size\n");
				break;	
				case 1:
      					printf("File # %d, name = %s \n",i,"Filename exceed bufer size\n");
				break;
			}
		}
	 }

	 printf("%d files was found  \n", zip_lib_get_header_count());
	 return 0;
}

