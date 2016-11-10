#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "../includes/group.h"

#define PORT    4006
#define MAXMSG  512
#define GROUPSIZE 10

int J_CLI;

struct clients *client=NULL;
struct groups group[GROUPSIZE]; 
int **b=NULL;
int ib=0;
int NUM_CLI,PER_CLI;
enum operation {A=1,S,M,D,R=9999};

int
make_socket (int port)
{
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl (INADDR_ANY);
  if (bind (sock, (struct sockaddr *) &name, sizeof (name)) != 0)
    {
      perror ("bind");
      exit (EXIT_FAILURE);
    }

  return sock;
}


void assignGroup(struct clients *cli)
{
	int i;
	for(i=0;i<GROUPSIZE;i++)
	{
			if(strcmp(group[i].task,"SORTING")==0)
			{
				group[i].client[group[i].size].client_addr = cli->client_addr;
				group[i].client[group[i].size].client_port = cli->client_port;
                        	group[i].client[group[i].size].in_use = false;
				group[i].client[group[i].size].fd = cli->fd;
				printf("assign group  %d   %d",i,group[i].client[group[i].size].fd);
                        	group[i].client[group[i].size].capability = cli->capability;
				printf("in function %s size is %d\n",__FUNCTION__,group[i].size);
                      		group[i].size+=1;
			}
	}
}


void parse_res(char* buf)
{
	int a;
	char* tok;
	tok = strtok(buf," ");
	a=0;
	b[ib][a++] = (int) strtol(tok, (char **)NULL, 10);
	while(tok!=NULL)
	{
		tok = strtok(NULL," ");
		b[ib][a++] = (int) strtol(tok, (char **)NULL, 10);
	}
	ib=ib+1;
}


void merge()
{
	int cl[NUM_CLI];
	int i;
	while(i<NUM_CLI)
		cl[i]=0;
	while(cl[0] < PER_CLI && cl[1] < PER_CLI)
	{
		if(b[0][cl[0]] < b[1][cl[1]])
		{
			printf("%d\n",b[0][cl[0]]);
			cl[0] += 1;
		}
		if(b[0][cl[0]] > b[1][cl[1]])
		{
			printf("%d\n",b[0][cl[1]]);
			cl[1] += 1;
		}
	}

	if(cl[0]==PER_CLI)
	{
		for(i=cl[1];i<PER_CLI;i++)
			printf("%d\n",b[0][cl[1]]);	
	}
	else if(cl[1]==PER_CLI)
	{
		for(i=cl[0];i<PER_CLI;i++)
			printf("%d\n",b[0][cl[0]]);	
	}
	
}

void
read_from_client (int filedes,struct sockaddr_in clientname)
{
  printf("In function read_from_client");
  char buffer[MAXMSG];
  int nbytes;
  int capability_value = 0;
  char* res;
  char cap;
  nbytes = recv(filedes, buffer, sizeof(buffer),0);
  if (nbytes < 0)
    {
      /* Read error. */
      perror ("read");
      exit (EXIT_FAILURE);
    }
  else if (nbytes == 0)
    /* End-of-file. */
    return -1;
  else
    {
      /* Data read. */
      printf("In %s--- %c",__FUNCTION__,buffer[0]);
      switch (buffer[0]) {
	case 'J':
		capability_value = 1;
		break;
	case 'D':
		capability_value = 2;
		break;
	case 'R':
		capability_value = 3;
		fscanf(buffer,"%c %s",&cap,&res);
		break;
	default:
		capability_value = 0;
		break;
      }
      printf (stderr, "\n Server: got message: %s   %d\n", buffer,capability_value);
      
      if ( capability_value== 1)
      {
	printf("inside else \n");
	//Create Client structure and add it to group based on capability
        struct clients *tmpClient =(struct clients*) malloc(sizeof(struct clients));
	tmpClient->fd = filedes;
	printf("filedes   %d\n",tmpClient->fd);
        tmpClient->client_addr = clientname.sin_addr;
        tmpClient->client_port = clientname.sin_port;
        tmpClient->in_use = false;
	tmpClient->capability = capability_value;
	tmpClient->next = NULL;
	assignGroup(tmpClient);
	if(client == NULL)
		client  = tmpClient;
	else {
		tmpClient->next = client;
		client = tmpClient;
	}						
          		
      }
      else if(capability_value == 2)
      {
					
      }
      else if(capability_value == 3)
      {
	int i;
	if(b==NULL)
	{
		b = (int **) malloc(sizeof(int *)*NUM_CLI);
		for(i=0;i<NUM_CLI;i++)
		{
			b[i] = (int *) malloc(sizeof(int)*PER_CLI);	
		}
	}
	else {
		if(ib==NUM_CLI)
			merge();
		else {
			parse_res(res);
		}
		
	}	
      }
    }
}

