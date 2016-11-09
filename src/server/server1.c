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

struct clients *client=NULL;
struct groups group[GROUPSIZE]; 
enum operation {A=1,S,M,D,R=9999};

int
make_socket (int port)
{
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
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

int
read_from_client (int filedes)
{
  char buffer[MAXMSG];
  int nbytes;
  int capability_value = 0;
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
      switch (buffer[0]) {
	case 'A':
		capability_value = 1;
		break;
	case 'S':
		capability_value = 2;
		break;
	case 'M':
		capability_value = 3;
		break;
	case 'D' :
		capability_value = 4;
		break;
	case 'R' :
		capability_value = 9999;
		break;
	default:
		capability_value = 0;
		break;
      }
      fprintf (stderr, "\n Server: got message: %s   %d\n", buffer,capability_value);
      memset(buffer, '\0', sizeof(buffer));
      sprintf(buffer, "%s", "JOINED");
      send(filedes, buffer, strlen(buffer), 0);
      return capability_value;
    }
}


void assignGroup(struct clients *cli)
{
	int i;
	for(i=0;i<GROUPSIZE;i++)
	{
		if (cli->capability >=1 && cli->capability <= 4)
		{
			if(strcmp(group[i].task,"SORTING")==0)
			{
				group[i].client[group[i].size].client_addr = cli->client_addr;
				group[i].client[group[i].size].client_port = cli->client_port;
                        	group[i].client[group[i].size].in_use = false;
                        	group[i].client[group[i].size].capability = cli->capability;
                      		group[i].size+=1;
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
//printf("sdsdsd\n");
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
//printf("dfdddddd\n");
                news = accept (sock,
                              (struct sockaddr *) &clientname,
                              &size);
//printf("ggfgfgfgfggf\n");
                if (news < 0)
                  {
                    perror ("accept");
                    exit (EXIT_FAILURE);
                  }
//printf("4\n");
        /*        fprintf (stderr,
                         "Server: connect from host %s, port %hd.\n",
                         inet_ntoa (clientname.sin_addr),
                         ntohs (clientname.sin_port));*/
                FD_SET (news, &active_fd_set);
//printf("5\n");
              }
            else
              {
	//	printf("Else i= %d, sock = %d\n",i,sock);
                /* Data arriving on an already-connected socket. */
		int temp_capability = read_from_client(i);
                if (temp_capability >= 0)
                {
			printf("inside else \n");
		    	//Create Client structure and add it to group based on capability
               		struct clients *tmpClient =(struct clients*) malloc(sizeof(struct clients));
			tmpClient->fd = i;
                	tmpClient->client_addr = clientname.sin_addr;
                	tmpClient->client_port = clientname.sin_port;
                	tmpClient->in_use = false;
			tmpClient->capability = temp_capability;
			tmpClient->next = NULL;
			assignGroup(tmpClient);

			if(client == NULL)
				client  = tmpClient;

			else {
				tmpClient->next = client;
				client = tmpClient;
			}						
                //    	close (i);
                  //  	FD_CLR (i, &active_fd_set);

			
                  }
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
	    printf("There are %d element in %s group\n",group[i].size,group[i].name);
	}
}

int sorting_group_search()
{	
	int i;
	int max, index;
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

void distribute_task(int num_cli,struct group G,int grp_index,int *a,int n)
{
	int arr[100000];
	
	int i,j,k,count,count_client=0;
	j=n/num_cli;

	count=0;
	for(k=0;k<num_cli;k++);
	{
		char buf[100000]="";
		for(i=j*k;i<(j+k*j)-1;i++)
		{
			char str_num[5];
			my_itoa(a[i],str_num);
			strcat(buf,str_num);
			strcat(buf," ");
		}	
		my_itoa(a[i],str_num);
                strcat(buf,str_num);
		send(G.client[k]->fd,buf,strlen(buf),0);

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
		fscanf(fp,"%d",arr[i]);
		n+=1;
	}
	
	grp_index = sorting_group_search();		
	num_cli = group[grp_index].size;
	distribute_task(num_cli,group[grp_index],grp_index,arr,n);		
}

void perform_task()
{
	int choice;
	printf("*******************************\n");
        printf("Type of task\n");
        printf("1. Sorting\n");
        printf("2. Arithmetic\n");
        printf("3. Maximum and Minimum\n");
        printf("*******************************\n");
	scanf("Enter the choice for the task : %d",&choice);

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
