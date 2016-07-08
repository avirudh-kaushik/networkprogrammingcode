/*
*   File:       myserver.c
*
*   Purpose:    This is a skeleton file for a server.
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
  int				  server_socket, client_socket, n;
	struct      sockaddr_in	servaddr;
	char				send_buffer[BUFFSIZE];
	char 				recv_buffer[256];
	time_t			ticks;
	int         choice = 0;
	int         bytes;
	int 				port_number;
	char				conv[256];
	int 				connection_queue = 10;
	FILE        *f_ptr;
	char 				cond[256];
	char 				check[20];
  int         x = 0;

	if(argc != 2){
		printf("\n The Port Number was not specified..");
	}

    // Cretaing the Server Socket
   	server_socket = Socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket == -1){
		printf("\n There was an error in creating the socket..");
	}

	bzero(&servaddr, sizeof(servaddr));
	port_number = atoi(argv[1]); // Assigning the Port Number

	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port_number);
    
    // Binding the Addresses with the Socket Descriptor
	Bind(server_socket, (SA *) &servaddr, sizeof(servaddr));
   
    if(n == -1){
    	printf("\n There was an error in Binding the socket..");
    }
    
    // Listening for the connections on the specified port
    Listen(server_socket, connection_queue);

    if(n == -1){
    	printf("\n There was an error in Listening.. ");
    }

    for( ; ; ){
    	client_socket = Accept(server_socket, (SA *) NULL, NULL);

    	if(client_socket == -1){
    		printf("\n There was an error in Accepting the Connection..");
    	}

    	while(choice == 0){
    	   bzero(recv_buffer,256);

    	  if( (n = read(client_socket, recv_buffer, 256))== 0){
              printf("\n The client has crashed..");
              break;
            }
    	   if (n < 0)
		     err_sys(" \n There was an error in reading the data..");
		     recv_buffer[n] = 0;	/* null terminate */
         printf("\n The command received from client is: %s \n",recv_buffer);

         strcpy(check,"exit");
		   // Checking for the end condition of the client 
           if(strstr(recv_buffer, check) != NULL) {
                //choice = 1;
           	    break;
            }
		   
		   
		  // if (fputs(recv_buffer, stdout) == EOF)
			 //  err_sys(" \n There was an error in reading the file..");

		   f_ptr = popen(recv_buffer,"r");  // Reading the output of the shell command in the file pointer.. 
       

		   if(f_ptr == NULL) 
           {
             perror("Error in opening file");
             return(-1);
           }
           else{
            x = 1;
           }
       
           
           // Whole while loop is for sending the data from the file pointer to the client side
           while(x == 1)
          {
            /* First read file in chunks of BUFFSIZE bytes */
            bzero(send_buffer,BUFFSIZE);
            int nread = fread(send_buffer,1,BUFFSIZE,f_ptr);
            //printf("Bytes read %d \n", nread);        

            /* If read was success, send data. */
            if(nread > 0)
            {
               // printf("Sending \n");
                write(client_socket, send_buffer, nread);
            }

            /*
             * There is something tricky going on with read .. 
             * Either there was error, or we reached end of file.
             */
            if (nread < BUFFSIZE)
            {
                if (feof(f_ptr)){
                    bzero(send_buffer,BUFFSIZE);
                    strcpy(send_buffer,"ACK");
                    write(client_socket, send_buffer, strlen(send_buffer));
                   // printf("End of file\n");
                    x = 0;
                  }
                if (ferror(f_ptr)){
                    printf("Error reading\n");
                    x = 0;
                  }
                
            }
          

        }
       // printf("\n We came outside the while loop.."); 
        
         /*  printf("\n The output of the command is:\n");
            
           // Getting the data from the file pointer and displaying on the screen..
           do{
             int c = fgetc(f_ptr);
             if( feof(f_ptr) )
                {
                  break ;
                }
             printf("%c", c);
            }while(1);
            */


		   bzero(recv_buffer,256);


		   
	    }
          
        printf("\n The previous client has disconnected.. \n");
        choice = 0;
	      close(client_socket); 
	     


     }

   return 0;
}
