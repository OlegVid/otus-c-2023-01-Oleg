/* homework for course c-2023-01 third lesson  
 *author: Oleg Videnin
 *date:   13/02/2023
 * zipjpg read file list implementation
 * 
*/

#include "zip_lib.h"


int main (int argc, char **argv){
char buffer[500];

	if (argc < 2){ 
		printf("No input file in args \n");
		return 0;
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
	 	printf("File # %d, name = %s \n",i,buffer);
		} else {
		  printf("File # %d, name = %s \n",i,"Filename exceed bufer size\n");
		}
	 }

	 printf("%d files was found  \n", zip_lib_get_header_count());
	 return 0;
}

