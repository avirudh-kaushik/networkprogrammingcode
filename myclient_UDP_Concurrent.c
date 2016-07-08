/*
*   File:       myclient.c
*
*   Purpose:    This is a skeleton file for a client.
*/

#include "myunp.h"
#include <time.h>
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
#include <pthread.h>  
#include <time.h> 

#define BUFLEN 256

// Global Variables for Storing IP Addresses and Ports of the server
char        IP_address[10][100];
char        Port_Number[10][100];



int         recv_length;
int         offset_size[10];
int         chunk_size[10];
char        name[BUFLEN];
int         number_of_servers;
int         max = 0;

pthread_mutex_t lock;


void *client_thread(void *struct_vals){

  char      send_buffer[BUFLEN];
  char      recv_buffer[BUFLEN];
  char      tok[BUFLEN];
  char      buffer[BUFLEN];
  int       offset_size;
  int       chunk_size;
  int       id,x;
  int       err;
  int       bytes;
  int       client_socket_1, n = 0;
  int       port_number;
  FILE      *output_file;
  off_t      position;
  int        charsToDelete;
  struct    sockaddr_in servaddr_1;
  struct    sockaddr_in clientaddr_1;
  struct    sockaddr_in tempaddr;
  socklen_t len = sizeof(clientaddr_1);
  int       addrlen_1;
  fd_set    readfd_1;
  struct    timeval tv;
  int       maxfd;
  


  addrlen_1 = sizeof(tempaddr);

  

  thread_struct *thread_data;
  thread_data = (thread_struct *)struct_vals;

  offset_size = thread_data->offset_size;
  chunk_size = thread_data->chunk_size;
  id = thread_data->thread_id;

    printf("\n The id is: %d", id);
    printf("\n The offset_size is: %d", offset_size);
    printf("\n The chunk size is: %d", chunk_size);

    // Creating the UDP Socket Descriptor for Thread

    client_socket_1 = socket(AF_INET, SOCK_DGRAM, 0);

    maxfd = client_socket_1 + 1;

    if(client_socket_1 == -1){
       printf("\n There was an error in creating the socket..");
      }


     memset((char *) &clientaddr_1, 0, sizeof(clientaddr_1)); 
     clientaddr_1.sin_family      = AF_INET;
     clientaddr_1.sin_addr.s_addr = htonl(INADDR_ANY);
     clientaddr_1.sin_port        = htons(0);
    
   // Binding the Addresses with the Client Socket Descriptor
     Bind(client_socket_1, (SA *) &clientaddr_1, sizeof(clientaddr_1));  

    port_number = atoi(Port_Number[id]);                               //Assigning the port number
     
   // bzero(&servaddr, sizeof(servaddr));
    memset((char *) &servaddr_1, 0, sizeof(servaddr_1)); 
    servaddr_1.sin_family = AF_INET;
    servaddr_1.sin_port   = htons(port_number);                         //Converting the port number to the network order

    if (inet_pton(AF_INET, IP_address[id], &servaddr_1.sin_addr) <= 0)  // Converting the IP address specified from string to network order
    err_quit("inet_pton error for IP: %s", IP_address[id]);



   /* if (connect(client_socket, (SA *) &servaddr, sizeof(servaddr)) < 0){
    err_sys("Connection Establishment Error");
    }
    else{
      printf("\n Client_Thread %d is UP and RUNNING !!  ",id+1);
    }*/


    bzero(tok,BUFLEN); 
    strcpy(tok,"GET-DATA"); // The token which will notify server that we are asking for size of file


        // Sending the Token for Retreiving data
        bytes = sendto(client_socket_1, tok, BUFLEN,0,(struct sockaddr *)&servaddr_1, sizeof(servaddr_1));
      //  printf("\n The number of bytes sent are: %d",bytes);

        if(bytes == -1){
              printf("\n Writing error in the token..");
        }
        // Waiting for an ACK for the token
        
       bzero(recv_buffer, BUFLEN);

       FD_ZERO(&readfd_1);
       FD_SET(client_socket_1,&readfd_1);
       tv.tv_sec = 1;
       tv.tv_usec = 0;

       select (maxfd, &readfd_1, NULL, NULL, &tv);

       if(FD_ISSET(client_socket_1,&readfd_1)){
            n = recvfrom(client_socket_1, recv_buffer, BUFLEN, 0, (struct sockaddr *) &tempaddr, &addrlen_1);
          //  printf("\n The number of bytes received are: %d",n);
          }

        
       /*  if (getsockname(client_socket_1, (struct sockaddr *)&clientaddr_1, &len) == -1)
          perror("getsockname");
         else
          printf(" \n port number of client thread is: %d\n", ntohs(clientaddr_1.sin_port)); */
        // printf("\n The port number of thread is: %d",x); */
        

        
       
          

        // Sending the file name to the server
        bzero(send_buffer,BUFLEN);
        strcpy(send_buffer,name);

        bytes = sendto(client_socket_1, send_buffer, sizeof(send_buffer),0,(struct sockaddr *)&servaddr_1, sizeof(servaddr_1));
       // printf("\n File name sent is : %s",send_buffer);

        if(bytes == -1){
              printf("\n Writing error..");
        }


        // Waiting for an ACK for the file name
          bzero(recv_buffer,BUFLEN);
          FD_ZERO(&readfd_1);
          FD_SET(client_socket_1,&readfd_1);
          tv.tv_sec = 1;
          tv.tv_usec = 0;

          select (maxfd, &readfd_1, NULL, NULL, &tv);

          if(FD_ISSET(client_socket_1,&readfd_1)){
            n = recvfrom(client_socket_1, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &tempaddr, &addrlen_1);
          //  printf("\n The number of bytes received are: %d",n);
          //  printf("\n FILE NAME: The recv_buffer should have ACK but has: %s", recv_buffer);
          }

         
      
          
          

  
     // Sending the offset size
     bzero(send_buffer,BUFLEN);
     sprintf(send_buffer, "%d", offset_size);
     bytes = sendto(client_socket_1, send_buffer, BUFLEN,0,(struct sockaddr *)&servaddr_1, sizeof(servaddr_1));

        if(bytes == -1){
              printf("\n Writing error..");
        }

      //  printf("\n Offset size sent is: %s", send_buffer);

   /*  bzero(send_buffer,BUFLEN);
     strcpy(send_buffer,"ACK");
     bytes =  sendto(client_socket, send_buffer, BUFLEN,0,(struct sockaddr *)&servaddr, sizeof(servaddr));

        if(bytes == -1){
              printf("\n Writing error..");
            }  */

        // Receiving an ACK for the Offset_Size
            bzero(recv_buffer,BUFLEN);
            FD_ZERO(&readfd_1);
            FD_SET(client_socket_1,&readfd_1);
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            select (maxfd, &readfd_1, NULL, NULL, &tv);

            if(FD_ISSET(client_socket_1,&readfd_1)){
            n = recvfrom(client_socket_1, recv_buffer, BUFLEN, 0, (struct sockaddr *) &tempaddr, &addrlen_1);
           // printf("\n The number of bytes received are: %d",n);
           // printf("\n OFFSET SIZE: The recv_buffer should have ACK but has : %s",recv_buffer);
             }

            

            

        // SENDING CHUNK SIZE
            
        bzero(send_buffer,BUFLEN);
        sprintf(send_buffer, "%d", chunk_size);

        bytes = sendto(client_socket_1, send_buffer, BUFLEN,0,(struct sockaddr *)&servaddr_1, sizeof(servaddr_1)); 

        if(bytes == -1){
              printf("\n Writing error..");
        }

       // printf("\n Chunk Size sent is : %s", send_buffer);

      /*  bzero(send_buffer,BUFLEN);
        strcpy(send_buffer,"ACK");
        bytes = sendto(client_socket, send_buffer, BUFLEN,0,(struct sockaddr *)&servaddr, sizeof(servaddr));

        if(bytes == -1){
              printf("\n Writing error..");
        } */

       
           bzero(recv_buffer,BUFLEN);

     
          

           output_file = fopen("Output_File.txt","w+");
           if(NULL == output_file)
           {
             printf("Error opening file");
            // return 1;
           }

             
           //  pthread_mutex_lock(&lock);
      
            // Implementing fseek in the client thread


             err = fseek(output_file, offset_size, SEEK_SET);
               if(err < 0){
                printf("\n There was an error in traversing the file");
                }


            FD_ZERO(&readfd_1);
            FD_SET(client_socket_1,&readfd_1);
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            select (maxfd, &readfd_1, NULL, NULL, &tv);

            if(FD_ISSET(client_socket_1,&readfd_1)){
              while (1){
              n = recvfrom(client_socket_1, recv_buffer, BUFLEN, 0, (struct sockaddr *)&tempaddr, &addrlen_1);
              
             // printf("\n The data being written by thread is: %s",recv_buffer);
              if(strstr(recv_buffer, "ENDOFFILE") != NULL){
                break;
               }

              fwrite(recv_buffer, 1,bytes,output_file);
               
             }
         }

            
              
           bzero(recv_buffer,BUFLEN);

            printf("\n The File has been written by thread %d \n",id+1);

            //This piece of code is for removing extra characters from the file
            
           /* charsToDelete = 310;
            fseeko(output_file,-charsToDelete,SEEK_END);
            position = ftello(output_file);
            ftruncate(fileno(output_file), position); */

            fclose(output_file);

          //  pthread_mutex_unlock(&lock); // Unlocking the mutex

           // pthread_exit((void*) struct_vals);
           

}



