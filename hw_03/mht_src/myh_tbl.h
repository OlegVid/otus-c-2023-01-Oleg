#pragma once

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MHT_REHASH_ERROR = -5,
    MHT_RES_HASH_INCORRECT = -4,
    MHT_RES_ERROR_ALLOC = -3,
    MHT_RES_ERR_MEMORY_OUT = -2,
    MHT_RES_KEY_NOT_FOUND = -1,
    MHT_OK = 0
} mht_result_t;


typedef struct mht_storage_t mht_storage_t;

mht_storage_t *myh_table_init(size_t init_capacity); // инициализация таблицы
size_t myh_table_get_count(mht_storage_t *mht);
size_t myh_table_get_capacity(mht_storage_t *mht);
mht_result_t myh_table_insert(mht_storage_t *mht, char *key,int value); // добавление значения или его модификация, если такой ключ уже есть
mht_result_t myh_table_lookup(mht_storage_t *mht, char *key, int *value); // получение значение по ключу
mht_result_t myh_table_index(mht_storage_t *mht, size_t index,char *key, int *value); // получение значение по индексу
void myh_table_free(mht_storage_t *mht);


///
void myh_table_dbg_msg(bool enable_dbg);



 
