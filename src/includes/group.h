#include<stdio.h>


char *join_group();
void leave_group();
char *search_group();


struct clients {
	char ip[15];
	char *grp_name;
	char capability;
	bool in_use;
};	
	

struct groups {
	char *name;
	struct clients client[255];	
	int size;
	bool in_use;
};	
		
