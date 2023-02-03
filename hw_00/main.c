/* homework for course c-2023-01 pre lesson functional programming in C
 *author: Oleg Videnin
 *date:   03/02/2023
 * map and filter implementation in C 
 * */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list_lib.h" 

/*
 * modifier - function modifier for MAP 
 */
static int modifier(int inp_value){
	return inp_value * 5;
}

/*
 * is_even - filtering function for FILTER 
 */
static bool is_even(int value){
	return (value % 2) == 0;
}

static void print_list(pair *list,char *desc){
	printf("\n %s \n", desc);
	for (pair* p = list; p ; p = p->next){
		printf("value = %d \n", p->value);
	}
	printf("---------------------\n");
}


int main (void){
	pair* list = &(pair){4, &(pair){8, &(pair){17, &(pair){33, &(pair){14,&(pair){27,NULL}}}}}};	
	
	pair* filtered = inv_list(filter(list, is_even));
	pair* mapped =  inv_list(map(list,  modifier)); 
	
	print_list(list,"initial list:");
	print_list(filtered, "filtered list:") ;
	print_list(mapped,"maped list:");

	return 0;
}

