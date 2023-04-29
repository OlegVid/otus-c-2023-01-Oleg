#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myh_tbl_settings.h"
#include "myh_tbl.h"

typedef enum {
    NO_INDEX_FOUND, EMPTY_INDEX_FOUND, EXISTING_INDEX_FOUND
} locate_t; // тип возвращаемого значения при поиске свободных индексов
typedef enum {
    VAULT_ONE, VAULT_TWO
} current_vault_t; // тип для задания "текущего хранилища"
typedef enum {
    MHT_ITEM_EMPTY, MHT_ITEM_BUSY = 101, MHT_ITEM_DELETED = 102
} mht_state_t; // тип отображения статуса хранилища. пустой, занятый и удаленный (функция удаления пока не реализованна)

typedef uint32_t my_hash_t; // псевдоним типа для хранения значения хэша

// Элемент одного значения их хэш таблицы. Содержит ключ,значение и статус
typedef struct mht_item {
    char *key;
    int value;
    mht_state_t state;
} mht_item_t;

// Структура "под-хранилища". Содержит указатель на таблицу, счетчик заполнености, значение выделенной памяти и порог по достижении которого нужно перехэшировать таблицу
typedef struct {
    mht_item_t *table;
    my_hash_t *index_table;
    size_t count;
    size_t allocated_count;
    size_t rehash_treshold;
} mht_t;

// Структура "хранилища" который состоит из двух подхранилищ, которые переключаются, когда происходит перехэширование таблицы (по достижению порога)
struct mht_storage_t {
    mht_t vault_one;
    mht_t vault_two;
    current_vault_t current_vault;
};


int PRINT_DBG = 0;// печать отладочной информации (регулируется внешней функцией myh_table_dbg_msg

/**
* function myh_table_dbg_msg
* brief Управляет включение выдачи отладочной информации через printf
* param bool enable_dbg  1 - включить выдачу, 0 - отключить
* retval void
*/
void myh_table_dbg_msg(bool enable_dbg) {
    PRINT_DBG = enable_dbg;
}

/**
* function hash_func
* brief хеш-функция для символьной строки, Возвращает значение в диапазоне от 0 до rage
* param char *key - строка, int range - максимальное значение ХЭШ числа
* retval my_hash_t -вычисленное хэш значение
*/
my_hash_t hash_func(char *key, size_t range) {
    my_hash_t hashf = 0;
    size_t len = strlen(key);
    if (len == 1) {
        hashf = (uint8_t)key[0];
    } else {
        for (size_t i = 0; i < len; i++) {
            hashf = (hashf >> 4) + key[i];  }
    }
    return hashf % range;
}

/**
* function CurrVault
* brief возвращает текущее используемое подхранилище
* param mht_storage - хранилище, с которым и работаем
* retval возвращает текущее используеое подхранилище хэш таблицы
* comment  с текущим подхранилищем ним работают функции, при рехешифоровании даные с него копируются во вторичное подхранилище, а затем меняются роли хранилищ.
*/
static mht_t * CurrVault(mht_storage_t *mht_storage) {
    return mht_storage->current_vault == VAULT_ONE ? &mht_storage->vault_one : &mht_storage->vault_two;
}

/**
* function SecVault
* brief возвращает вторичное подхранилище
* param mht_storage - хранилище, с которым и работаем
* retval возвращает вторичное, неиспользуеое подхранилище хэш таблиц
* comment  вторичное подхранилище получает данные основного при рехешировании, а затем становится основным через функцию swap_CV
*/
static mht_t *SecVault(mht_storage_t *mht_storage) {
    return mht_storage->current_vault == VAULT_ONE ? &mht_storage->vault_two : &mht_storage->vault_one;
}

