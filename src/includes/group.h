#include<stdio.h>
#include<stdbool.h>

char *join_group();
void leave_group();
char *search_group();


struct clients {
	struct in_addr client_addr;
	unsigned short client_port;
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
		
