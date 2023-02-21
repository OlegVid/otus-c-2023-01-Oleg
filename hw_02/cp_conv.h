/*
 * code_pages.h - protoype function for conversion cp1251, KOI8-R, ISO 8859-5 to utf-8
 * author: Oleg Videnin
 * date: 17/02/2023
 */
#pragma once
#include <stdint.h>


typedef uint32_t (*conv_func_t)(uint8_t inp_char);

/*
cp_to_utf8 - conversion function from any 8-bit charset to utf-8
args : fun- selected 8-bit charset, 
       inp_buf - input array of byte for conversion
       inp_buf_len - length of input buffer	
	outp_buffer - bufer for output text 
	out_buf_len - outpu buffer length (max allow memory for output text)
       return value -count of writed bytes, or
       or negative  count of byte if output bytes less than out_buf_len  
        
*/

int64_t cp_to_utf8(conv_func_t cp_f, uint8_t * inp_buf, int64_t inp_buf_len, uint8_t *outp_buffer, int64_t out_buf_len);

uint32_t cp1251chMap(uint8_t cp_1251_code);  // one char conversion for cp1251
uint32_t koi8chMap(uint8_t koi8_code);          // one char conversion for koi8
uint32_t iso8859chMap(uint8_t iso8859_code);// one char conversion for iso8859

