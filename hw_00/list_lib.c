
/* list_lib.c
 * Author: Oleg Videnin
 * impleentation for list control functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list_lib.h"


static pair* _filter(pair* list, pair* result, bool (*predicate)(int));
static pair* _map(pair* list, pair* result, int (*modif)(int));
static pair* _inv_list(pair* list, pair* result);


pair* make_pair(int value, pair* next){
	pair* result = malloc(sizeof(pair));
	result->value = value;
	result->next = next;
	return result;
}

/*
 * filter - wrapper for function _filter
 * wrapper made for exclude auxilary argument for interface
 * 
 * */

pair* filter(pair* list, bool (*predicate)(int)){
	return _filter(list, NULL, predicate);
}

/*
 * map - wrapper for function _map
 * wrapper made for exclude auxilary argument for interface
 * */
pair* map(pair* list, int (*modif)(int)){
	return _map(list, NULL, modif);
}

/*
 * inv_list - wrapper for function _inv_list
 * wrapper made for exclude auxilary argument for interface
 * */

pair* inv_list(pair* list){
	return _inv_list(list, NULL);
}

static pair* _filter(pair* list, pair* result, bool (*predicate)(int)){
	if (!list)
		return result;
	return _filter(list->next, predicate(list->value) ? make_pair(list->value,result): result, predicate);
}


static pair* _map(pair* list, pair* result, int (*modif)(int)){
	if (!list)
		return result;
	return _map(list->next, make_pair(modif(list->value),result),modif);
}

static pair* _inv_list(pair* list, pair* result){
	if (!list)
		return result;
	return _inv_list(list->next, make_pair(list->value,result));
}
