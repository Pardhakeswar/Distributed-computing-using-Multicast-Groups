#include<stdio.h>
#include<stdbool.h>

char *join_group();
void leave_group();
char *search_group();


struct clients {
	struct in_addr client_addr;
	unsigned short client_port;
	char *grp_name;
	int capability;
    int fd;
	bool in_use;
	struct clients *next;	
};	
	

struct groups {
	char name[15];
	struct clients client[255];	
	int size;
	bool in_use;
	char task[15];
};	
		
