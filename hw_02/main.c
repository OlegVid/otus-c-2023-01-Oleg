/* homework for course c-2023-01 third lesson  
 *author: Oleg Videnin
 *date:   13/02/2023
 * decode cp1251, iso8855, koi8 to utf-8
 * 
*/
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cp_conv.h"


int decode_mode = 0;
static FILE *dec_file = NULL;
conv_func_t ch_map_f = NULL;

static int64_t getFileSize(const char* file_name){
	int64_t _file_size = 0;
	struct stat _fileStatbuff;
	int fd = open(file_name, O_RDONLY);
	if(fd == -1){
		_file_size = -1;
	}
	else{
		if ((fstat(fd, &_fileStatbuff) != 0) || (!S_ISREG(_fileStatbuff.st_mode))) {
			_file_size = -1;
		}
		else{
			_file_size = _fileStatbuff.st_size;
		}
		close(fd);
	}
	return _file_size;
}

static void pri_short_help_msg(void){
	printf("Usage:  hw_02 in_file [-cp1251] [-koi8] [-iso8859] out_file\n");
	printf("if out_file is ommited - out to stdout\n");
}

int main (int argc, char **argv){
uint8_t * inp_buffer;
int64_t   inp_buffer_size;
uint8_t * outp_buffer;
int64_t   outp_buffer_size;
int c;
	if (argc < 3){ 
		pri_short_help_msg();
		return 0;
	}

	if(!strcmp(argv[2], "-cp1251")) {
		 decode_mode = 0;
		 ch_map_f = cp1251chMap;
	 } 

	if(!strcmp(argv[2], "-koi8")) {
		 decode_mode = 1;
                 ch_map_f = koi8chMap;
	 } 

	if(!strcmp(argv[2], "-iso8859")) {
		 decode_mode = 2;
                 ch_map_f = iso8859chMap;
	 } 
	if (ch_map_f == NULL){
		pri_short_help_msg();
		return 0;
	}
inp_buffer_size = getFileSize(argv[1]);
inp_buffer = malloc(inp_buffer_size);
outp_buffer_size = 4*inp_buffer_size+1;
outp_buffer= malloc(outp_buffer_size);

if ((inp_buffer == NULL) |((outp_buffer == NULL))){
	printf("error memory allocation for file %s \n",argv[1]);
	return -1;
}

dec_file = fopen(argv[1], "rb");
int i = 0;
while ((c = getc(dec_file)) != EOF){
inp_buffer[i] = (uint8_t)c;
i++;
}
fclose(dec_file);

int64_t len = cp_to_utf8(ch_map_f,inp_buffer,inp_buffer_size,outp_buffer, outp_buffer_size);
if (len <= 0){
 printf("Error during conversion operation\n");
 return -1;
}
outp_buffer[len] = 0;
if ((argc > 3) && (argv[3] != NULL)){
	FILE *out = fopen(argv[3], "w" );
	fprintf(out, "%s",outp_buffer);
	fclose(out);
} else{
	printf("%s\n",outp_buffer); 
  }

if (inp_buffer != NULL) free(inp_buffer);
if (outp_buffer != NULL) free(outp_buffer);
return 0;
}

