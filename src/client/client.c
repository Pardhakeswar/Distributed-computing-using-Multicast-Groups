#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "../includes/logger.h"

int main(int argc, char *argv[])
{
pid_t pid;
//int choice;
int a,c,res;
char cap;
int sockfd = 0;
char sendBuff[1024];
struct sockaddr_in serv_addr;
int i=0;

if(argc != 2)
{
    printf("\n Usage: %s <ip of server> \n",argv[0]);
    return 1;
} 

if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
    
    ERROR_LOG("\n Error : Could not create socket \n");
    return 1;
} 

memset(&serv_addr, '0', sizeof(serv_addr)); 

serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(5000); 

if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
{
    ERROR_LOG("\n inet_pton error occured\n");
    return 1;
} 

if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
{
   ERROR_LOG("\n Error : Connect Failed \n");
   return 1;
}


fork();
pid = getpid();
if(pid==0)
{
while(1)
{
	printf("Press 1, to join a group\n");
	printf("Press 2, to leave the group\n");
	printf("Press 3, to search the group\n");
	int choice;
	scanf("Enter your choice here : %d",&choice);
	
	
	
		/*do{
		  memset(sendBuff, '\0', sizeof(sendBuff));	
		  sprintf(sendBuff, "Server got This is line %d", i);
	 	  send(sockfd, sendBuff, strlen(sendBuff), 0);
	  	  memset(sendBuff, '\0', sizeof(sendBuff));
	  	  recv(sockfd, sendBuff, sizeof(sendBuff),0);
	  	  printf( "%s\n", sendBuff);
	  	  //sleep(5);
		}while(++i<2);*/
		printf("\nEnter the capability (A,S,D,M)");
		scanf("%c",&cap);
		switch(choice) {
			case 1: 
				sprintf(sendBuff,"%c %s",&cap,"JOIN");
				send(sockfd, sendBuff, strlen(sendBuff), 0);
				recv(sockfd, sendBuff, sizeof(sendBuff),0);
				printf( "%s\n", sendBuff);
				break;
			case 2: 
				sprintf(sendBuff,"%c %s",&cap,"LEAVE");
				send(sockfd, sendBuff, strlen(sendBuff), 0);
				recv(sockfd, sendBuff, sizeof(sendBuff),0);
				printf( "%s\n", sendBuff);
				break;
			case 3: 
				sprintf(sendBuff,"%c %s",&cap,"SEARCH");
				send(sockfd, sendBuff, strlen(sendBuff), 0);
				recv(sockfd, sendBuff, sizeof(sendBuff),0);
				printf( "%s\n", sendBuff);
				break;
		}
	}
}	
	else {
		/*recv(sockfd, recvBuff, sizeof(recvBuff),0);
		sscanf(recvBuff,"%d %d",&a,&b);
		if(cap == 'A');
			res=a+b;
		else if(cap == 'S');
			res=a-b;
		else if(cap == 'D');
			res=a/b;
		else if(cap == 'M');
			res=a*b;
		sprintf(sendBuff,"%d",&res);
		send(sockfd, sendBuff, strlen(sendBuff), 0);	*/
		printf("sadasdasdasd\n");
	}


return 0;
}

