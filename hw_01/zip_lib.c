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
        const uint8_t *name;
        const uint8_t *extra;
}lfh_t;

#pragma pack(pop)

static uint8_t  sig_file_header[4] = {0x50,0x4b,0x01,0x02};
static FILE *zip_file = NULL;
static char** list_of_file = NULL;
static uint16_t count_of_file = 0;
static char* error_msg = "Filename exceed buffer length";

/*
add file name - to string list? for storage and further use
*/
void add_new_item(char *filename){
	count_of_file++;
	list_of_file = (char**) realloc(list_of_file, count_of_file*sizeof(char*));
	list_of_file[count_of_file-1] = (char*) malloc(strlen(filename) + 1);
	strcpy(list_of_file[count_of_file-1], filename);
}

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
zip_result_t  zip_lib_file_open(char *filepath){
	cfh_t cfh;
	char buffer[STR_BUF_SIZE];
	int c;
	count_of_file = 0;

	zip_file = fopen(filepath, "rb");
	if (NULL == zip_file){
	 return ZL_ERR_FILE_NOT_FOUND;
	}

	while ((c = getc(zip_file)) != EOF){
	   if (sig_is_found(c,sig_file_header)){  // signature was found
				if (fread(&cfh, sizeof(cfh_t), 1, zip_file) > 0){
				if (cfh.name_len >=STR_BUF_SIZE){ 
					strcpy(buffer,error_msg);
	                                add_new_item(buffer);
				}
			 	else{ 
					if ( fread(buffer, cfh.name_len, 1, zip_file) > 0){ 
						buffer[cfh.name_len] = '\0';
						add_new_item(buffer);
		}
				}
				
			}
	   }
	}
	fclose(zip_file);
	return (count_of_file > 0) ? count_of_file :  ZL_ERR_FILE_NOT_ZIP;
}

int zip_lib_get_header_count(void){
	return count_of_file;
}

/*
compare func for char** from internet 
*/
int cmp(const void* s1, const void* s2)
{
    const char** a = (const char**) s1;
    const char** b = (const char**) s2;
    return strcmp(*a, *b);
}

int zip_get_header_filename(uint8_t header_count, uint16_t buf_max_size, char * buffer){
        qsort(list_of_file,count_of_file,sizeof(char *),cmp); // sorting filename list
	if (strlen(list_of_file[header_count]) >= buf_max_size){
		return -1;                             //control if buffer les than filename
	}
	strcpy(buffer, list_of_file[header_count]);
	return 0;
}