/**
* function  swap_CV
* brief меняет ролями основное и вторичное подхранилище
* param mht_storage - хранилище, с которым и работаем
* retval void
* comment используется в функции rehashTable
*/
static void swap_CV(mht_storage_t *mht_storage) {
    if (mht_storage->current_vault == VAULT_ONE) {
        mht_storage->current_vault = VAULT_TWO;
    } else {
        mht_storage->current_vault = VAULT_ONE;
    }
}
void mhtFree(mht_t *mht) {
    if (mht == NULL) return;
    if (mht->table == NULL) return;
    for (size_t i = 0; i < mht->allocated_count; i++) {
        if (MHT_ITEM_BUSY == mht->table[i].state) {
            free(mht->table[i].key);
        }
    }
    free(mht->table);
}
/**
* function myhTableInit
* brief функция инициализации подхранилща
* param mht -  указатель на подхранилище
* retval mht_result_t - результат операции, 0 (MHT_OK) - успех или MHT_RES_ERROR_ALLOC, если не удалось выделить память под таблицу.
*/
static mht_result_t myhTableInit(mht_t *mht, size_t count) {
    if (PRINT_DBG) printf("init size : %zu \n", count);
    //  free(mht->table);
    mhtFree(mht); // очищаем таблицу, если в ней что было
    mht->table = calloc(count, sizeof(mht_item_t));
    mht->index_table = calloc(count,sizeof(my_hash_t));
    if (mht->table == NULL) {
        if (PRINT_DBG) printf("MHT_RES_ERROR_ALLOC");
        return MHT_RES_ERROR_ALLOC;
    }
    mht->count = 0;
    mht->allocated_count = count;
    mht->rehash_treshold = (MAX_FULFILEMNT_PERCENTAGE * mht->allocated_count) /
                           100; // задается порог, после которого происходит рехешироваие
    return MHT_OK;
}

/**
* function myh_table_init
* brief внешняя функция инициализации объекта - хранилища хэш таблицы
* param init_capacity - начальная емкость
* retval mht_storage_t * - указатель на размещенный в памяти объект хранилище хэш таблицы.
*/
mht_storage_t *myh_table_init(size_t init_capacity) {
    mht_storage_t *mht = (mht_storage_t *) calloc(1,sizeof(mht_storage_t));
    if (mht == NULL) return NULL;
    mht->current_vault = VAULT_ONE;
    if (myhTableInit(&mht->vault_one, init_capacity) == MHT_OK) {
        return mht;
    } else return NULL;
}

/**
* function mht_set_value
* brief внутренияя ф-я задание значения, выделение памяти под ключ и копирование ключа в элемент таблицы
* param mht  - таблицу с которой работаем, hash_index - индекс модифицируемого элемента в таблице, *key - ключ, value - значение
* retval mht_result_t - статус выполнения функции, MHT_OK если все удалось.
*/
mht_result_t mht_set_value(mht_t *mht, my_hash_t hash_index, char *key, int value, bool is_new) {
    if (hash_index >= mht->allocated_count){
        return MHT_RES_HASH_INCORRECT;
    }
    free(mht->table[hash_index].key);
    mht->table[hash_index].key = calloc(strlen(key) + 1,sizeof(char) );
    if (mht->table[hash_index].key == NULL) return MHT_RES_ERR_MEMORY_OUT;
    strcpy(mht->table[hash_index].key, key);
    mht->table[hash_index].value = value;
    mht->table[hash_index].state = MHT_ITEM_BUSY;
    if (is_new) {
        mht->index_table[mht->count] = hash_index;
        mht->count++;
    }
    return MHT_OK;
}

/**
* function locateEmptyIndex
* brief находит ближайший после hash_index пустой или совпадающий по key индекс массива хэш таблицы.
* param mht - подхранилище, где ищем значение, key - строка искомого ключа, hash_index - начальное значение индекса с которого начинаем поиск, found_index - 
* указатель куда помещаем найденный индекс.
* retval 1 - индекс найден, 0 - индекс не найден
* comment Сначала ищет от hash_index до конца таблицы, если ничего подходящего не найдено то с начала таблицы и до  hash_index. 
*/
static locate_t locateEmptyIndex(mht_t *mht, char *key, my_hash_t hash_index, my_hash_t *found_index) {
    for (my_hash_t i = hash_index; i < mht->allocated_count - 1; i++) {
        if ((mht->table[i].state == MHT_ITEM_EMPTY) || (mht->table[i].key == NULL)) {
            *found_index = i;
            return EMPTY_INDEX_FOUND;
        } else {
            if (strcmp(mht->table[i].key, key) == 0) {
                *found_index = i;
                return EXISTING_INDEX_FOUND;
            }
        }
    }
    for (my_hash_t i = 0; i < hash_index; i++) {
        if ((mht->table[i].state == MHT_ITEM_EMPTY) || (mht->table[i].key == NULL)) {
            *found_index = i;
            return EMPTY_INDEX_FOUND;
        } else {
            if (strcmp(mht->table[i].key, key) == 0) {
                *found_index = i;
                return EXISTING_INDEX_FOUND;
            }
        }
    }
    return NO_INDEX_FOUND;
}

