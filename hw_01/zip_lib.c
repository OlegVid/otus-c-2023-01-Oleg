#include "zip_lib.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
   
/*
  struct for central header
*/

#pragma pack(push,1)

typedef struct {
        uint16_t made_by_ver;    /* Version made by. */
        uint16_t extract_ver;    /* Version needed to extract. */
        uint16_t gp_flag;        /* General purpose bit flag. */
        uint16_t method;         /* Compression method. */
        uint16_t mod_time;       /* Modification time. */
        uint16_t mod_date;       /* Modification date. */
        uint32_t crc32;          /* CRC-32 checksum. */
        uint32_t comp_size;      /* Compressed size. */
        uint32_t uncomp_size;    /* Uncompressed size. */
        uint16_t name_len;       /* Filename length. */
        uint16_t extra_len;      /* Extra data length. */
        uint16_t comment_len;    /* Comment length. */
        uint16_t disk_nbr_start; /* Disk nbr. where file begins. */
        uint16_t int_attrs;      /* Internal file attributes. */
        uint32_t ext_attrs;      /* External file attributes. */
        uint32_t lfh_offset;     /* Local File Header offset. */
}cfh_t;


/*
  struct for local header 
*/

typedef struct  {
        uint16_t extract_ver;
        uint16_t gp_flag;
        uint16_t method;
        uint16_t mod_time;
        uint16_t mod_date;
        uint32_t crc32;
        uint32_t comp_size;
        uint32_t uncomp_size;
        uint16_t name_len;
        uint16_t extra_len;
}lfh_t;

#pragma pack(pop)

static uint8_t  sig_file_header[4] = {0x50,0x4b,0x03,0x04};
static FILE *zip_file = NULL;
static char* error_msg = "Filename exceed buffer length";

/*
add file name - to string list? for storage and further use
*/


unsigned char sig_is_found(uint8_t inp, uint8_t* const signature){
static int fsm = -1;
	switch (fsm){
		default:
			fsm = (inp == signature[0]) ? 0: fsm;  // reset and start fsm
		break;

		case 0:
			fsm = (inp == signature[1]) ? 1: -1;
		break;

		case 1:
			fsm = (inp == signature[2]) ? 2: -1	;
		break;

		case 2:
			fsm = (inp == signature[3]) ? 3: -1;
		break;

		case 3:	
			fsm = -1;
		break;
	}
 return (fsm == 3);
}

// open and read zip headers
zip_result_t  zip_lib_file_open_and_list(char *filepath){
	lfh_t lfh;
	char buffer[STR_BUF_SIZE];
	int c;
	int count_of_file = 0;
        uint64_t count_of_byte = 0;

	zip_file = fopen(filepath, "rb");
	if (NULL == zip_file){
	 return ZL_ERR_FILE_NOT_FOUND;
	}

	while ((c = getc(zip_file)) != EOF){
		count_of_byte++;
	   if (sig_is_found(c,sig_file_header)){  // signature was found
				count_of_file++;

                 if (count_of_byte <= sizeof(sig_file_header)){
			return ZL_ERR_FILE_NOT_JOINED;   //check if PK signature at begin of file then file is pure zip (not joined) 
		}
				if (fread(&lfh, sizeof(lfh_t), 1, zip_file) > 0){
				if (lfh.name_len >=STR_BUF_SIZE){ 
					strcpy(buffer,error_msg);
	                                printf("%s \n",buffer);
				}
			 	else{ 
					if (fread(buffer, lfh.name_len, 1, zip_file) > 0){ 
						buffer[lfh.name_len] = '\0';
						printf("%s \n",buffer);
					}
				}
				
			}
	   }
	}
	fclose(zip_file);
	return (count_of_file > 0) ? count_of_file :  ZL_ERR_FILE_NOT_ZIP;
}


