/*
 * zip_lib.h - interface for acces Zip file meta information
 * author: Oleg Videnin
 * date: 03/02/2023
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define STR_BUF_SIZE (500)

#if STR_BUF_SIZE < 50
	#error "buffer size to low"
#endif


typedef enum{ZL_ERR_FILE_NOT_FOUND = -2,ZL_ERR_FILE_NOT_ZIP = -1} zip_result_t;

zip_result_t  zip_lib_file_open(char *filepath);// open and read zip headers

int zip_lib_get_header_count(void); // return count of founded headers, -1 if no one
int zip_get_header_filename(uint8_t header_count, uint16_t buf_max_size, char * buffer);





