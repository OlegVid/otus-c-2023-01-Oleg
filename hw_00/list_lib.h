#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* demo linked list struct
 */
typedef struct pair{
	int value;
	struct pair* next;
} pair;

/* make_pair 
 * allocate in memory new item of struct pair and link it with previos iterm
 * 
 * */

pair* make_pair(int value, pair* next);

/* filter 
 * return filtered list from source *list, condition for filtering - predicate
 */
pair* filter(pair* list, bool (*predicate)(int));

/* map
 * return modifed list from source list, modifier in arg - modif
 */
pair* map(pair* list, int (*modif)(int));
/*inv_list
 *inverting of items order in list, in order to suppress effect of inverting item order in list in map or filter operations
 */
pair* inv_list(pair* list);
