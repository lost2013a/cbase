#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ELEMENT_NB 10

struct element{
unsigned int id;
unsigned int cnt;
};

struct table{
	struct element ele[MAX_ELEMENT_NB];
	unsigned int nb;
};

void table_init(void)
{
	return;
}

unsigned int find_element(unsigned int id)
{
	unsigned int i;
	for(i=0; i<MAX_ELEMENT_NB; i++){
		if(table.ele[i].id == id){
			break;
		}
	}
	if(i == MAX_ELEMENT_NB)
		i=0;
	return i;
}

void table_add(unsigned  int id)
{
	unsigned int i;
	i= find_element(id);
	if(0 == i){
		if(table.nb < MAX_ELEMENT_NB){
			table.nb ++;
			printf("new table\n");
		}
		else
			printf("table full\n");
	}
	else{
		printf("old table\n");
	}
}

void table_loop(unsigned  int id)
{
	unsigned int i;
	for(i=0; i<MAX_ELEMENT_NB; i++){
		if(table.ele[i].cnt > 0){
			table.ele[i].cnt--;
		}
	}
}

void table_printf(unsigned  int id)
{
	unsigned int i;
	printf("id table:\n");
	for(i=0; i<MAX_ELEMENT_NB; i++){
		if(table.ele[i].cnt > 0){
			printf("id: %03d\n", table.ele[i].id);
		}
	}
	printf("\n");
}

int main(int argc ,char** argv)
{

	return 0;
}