int join_client() 
{

  int make_socket (int port);
  int sock;
  fd_set active_fd_set, read_fd_set;
  int i;
  struct sockaddr_in clientname;
  size_t size;

  /* Create the socket and set it up to accept connections. */
  sock = make_socket (PORT);
  if (listen (sock, 1) < 0)
    {
      perror ("listen");
      exit (EXIT_FAILURE);
    }

  /* Initialize the set of active sockets. */
  FD_ZERO (&active_fd_set);
  FD_SET (sock, &active_fd_set);

  while (1)
    {
      /* Block until input arrives on one or more active sockets. */
      read_fd_set = active_fd_set;
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
          perror ("select");
          exit (EXIT_FAILURE);
        }

      /* Service all the sockets with input pending. */
      for (i = 0; i < FD_SETSIZE; ++i)
        if (FD_ISSET (i, &read_fd_set))
          {
            if (i == sock)
              {
		printf("IF i= %d\n",i);
                /* Connection request on original socket. */
                int news;
                size = sizeof (clientname);

                news = accept (sock,
                              (struct sockaddr *) &clientname,
                              &size);

                if (news < 0)
                  {
                    perror ("accept");
                    exit (EXIT_FAILURE);
                  }

        /*        fprintf (stderr,
                         "Server: connect from host %s, port %hd.\n",
                         inet_ntoa (clientname.sin_addr),
                         ntohs (clientname.sin_port));*/
                FD_SET (news, &active_fd_set);
		J_CLI=i;
              }
            else
		{
		printf("%d iiii \n",i);
              read_from_client(i,clientname);
              }
          }

   }
	return 0;
}
void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
}
void print_group(){
	int i=0;
	for(i=0;i<GROUPSIZE;i++){
	    if(group[i].size == 0){
		printf("There is no element in %s group\n",group[i].name);
		continue;
	    }
	    printf("There are %d element in %s group   %d\n",group[i].size,group[i].name,group[i].client[0].fd);
	}
}

int sorting_group_search()
{	
	int i;
	int max, index = -1;
	max=0;
	for(i=0;i<GROUPSIZE;i++)
	{
		if(strcmp(group[i].task,"SORTING")==0)
		{
			if(group[i].size > max)
			{
				max = group[i].size;
				index = i;
			}
		}	
	}
	return index;
}

char *my_itoa(int num, char *str)
{
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}

void distribute_task(int num_cli,int grp_index,int *a,int n)
{
	int arr[100000];
	
	int i,j,k1,count,count_client=0,fl;
	j=n/num_cli;
	PER_CLI = j;
	count=0;

	for(k1=0;k1<num_cli;k1++)
	{
		printf("kjkjkjkj  %d\n",k1);
		char buf[100000]="";
		char str_num[5];
		for(i=j*k1;i<(j+k1*j)-1;i++)
		{
			char str_num[5];
			my_itoa(a[i],str_num);
			strcat(buf,str_num);
			strcat(buf," ");
		}
		printf("kkkkk   %d\n",k1);	
		my_itoa(a[i],str_num);
                strcat(buf,str_num);
		printf("%s\n\n",str_num);
		printf("distri\n\n");
		printf("distri 111 %s\n",group[grp_index].task);
		printf("in function %s size is %d, k=%d\n",__FUNCTION__,group[grp_index].size,k1);
		printf("%d \t %s\n",group[grp_index].client[k1].fd,buf);
		if(send(group[grp_index].client[k1].fd,buf,strlen(buf),0)<0)
			printf("ERROR\n");

	}
}
void sorting()
{
	FILE *fp;
	int num_cli,n,grp_index;
	int arr[100000];
	fp = fopen("sort.txt","r");
	n=0;
	while(!feof(fp))
	{
		fscanf(fp,"%d",&arr[n]);
		n+=1;
	}
	
	grp_index = sorting_group_search();		
	printf("goupnindex   %d\n",grp_index);
	if(grp_index != -1) {
		num_cli = group[grp_index].size;
		NUM_CLI=num_cli;
		distribute_task(num_cli,grp_index,arr,n);		
	}
	else {
		printf("No such group exist to do the intending task\n");
	}
	return;
}

void arithmetic()
{}

void max_min()
{}

void search_group()
{}

void perform_task()
{
	int choice;
	printf("*******************************\n");
        printf("Type of task\n");
        printf("1. Sorting\n");
        printf("2. Arithmetic\n");
        printf("3. Maximum and Minimum\n");
        printf("*******************************\n");
	printf("Enter the choice for the task :");
	scanf("%d",&choice);

	switch(choice)
	{
		case 1:
			sorting();
			break;
		case 2: 
			arithmetic();
			break;
		case 3:
			max_min();
			break;
		default:
			printf("!!!!!!!!!  Task Aborted  !!!!!!!!!!\n");
	}
}


void process_cli(){
    int choice;
    do{
	printf("######################\n");
	printf("Enter your choice\n");
	printf("1. Print Groups details\n");
	printf("2. Search for specific group\n");
	printf("3. Perfornm Task\n");
	printf("######################\n");
	scanf("%d",&choice);
	switch(choice){
		case 1 :
		    print_group();
		    break;
		case 2:
		    search_group();
		    break;	
		default:
		    perform_task();	
		    break;	
	}		
    }while(1);
}
void defineGroup()
{
	int i;
	for(i=0;i<GROUPSIZE;i++)
	{
		if(i%3==0)
		{
			strcpy(group[i].task,"ARITHMETIC");
			strcpy(group[i].name,"ARITHMETIC");
			group[i].size=0;
		}

		if(i%3==1)
		{
			strcpy(group[i].task,"MAXIMUM");
		        strcpy(group[i].name,"MAXIMUM");
               		group[i].size=0;
		}
		if(i%3==2)
		{
			strcpy(group[i].task,"SORTING");
               		strcpy(group[i].name,"SORTING");
               		group[i].size=0;
		}
	}
}

int
main (void)
{
     pthread_t thread1,thread2;
     int  iret1,iret2;

    /* Create independent threads each of which will execute function */
     defineGroup();	
     iret1 = pthread_create( &thread1, NULL,join_client , NULL);
     iret2 = pthread_create( &thread2, NULL,process_cli , NULL);
     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL);
     printf("Thread 2 returns: %d\n",iret2);
     printf("Thread 1 returns: %d\n",iret1);
     exit(0);
     return 0;
}
