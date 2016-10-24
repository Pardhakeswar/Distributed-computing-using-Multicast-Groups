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
	bool in_use;
	struct clients *next;	
};	
	

struct groups {
	char name[15];
<<<<<<< HEAD
	struct clients *client[255];	
=======
	struct clients client[255];	
>>>>>>> ccff69f49fbed711813828a35bbfca77aa4c8901
	int size;
	bool in_use;
	char task[15];
};	
		