/**
* function myhTableInsert
* brief внутренияя ф-я добавление элемента в таблицу подхранилища, в случае если элемент уже есть - то модификация его значения.
* param mht - таблицу с которой работаем, *key - ключ, value - значение
* retval mht_result_t - статус выполнения функции, MHT_OK если все удалось.
* comment
*/
mht_result_t myhTableInsert(mht_t *mht, char *key, int value) {
    my_hash_t hash_index = hash_func(key, mht->allocated_count);
    if (PRINT_DBG) printf("HASH key = %s, hash = %d\n", key, hash_index);
    if (hash_index > mht->allocated_count) return MHT_RES_HASH_INCORRECT;
    switch (mht->table[hash_index].state) {
        default:
        case MHT_ITEM_DELETED:
        case MHT_ITEM_EMPTY:
            return mht_set_value(mht, hash_index, key, value, true);
            break;
        case MHT_ITEM_BUSY:
            if (strcmp(mht->table[hash_index].key, key) == 0) {
                return mht_set_value(mht, hash_index, key, value, false);
            }
            if (PRINT_DBG) printf("BUSY %s\n", mht->table[hash_index].key);
            my_hash_t secondary_hash_index;
            locate_t res_locate = locateEmptyIndex(mht, key, hash_index, &secondary_hash_index);
            switch (res_locate) {
                case EMPTY_INDEX_FOUND:
                      return mht_set_value(mht, secondary_hash_index, key, value, true);
                    break;
                case EXISTING_INDEX_FOUND:
                    return mht_set_value(mht, secondary_hash_index, key, value, false);
                    break;
                case NO_INDEX_FOUND:
                    return MHT_RES_ERROR_ALLOC;
                    break;
            }
            break;
    }
    return MHT_RES_ERROR_ALLOC; // если дошло сюда, значит ошибка
}

/**
* function isReachRehash
* brief внутренияя ф-я если значение count -количество элементов в таблице достигло значения порога, 
* то возвращаем true и вызывающая функция инициализирует рехэширование
* param mht -подхранилище с которой работаем
* retval bool True (1) - если порог рехэширвоания достигнут
* comment
*/
bool isReachRehash(mht_t *mht) {
    return mht->count > mht->rehash_treshold;
}

/**
* function __rehash_table
* brief внутренияя ф-я выполняет перехеширование при этом во вторичном подхранилище выдеяется память под новую таблицу,
* а затем данные поэлементно копируются из основной
* * param old_mht -текущее подхранилище, new_mht - новое подхранилище
* retval mht_result_t - результат выполнения if (PRINT_DBG)
*/
mht_result_t rehashTable(mht_t *old_mht, mht_t *new_mht) {
    if (PRINT_DBG) printf("rehash started count = %zu, threshold = %zu\n", old_mht->count, old_mht->rehash_treshold);
    if (myhTableInit(new_mht, old_mht->allocated_count*default_table_increment_k + default_table_increment) != MHT_OK) {
        if (PRINT_DBG)   printf("MHT_REHASH_INIT_ERROR\n");
        return MHT_REHASH_ERROR;
    }
    for (size_t i = 0; i < old_mht->allocated_count; i++) {
        if (old_mht->table[i].state == MHT_ITEM_BUSY) {
            if (PRINT_DBG) printf("rehash insert %zu |  ", i);
            if (myhTableInsert(new_mht, old_mht->table[i].key, old_mht->table[i].value) != MHT_OK) {
                if (PRINT_DBG) printf("MHT_REHASH_ERROR\n");
                return MHT_REHASH_ERROR;
            }
            if (PRINT_DBG) printf(" *** ins fin, all_cnt: %zu  cnt: %zu \n", new_mht->allocated_count, new_mht->count);
        }
    }
    if (PRINT_DBG) printf("end_rehash old_count %zu new count %zu \n", old_mht->count, new_mht->count);
    if (PRINT_DBG) printf("end_rehash old_count %zu new count %zu \n", old_mht->allocated_count, new_mht->allocated_count);
    //	free(old_mht->table);
    if (PRINT_DBG) printf("end_free\n");
    return MHT_OK;
}

