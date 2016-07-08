/*
*   File:       myclient.c
*
*   Purpose:    This is a skeleton file for a client.
*/

#include "myunp.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFFSIZE 16384
int main(int argc, char *argv[])
{

	int					client_socket, n;
	char				recv_buffer[BUFFSIZE];
	char 				send_buffer[256];
	struct      sockaddr_in	servaddr;
	int         port_number;
	int 				bytes;
	char 				check[256];
	FILE 				*fp;
	int 				choice = 0;
    
    if (argc != 3)
		err_quit(" The IP Address and the Port Number were not specified properly ");

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket == -1){
		printf("\n There was an error in creating the socket..");
	}

	port_number = atoi(argv[2]);//Assigning the port number
     
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port_number);

	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)  // Converting the IP address specifies from string to network order
		err_quit("inet_pton error for IP: %s", argv[1]);

	if (connect(client_socket, (SA *) &servaddr, sizeof(servaddr)) < 0){
		err_sys("Connection Establishment Error");
    }
    else{
    	printf("\n Successfully connected to the client.. ");
    }

    while(choice == 0){
			printf("\n Please enter the command: ");
			bzero(send_buffer,256);

			fgets(send_buffer,  sizeof(send_buffer), stdin);
      send_buffer[strcspn(send_buffer,"\n")] = '\0'; // To Remove the /n in fgets
      //printf("\n The command entered by client is:%s+",send_buffer);
            bytes =  Write(client_socket, send_buffer, strlen(send_buffer));
            if(bytes == -1){
            	printf("\n Writing error..");
            }

            if(strcmp(send_buffer,"exit") == 0){
            	choice = 1;
            } 

           
              
    /* Receive data in chunks of BUFFSIZE bytes */

      
    fp = fopen("sample_file.txt", "ab"); 
    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

     // printf("\n The data returned by server is:\n");
      while((bytes = read(client_socket, recv_buffer, BUFFSIZE)) > 0)
      {
           fwrite(recv_buffer, 1, bytes,fp);
           if(strstr(recv_buffer, "ACK") != NULL ) {
                //choice = 1;
               recv_buffer[bytes - strlen("ACK") ] = 0;
               printf("%s \n", recv_buffer);
               break;
            }
            // printf("Bytes received %d\n",bytes);    
            recv_buffer[bytes] = 0;
            printf("%s \n", recv_buffer);
                }
     // printf("\n I got out of the while loop..");

       if(bytes < 0)
       {
         printf("\n Read Error \n");
       }

           
           // fgets(conv, sizeof(conv), stdin);
            //choice = atoi(conv); 


    }    

    printf("\n The client is ending it's connection..");
    return 0;
}
