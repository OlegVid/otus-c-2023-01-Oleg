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
#include "./mht_tests/myh_tbl_test.h"

int main(int argc, char **argv) {

    int res = test_myh_tbl_unit();
    if (res != 0) {
        printf("test_myh_tbl_unit fail !!!, code = %d  \n", res);
        return res;
    }  
if (argc > 1){
 printf("%s test successefull\n", argv[0]);
}
 return 0;

}