/**
* function myh_table_insert
* brief внешняя функция выполнени вставки или модификации элемента в хранилище 
* предварительно проверяются условие на необхоимость рехеширования таблици и при выполнении условий - выполняет ее
*  mht - объект, хранилище таблице. key- ключ, value -значение
* retval mht_result_t - результат выполнения
*/
mht_result_t myh_table_insert(mht_storage_t *mht, char *key, int value) {
    if (isReachRehash(CurrVault(mht))) {
        if (PRINT_DBG) printf("rehash start\n");
        mht_result_t tmp_res = rehashTable(CurrVault(mht), SecVault(mht));
        if (PRINT_DBG)
            printf("rehsh_result, alloc_cnt %zu count %zu  \n", SecVault(mht)->allocated_count, SecVault(mht)->count);
        if (tmp_res != MHT_OK) {
            if (PRINT_DBG) printf("rehsh_error \n");
            return tmp_res;
        }
        swap_CV(mht);
    }
    return myhTableInsert(CurrVault(mht), key, value);
}

/**
* function myh_table_lookup
* brief вннешняя функция выполнени поиск элемента в хранилище 
*  mht - объект, хранилище таблице. key- ключ, value указатель куда прописывается-значение.
* retval mht_result_t - результат выполнения.
*/
mht_result_t myh_table_lookup(mht_storage_t *mht, char *key, int *value) {
    mht_t *my_mht = CurrVault(mht); // получаем текущую рабочее подхранилище.
    my_hash_t hash_index = hash_func(key, my_mht->allocated_count);
	*value = 0;
    if (hash_index > my_mht->allocated_count) {
        if (PRINT_DBG) printf("MHT_RES_HASH_INCORRECT\n");
        return MHT_RES_HASH_INCORRECT;
    }
    if (PRINT_DBG) printf("myh_table_lookup-0 \n");
    // если по индексе hash_index не найдено элемента, то выходим из функции возвращая MHT_RES_KEY_NOT_FOUND
    if ((my_mht->table[hash_index].state != MHT_ITEM_BUSY) &&
        (my_mht->table[hash_index].state != MHT_ITEM_DELETED))
        return MHT_RES_KEY_NOT_FOUND;
    // если ключи совпадают - то копируем зачение и выходим из функции, возвращая MHT_OK
    if (strcmp(my_mht->table[hash_index].key, key) == 0) {
        *value = my_mht->table[hash_index].value;
        return MHT_OK;
    }
    if (PRINT_DBG) printf("myh_table_lookup-1\n");
    for (my_hash_t i = hash_index + 1; i < my_mht->allocated_count; i++) {
        if (my_mht->table[i].state == MHT_ITEM_EMPTY) {
		return MHT_RES_KEY_NOT_FOUND;
        }
        if ((strcmp(my_mht->table[i].key, key) == 0)) {
		*value = my_mht->table[i].value;
		return MHT_OK;
        }
    }
    if (PRINT_DBG) printf("myh_table_lookup-2\n");
    for (my_hash_t i = 0; i < hash_index; i++) {
        if (my_mht->table[i].state == MHT_ITEM_EMPTY) {
            return MHT_RES_KEY_NOT_FOUND;
        }
        if ((strcmp(my_mht->table[i].key, key) == 0)) {
            *value = my_mht->table[i].value;
            return MHT_OK;
        }
    }
    return MHT_RES_KEY_NOT_FOUND;
}


void myh_table_free(mht_storage_t *mht) {
    mhtFree(&mht->vault_one);
    mhtFree(&mht->vault_two);
    free(mht);
}

size_t myh_table_get_count(mht_storage_t *mht) {
    mht_t *my_mht = CurrVault(mht);
    return my_mht->count;
}

size_t myh_table_get_capacity(mht_storage_t *mht) {
    mht_t *my_mht = CurrVault(mht);
    return my_mht->allocated_count;
}

mht_result_t myh_table_index(mht_storage_t *mht, size_t index, char *key, int *value){
    if (index >= myh_table_get_count(mht)){
        return MHT_RES_KEY_NOT_FOUND;
    }
    mht_t *my_mht = CurrVault(mht);
    my_hash_t hash_indx = my_mht->index_table[index];
    //*key = calloc(strlen(my_mht->table[hash_indx].key),sizeof (char));
	//if ((*key) == NULL){
	//	return 	MHT_RES_ERR_MEMORY_OUT;
	//}
    strcpy(key,my_mht->table[hash_indx].key);
    *value = my_mht->table[hash_indx].value;
    return MHT_OK;
}

 
