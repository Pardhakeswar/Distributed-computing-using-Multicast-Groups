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
#include <pthread.h>

#include "../includes/logger.h"


#define PORT 4006

void JoinToGroup(char *ip_addr, char capability)
{
    int sockfd = 0;
    char sendBuff[1024];
    struct sockaddr_in serv_addr;
    int i=0;
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); 
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); 
    
    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return;
    } 
    
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return;
    }

    memset(sendBuff, '\0', sizeof(sendBuff));
    sprintf(sendBuff, "%c", capability);
    send(sockfd, sendBuff, strlen(sendBuff), 0);
    memset(sendBuff, '\0', sizeof(sendBuff));
    recv(sockfd, sendBuff, sizeof(sendBuff),0);
    printf("%s\n",sendBuff);
    if(strcmp(sendBuff,"JOINED")!=0)
    {
      printf("Client could not joined");
      return;
    }     
}

void ExitFromGroup()
{
    //ToDo
}

void JoinOrExit(){
    int choice;
    char capability;
    char ser_ip_addr[16];
    do{
	printf("######################\n");
	printf("Enter your choice\n");
	printf("1. Join to group\n");
	printf("2. Exit from group\n");
	printf("######################\n");
	scanf("%d",&choice);
    
	switch(choice){
		case 1 :
            //printf("Enter capability of the client\n");
            //capability = getchar();
            capability = 'A';
            printf("Enter the server IP Address\n");
            scanf("%s",ser_ip_addr);
		    JoinToGroup(ser_ip_addr, capability);
		    break;
		case 2:
            ExitFromGroup();
		    break;
		default:
		    break;	
	}		
    }while(1);
}

void PerformTask()
{
    //to do
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    int i=0;
    char receive_buff[1024];
    char ser_ip_addr[16] = "127.0.0.1";

    memset(&serv_addr, '0', sizeof(serv_addr)); 
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); 

    do{
        
        if(inet_pton(AF_INET, ser_ip_addr, &serv_addr.sin_addr)<=0)
        {
            printf("\n inet_pton error occured\n");
            return;
        } 
        
        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
           printf("\n Error : Connect Failed \n");
           return;
        }

        memset(receive_buff, '\0', sizeof(receive_buff));
        recv(sockfd, receive_buff, sizeof(receive_buff),0);
        printf("%s\n",receive_buff);
        if(strcmp(receive_buff,"JOINED")==0)
        {
           printf("In PerformTask JOINED\n");
        }
        else
        {
            printf("Client Received some work\n");

            printf("Data is : %s\n", receive_buff);

        }
    }while(1);
    return;    
}


int main(int argc, char *argv[])
{
    pthread_t thread1,thread2;
    int  iret1,iret2;

    iret1 = pthread_create( &thread1, NULL, PerformTask, NULL);
    iret2 = pthread_create( &thread2, NULL, JoinOrExit, NULL);

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    printf("Thread 2 returns: %d\n",iret2);
    printf("Thread 1 returns: %d\n",iret1);

    return 0;
}


int * sortNumbers(int *array, int num_of_elements)
{
    int indx_i = 0;
    int indx_j = 0;
    int temp = 0;

    for (indx_i = 0 ; indx_i < ( num_of_elements - 1 ); indx_i++)
    {
        for (indx_j = 0 ; indx_j < num_of_elements - indx_i - 1; indx_j++)
        {
            if (array[indx_j] > array[indx_j+1]) /* For decreasing order use < */
            {
                temp            = array[indx_j];
                array[indx_j]   = array[indx_j+1];
                array[indx_j+1] = temp;
            }
        }
    }

    return array;
}