int main(int argc, char **argv)
{

  int         client_socket, n;
  char        recv_buffer[BUFLEN];
  char        tok[BUFLEN];
  char        send_buffer[BUFLEN];
  char        buffer[BUFLEN];
  
 // char        File_Name[20];
  
  thread_struct   thread_array[10];
  int         port_number;
  int         bytes;
  char        check[BUFLEN];
  FILE        *input_file;
  int         choice = 0;
  int         count = 0;
  int         ch = 1;
  char        *token; // For Splitting up the input line from server-input file
  int         file_size;
  FILE        *output_file;
  int         file_exist;
  struct      sockaddr_in servaddr;
  struct      sockaddr_in clientaddr;
  int         addrlen;
  fd_set      readfd;
  struct      timeval tv;
  int         maxfd;
  
  
  
  
  int         chunk_extra;
  int         i;
  pthread_t   threads[10];
  int         th_ret[10];
  int         iterations;
  int         msec = 0, trigger = 1; /* 10ms */
  clock_t     before = clock();

  addrlen = sizeof(servaddr);
  

  if(argc != 3){
    printf("\n The server-input file has not been mentioned and You Have not entered the number of Servers you wish to use.. ");
    exit(0);
  }


    number_of_servers = atoi(argv[2]);
  
    // Here we are opening the server-info file and storing the data into arrays

    char const* const filename = argv[1];
    input_file = fopen(filename,"r");

    while(fgets(recv_buffer,sizeof(recv_buffer),input_file)){

        token = strtok(recv_buffer," ");

        while( token != NULL ) 
        {
           if(ch == 1){
           strcpy(IP_address[max],token);
           token = strtok(NULL," ");
           }
           
           if(ch == 2){
            strcpy(Port_Number[max],token);
            token = strtok(NULL," ");
           }
          
          if(ch == 1){
            ch = 2;
          }
          else{
            ch = 1;
          }
       }
      
      
      max++;


    }

    fclose(input_file);
    count = max;

    // Checking if the number of servers specified are greater than the number of servers available

    if(number_of_servers > count){
      printf("\n The number of servers specified are greater than the number of servers available");
      printf("\n The maxinum number of servers are: %d",count);
      printf("\n The Program will use the maximum number of servers available to copy the file");
    }
    else{
      count = number_of_servers;
    }


    // Till Now we have got the IP addresses of the servers and the their port numbers and have stored them in an array

    // Creating our UDP Socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

     maxfd = client_socket + 1;

    if(client_socket == -1){
       printf("\n There was an error in creating the socket..");
      }

    port_number = atoi(Port_Number[0]);       //Assigning the port number

    memset((char *) &clientaddr, 0, sizeof(clientaddr)); 
    clientaddr.sin_family      = AF_INET;
    clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientaddr.sin_port        = htons(0);
    
   // Binding the Addresses with the Client Socket Descriptor
    Bind(client_socket, (SA *) &clientaddr, sizeof(clientaddr));

  // Assigning the values for Server's Address and port number
    memset((char *) &servaddr, 0, sizeof(servaddr)); 
   // bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port_number);                         //Converting the port number to the network order
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    if (inet_pton(AF_INET, IP_address[0], &servaddr.sin_addr) <= 0)  // Converting the IP address specified from string to network order
    err_quit("inet_pton error for IP: %s", IP_address[0]);

   

    printf("\n The Client is Set up to transfer..");

   /* if (connect(client_socket, (SA *) &servaddr, sizeof(servaddr)) < 0){
    err_sys("Connection Establishment Error");
    }
    else{
      printf("\n Client is UP and RUNNING !!  ");
    } */
    
   // printf("\n About to send first token.."); 
    bzero(tok,BUFLEN); 
    strcpy(tok,"FILE-CHECK"); // The token which will notify server that we are asking for size of file

    // Using the sendto function to send the token

    

   //  do {
         
         bytes = sendto(client_socket, tok, BUFLEN,0,(struct sockaddr *) &servaddr, sizeof(servaddr));
          if(bytes == -1){
              printf("\n There was an error in sending the token..");
           }
          else{
           //  printf("\n Sent the first token.. and waiting for ACK.. ");
            // fflush(NULL);
          }

       /*  clock_t difference = clock() - before;
         msec = difference * 1000 / CLOCKS_PER_SEC;
         iterations++;
        } while ( msec < trigger );

        printf("\n Out of the while loop. ");*/
    bzero(recv_buffer, BUFLEN);

    FD_ZERO(&readfd);
    FD_SET(client_socket,&readfd);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    select (maxfd, &readfd, NULL, NULL, &tv);

    if(FD_ISSET(client_socket,&readfd)){
            n = recvfrom(client_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *) &clientaddr, &addrlen);
          }


        
   // printf("\n The Receive Buffer has: %s",recv_buffer);
   // printf("\n GOT THE FIRST ACK .. ");


    
    
    printf("\n Please enter the name of the file you wish to retrieve: ");
    bzero(send_buffer,BUFLEN);
    fgets(send_buffer,  sizeof(send_buffer), stdin); // Getting the name of the File from the user
    
    
    send_buffer[strcspn(send_buffer,"\n")] = '\0';   // To Remove the /n in fgets

    strcpy(name,send_buffer);

    bytes = sendto(client_socket, send_buffer, BUFLEN,0,(struct sockaddr *) &servaddr, sizeof(servaddr));

        if(bytes == -1){
              printf("\n Writing error..");
        }

     bzero(send_buffer,BUFLEN);

     FD_ZERO(&readfd);
     FD_SET(client_socket,&readfd);
     tv.tv_sec = 1;
     tv.tv_usec = 0;

     select (maxfd, &readfd, NULL, NULL, &tv);

    if(FD_ISSET(client_socket,&readfd)){
            n = recvfrom(client_socket, recv_buffer, BUFLEN, 0, (struct sockaddr *) &clientaddr, &addrlen);
          }

        if(strstr(recv_buffer, "NO FILE") != NULL){
          printf("\n The File does not exist on the server.. \n");
          file_exist = 0;
        }
        else{
          file_size = atoi(recv_buffer);
          file_size = file_size;
          printf("\n The size of the file is: %d \n",file_size);
          file_exist = 1;
        }

     if(file_exist == 1){
     
     printf("\n The count of servers is: %d",count);
     offset_size[0] = 0;
     for( i=0;i<count;i++){
        chunk_size[i] = chunk_size[i] + file_size/count;
     }

     chunk_extra = file_size%count;
     chunk_size[i-1] = chunk_size[i-1] + chunk_extra;

     for(i=1;i<count;i++){
      offset_size[i] = offset_size[i-1] + chunk_size[i-1];
     }

   //  printf("\n ALL is well till we start the threads.. ");


     
     // Sending the Offset_Size to the Server

     for(i=0;i<count;i++){
      thread_array[i].thread_id = i;
      thread_array[i].offset_size = offset_size[i];
      thread_array[i].chunk_size = chunk_size[i];
     } 


      for(i=0;i<count;i++){
      //  printf("\n The id for thread is: %d", thread_array[i].thread_id);
      //  printf("\n The size of file for thread %d is: %d",thread_array[i].thread_id,thread_array[i].chunk_size);
      //  printf("\n The offset of file for thread %d is: %d",thread_array[i].thread_id,thread_array[i].offset_size);
      //  printf("\n The chunk size %d is: %d",count,chunk_size[i]);
      //  printf("\n The offset size %d is: %d",count,offset_size[i]);
     }

     for(i=0;i<count;i++){
      th_ret[i] = pthread_create(&threads[i], NULL, client_thread, (void *) &thread_array[i]);
      if(th_ret[i]){
          printf("\n There was an error in creating thread: %d",i+1);
      }

     }

     for(i=0;i<count;i++){
      if(pthread_join(threads[i], NULL) != 0) {

       fprintf(stderr, "Error joining thread\n");
      //return 2;

         }


     }


     printf("\n The threads have returned safely.. \n");
     
    
     }

     else{
       printf("\n Since File does not exist.. Closing the Program.. ");
     }
     
     

     

  











    return 0;
}






