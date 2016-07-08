/*
*   File:       myserver.c
*
*   Purpose:    This is a skeleton file for a server.
*/

#include "myunp.h"
#include "myunp.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>


#define BUFLEN 256                

int main(int argc, char **argv)
{

	int				  server_socket, client_socket, n;
	struct      sockaddr_in	servaddr;
  struct      sockaddr_in clientaddr;

  int         addrlen;
	char		    send_buffer[BUFLEN];
	char 			  recv_buffer[BUFLEN];
  char        buffer[BUFLEN];
	//time_t			  ticks;
	int         choice = 0;
	int         bytes;
	int 			  port_number;
	char		    conv[BUFLEN];
	int 			  connection_queue = 10;
	FILE         *f_ptr;
	char 			  cond[BUFLEN];
	char 			  check[20];
    int        x = 1;
    int 			option = 0;         
    int       file_option = 1;   // Flag for file's existence (1 if present,0 if not)
    int       file_size = 0;
    int       bytes_read;
    int       bytes_written;
    int       file_d;
    int       offset_size;
    int       chunk_size;
    int       err;
    int       size_check;  // To calculate the number of times the while loop has to run
    char       name[BUFLEN];
    char      *ip;
    fd_set     readfd;
    struct     timeval tv;
    int        maxfd = server_socket + 1;

    struct stat s = {0};

    addrlen = sizeof(clientaddr);

	if(argc != 2){
		printf("\n The Port Number was not specified..");
	}

    // Cretaing the UDP Server Socket
   	server_socket = Socket(AF_INET, SOCK_DGRAM, 0);

	if(server_socket == -1){
		printf("\n There was an error in creating the socket..");
	}

	//bzero(&servaddr, sizeof(servaddr));
	port_number = atoi(argv[1]); // Assigning the Port Number
  
  memset((char *) &servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port_number);
    
    // Binding the Addresses with the Socket Descriptor
	Bind(server_socket, (SA *) &servaddr, sizeof(servaddr));
   
    if(n == -1){
    	printf("\n There was an error in Binding the socket..");
    }

  printf("\n The Server is ready.. ");

  /*  // Listening for the connections on the specified port
    Listen(server_socket, connection_queue);

    if(n == -1){
    	printf("\n There was an error in Listening.. ");
    }
    else{
    	printf("\n The Server is UP and is Listening.. ");
    } */


    for( ; ; ){
      option = 0;

      printf("\n Waiting For Connection.. ");

    /*	client_socket = Accept(server_socket, (SA *) NULL, NULL);

    	if(client_socket == -1){
    		printf("\n There was an error in Accepting the Connection..");
    	}
    	else{
    		printf("\n The Client has connected.. ");
    	} */

    	bzero(recv_buffer,BUFLEN);
      
     // printf("\n Waiting for the first token..");
      if((n = recvfrom(server_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *)&clientaddr, &addrlen)) > 0){
          //  printf("\n Received the first token..");
          //  printf("\n The number of bytes received is : %d",n);
          //  printf(" \n The receive buffer is: %s", recv_buffer);
            if(strstr(recv_buffer, "FILE-CHECK") != NULL){
                 option = 1;
                 bzero(recv_buffer,BUFLEN);    
                
                 bzero(send_buffer,BUFLEN);
                 strcpy(send_buffer,"ACK");
               //  printf("\n Sending the ACK for the first token..");
                 ip = inet_ntoa(clientaddr.sin_addr);
               //  printf("\n The Client's IP is: %s", ip);    // Checking the Client IP Address taken from clientaddr
                 bytes = sendto(server_socket, send_buffer, BUFLEN,0, (struct sockaddr *)&clientaddr, addrlen);
               
                 if(bytes == -1){
                printf("\n Writing error..");
                   }
              //  printf("\n Sent the ACK to the first token..");
                

            }else{
              if(strstr(recv_buffer, "GET-DATA") != NULL){
                 option = 2;
                 bzero(recv_buffer,BUFLEN);                                      
            

                bzero(send_buffer,BUFLEN);
                strcpy(send_buffer,"ACK");
                ip = inet_ntoa(clientaddr.sin_addr);
                n = ntohs(clientaddr.sin_port);
              //  printf("\n The Client's IP is: %s and its port number is: %d", ip,n);
              //  printf("\n TOKEN: The send_buffer should have ACK and has: %s",send_buffer);
                bytes = sendto(server_socket, send_buffer, BUFLEN,0,(struct sockaddr *)&clientaddr, addrlen);
              //  printf("\n The number of bytes sent are: %d",bytes);
                
                if(bytes == -1){
                 printf("\n Writing error..");
                }
            }
       }
      } 

        if(option == 1){

    	         // option = 0;


                if((n = recvfrom(server_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *)&clientaddr, &addrlen)) > 0){
            
                char *strlocation;
                strlocation = strstr(recv_buffer, "End of file");
                //strcpy(strlocation,"\0");
                printf("\n The file name sent by Client is: %s",recv_buffer);  
              //  option = 2; 
                
              }
    	  	    
    	  	  
            
        
                // Code for checking if a file exists or not

                if( access(recv_buffer, F_OK ) != -1 ) {
                 file_option = 1;
                } 
                else {
                 file_option = 0;
                }   

                //bzero(recv_buffer,BUFLEN);


            // Code for getting the size of a file using stat 

            bzero(send_buffer,BUFLEN);  
            

            //char *fd = recv_buffer;
            if(file_option == 1){      
            stat(recv_buffer, &s);
            file_size = s.st_size;
             printf("\n The File Size given by stat is: %d",file_size);
            sprintf(send_buffer, "%d", file_size);
            printf("\n Send_buffer: %s",send_buffer);

           }
           else{
            strcpy(send_buffer,"NO FILE");
           }

            // Sending the file size or NO FILE message to the client
            bytes = sendto(server_socket, send_buffer, BUFLEN,0, (struct sockaddr *)&clientaddr, addrlen);
            if(bytes == -1){
                printf("\n Writing error..");
            }
          //  printf("\n Sent the FILE SIZE MESSAGE.. ");

         }
            
         else if(option == 2){

          bzero(recv_buffer,BUFLEN);  

        //  printf("\n Waiting for the File Name from the client thread..");

          //Reading the name of the file from the client
          if((n = recvfrom(server_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *)&clientaddr, &addrlen)) > 0){
            
               // char *strlocation;
               // strlocation = strstr(recv_buffer, "End of file");
                strcpy(name,recv_buffer);
              //  strcpy(strlocation,"\0");
               printf("\n The file name sent by Client is: %s",recv_buffer);  
              //  option = 2; 
                
              }

            //  printf("\n Have received the file name and now going to send ACK..");

              // Sending an ACK for the name

              bzero(send_buffer,BUFLEN);
              strcpy(send_buffer,"ACK");
              bytes = sendto(server_socket, send_buffer, sizeof(send_buffer),0, (struct sockaddr *)&clientaddr, addrlen);
            //  printf("\n FILE-NAME: The send buffer should have ACK but has: %s",send_buffer);
              if(bytes == -1){
              printf("\n Writing error..");
               }

             //  printf("\n Have sent the ACK for File Name..");

           
            //  printf("\n Waiting for the Offset Size from the client thread..");
             // fflush(NULL);

            // Waiting for the Offset_Size from the Client

            bzero(recv_buffer, BUFLEN);
            FD_ZERO(&readfd);
            FD_SET(server_socket,&readfd);
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            select (maxfd, &readfd, NULL, NULL, &tv);

             if(FD_ISSET(server_socket,&readfd)){
                  n = recvfrom(server_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *) &clientaddr, &addrlen);
                  offset_size = atoi(recv_buffer);
                //  printf("\n HAHAHAHA The offset sent by client: %s", recv_buffer);
              }

                    
                                

            
            
        //  printf("\n Received the Offset Size and now sending the ACK..");

          // Sending the ACK for Offset_Size
          bzero(send_buffer,BUFLEN);
          strcpy(send_buffer,"ACK");
          bytes = sendto(server_socket, send_buffer, sizeof(send_buffer),0, (struct sockaddr *)&clientaddr, addrlen);
        //  printf("\n OFFSET-SIZE: The send buffer should have ACK but has: %s",send_buffer);
           if(bytes == -1){
              printf("\n Writing error..");
          }

         // printf("\n Have sent the ACK for Offset Size.. Now Waiting for the Chunk Size");


          bzero(recv_buffer, BUFLEN);
          // Waiting for the chunk_size from the client
            FD_ZERO(&readfd);
            FD_SET(server_socket,&readfd);
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            select (maxfd, &readfd, NULL, NULL, &tv);

             if(FD_ISSET(server_socket,&readfd)){
                  n = recvfrom(server_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *) &clientaddr, &addrlen);
                  chunk_size = atoi(recv_buffer);
               //   printf("\n HAHAHAHA The chunksize sent by client: %s", recv_buffer);
              }

                    


        //  printf("\n Have received the Chunk SIZE..");
        
          printf("\n The offset size received is: %d",offset_size);
          printf("\n The chunk size received is: %d",chunk_size);
            

            
            // Sending the Data to the file

            
            
            f_ptr = fopen(name,"r");
            if(f_ptr==NULL)
              {
                printf("File open error");
                return 1;   
              }  

            err = fseek(f_ptr, offset_size, SEEK_SET);
            if(err < 0){
              printf("\n There was an error in traversing the file");
            }

            size_check = chunk_size/BUFLEN;   // Calculating the times the loop has to iterate
            
            bzero(send_buffer,BUFLEN);

            n = 0;
          //  printf("\n About to transfer the file to the client thread..");

            while (n != (size_check+1)) {
           
               bytes_read = fread(send_buffer,1,BUFLEN,f_ptr);
                
                if(bytes_read > 0)
                 {
                   // printf("Sending \n");
                    sendto(server_socket, send_buffer, BUFLEN,0, (struct sockaddr *)&clientaddr, addrlen);
                   // printf("\n The data being sent to client thread is: %s",send_buffer);
                 }

                 if (bytes_read < BUFLEN)
                {
                if (feof(f_ptr))
                    printf("End of file\n");
                if (ferror(f_ptr))
                    printf("Error reading\n");
                     break;
                } 
                n++;
              }

         //  printf("\n Transferred the file.. Now Sending the END OF FILE Token..");

           bzero(send_buffer,BUFLEN);
           strcpy(send_buffer,"ENDOFFILE");
           
           bytes = sendto(server_socket, send_buffer, strlen(send_buffer),0, (struct sockaddr *)&clientaddr, addrlen);
           
          if(bytes == -1){
              printf("\n Writing error..");
            }
         //  printf("\n Have Sent the END OF FILE TOKEN..");
          
      }


    //  close(client_socket);

       
    	  	  
          //recv_buffer[n] = 0;	/* null terminate */
        
      
    }

    
    return 0;
}

