/* homework for course c-2023-01 lesson No 8
 *author: Oleg Videnin
 *date:  02/04/2023
 *Чтение файла из аргумента вызова и вывод списка слов в формате: слово = количество_встреч_в_тексте
 *
*/
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "myh_tbl.h"
#include "myh_tbl_test.h"

#define PERFORM_UNIT_TEST 1

void pri_short_help_msg(char *name) {
    printf("usage: %s filename\n", name);
}

int main(int argc, char **argv) {
#if PERFORM_UNIT_TEST == 1
    int res = test_myh_tbl_unit();
    if (res != 0) {
        printf("test_myh_tbl_unit fail !!!, code = %d  \n", res);
        return res;
    }  
#endif
    mht_storage_t *mht_storage = myh_table_init(10000);
    if (mht_storage == NULL) {
        printf("Could not allocate memory\n");
        return -2;
    }
    if (argc != 2) {
        pri_short_help_msg(argv[0]);
        return 0;
    }

    FILE *inp_file = fopen(argv[1], "r");
    if (inp_file == NULL) {
        printf("Could not open file %s\n", argv[1]);
        return -1;
    }
    char *word = (char *)calloc(1, sizeof(char));
    int ch;
    int value;
    size_t char_count = 0;
    while ((ch = fgetc(inp_file)) != EOF) {
        //isalpha - не работает с utf-8 - поэтому три функции заменители
        if (isspace(ch) || ispunct(ch) || isdigit(ch)) {
            if (char_count > 0) {
                word[char_count] = '\0';
                myh_table_lookup(mht_storage, word, &value);
                value++;
                myh_table_insert(mht_storage, word, value);
                char_count = 0;
            }
        } else {
            word = (char *) realloc(word, (char_count + 1) * sizeof(char));
            word[char_count] = (char) tolower(ch);
            char_count++;
        }
    }
    fclose(inp_file);

    char testKey[1000];
	int test_value;
	//printf("count = %zu \n",myh_table_get_count(mht_storage));
    for (size_t i = 0; i < myh_table_get_count(mht_storage); i++) {
        if (myh_table_index(mht_storage, i, testKey, &test_value) == MHT_OK) {
		//printf("%s %d\n",testKey,test_value);
		printf("%-30s %-10d\n", testKey, test_value);
       	} 
    }
  free(word);
  myh_table_free(mht_storage);
  return 0;
}

