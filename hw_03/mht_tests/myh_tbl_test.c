#include "../mht_src/myh_tbl.h"
#include "myh_tbl_test.h"
#include <stdio.h>

int test_myh_tbl_unit(void) {
    char varName[200];
    int * test_val = 0;
    mht_storage_t *mht_storage = myh_table_init(START_TABLE_CAPACITY);
    if (mht_storage == NULL) return -1;
    mht_result_t ret_code;
    for (size_t i = 0; i < TEST_ITEM_COUNT; i++) {
        sprintf(varName, "MY_varible_test_name_parametr_#%zu", i);
        if ((ret_code = myh_table_insert(mht_storage, varName, i * 23)) != MHT_OK) {
            printf("Line No %d Insert error item No = %zu, ret_code = %d \n",__LINE__, i, ret_code);
            return -(i + 1);
        }
#if INJECT_ERROR == 1
        if (i == INJECT_ERROR_ITEM_NO){
            myh_table_lookup(mht_storage,varName,&test_val);
            printf("1233 var name = %s, value = %d\n",varName, test_val);
            myh_table_dbg_msg(1);
            myh_table_insert(mht_storage,varName, 100);
            myh_table_dbg_msg(0);
            myh_table_lookup(mht_storage,varName,&test_val);
            printf("re 1233 var name = %s, value = %d\n",varName, test_val);
        }
#endif
    }

    for (size_t i = 0; i < TEST_ITEM_COUNT; i++) {
        sprintf(varName, "MY_varible_test_name_parametr_#%zu", i);
        if ((ret_code = myh_table_lookup(mht_storage, varName, &test_val)) != MHT_OK) {
            printf("Line No %d Extract error item No = %zu varName = %s return code = %d \n",__LINE__, i, varName, ret_code);
            return (i + 1);
        }
        myh_table_insert(mht_storage, varName, (*test_val) + 200);  // modification test
        if (myh_table_get_count(mht_storage) != TEST_ITEM_COUNT) {
            printf("Line No %d Test stage modification, Item: %zu,varName = %s,Expected Count %d, Count: %zu \n",__LINE__, i, varName,
                   TEST_ITEM_COUNT, myh_table_get_count(mht_storage));
            //	return - 10l;
        }
        myh_table_lookup(mht_storage, varName, &test_val);
        if ((*test_val) != (200 + (int)i * 23)) {
            printf("Line No %d  Extract value error item No = %zu value = %d \n",__LINE__, i, *test_val);
            return (i + 1);
        }
    }
    myh_table_free(mht_storage);
    return 0;
}
