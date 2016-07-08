/*
*   File:       myclient.c
*
*   Purpose:    This is a skeleton file for a client.
*/

#include "myunp.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>   




void *client_thread(void *struct_vals){


char      send_buffer[50960];
char      recv_buffer[4096];
char      temp[4096];
FILE      *inputfile;            // File Pointer for Forbidden Sites
FILE      *logfile;              // File Pointer for Log File
char      build_buffer[4096];
struct    sockaddr_in servaddr;
char      request[50960];
char      tags[10][100];
char      forbidden[10][100];    // String Array for storing forbidden websites
char      temp_tag[100];
int       client_sock;
int       server_sock;           // Descriptor for Server Socket 
char      req[50];               // Request of the Browser 
char      type[50];              // TYPE OF HTTP Request
int       web_port_number = 80;
int       n = 1;
int       i;
int       option = 0;
int       count=0;
int       bytes;
int       len;
int       chunks;
int       tag_count=0;
char      *token;                // For Splitting up the input line from server-input file
char      host_name[100];        // Will contain the host URI which we want to access
struct    hostent *he;
struct    in_addr **addr_list;
char      host_IP[100];              // Will contain the IP address of the host name
time_t    timer;



printf("\n Have entered the thread.. \n");

    // ***** This Code is for Getting the forbidden URI'S 

    bzero(recv_buffer,4096);

    inputfile = fopen("Forbidden.txt","r");

    while(fgets(recv_buffer,sizeof(recv_buffer),inputfile)){

        token = strtok(recv_buffer,"\n");

        while( token != NULL ) 
        {
           strcpy(forbidden[count],token);
           token = strtok(NULL,"\n");
        } 
        count++;
      }

    //printf("\n The Forbidden Sites are: ");
    
    
    // ***** TILL HERE 


      logfile = fopen("logfile.txt", "a");
      if (logfile == NULL)
        {
          printf("Error opening file!\n");
          exit(1); 
        } 

        time(&timer);

        fprintf(logfile, " \n NEW ENTRY !! \n\n");
        fprintf(logfile, " %s\n", ctime(&timer));

    client_sock = *(int *)struct_vals;

    printf("\n The socket received from main thread is: %d \n",client_sock);  
    
   // bzero(request,4096);
    bzero(build_buffer,4096);

    printf("\n Waiting for message from browser.. \n");
    
        
        bzero(recv_buffer,4096);
        n = read(client_sock, recv_buffer, 4096);
        printf("\n Just got something from browser.. \n");
        //strcat(request,recv_buffer);
    
    
    printf("\n The recv_buffer has the data from server..\n");
    printf("\n The HTTP request from Browser is: \n");
    printf("\n %s",recv_buffer);

    strcpy(temp,recv_buffer);
    
    // ******** This code is for parsing the data obtained from browser
    token = strtok(temp,"\r\n");
    while( token != NULL ) {
           strcpy(tags[tag_count],token);
           token = strtok(NULL,"\r\n");
           tag_count++;
    }
    
    // ******* This code is for getting the URI specified by the browser

    for(i=0; i<tag_count;i++){
    // printf("\n The tag %d is: %s",i+1,tags[i]);
      
      if(strstr(tags[i],"Host:")!= NULL){
        strcpy(temp_tag,tags[i]);
      }

    }

    token = strtok(temp_tag,": ");
    token = strtok(NULL,": ");
    strcpy(host_name,token);

    // strcpy(host_name,token);

    // *******  TILL HERE 

    // *******  THIS CODE IS FOR CHECKING IF A FORBIDDEN WEBSITE IS BEING REQUESTED

    for(i=0;i<count;i++){
       if(strstr(host_name,forbidden[i]) != NULL){
        option = 1;
       }
     }

    // ****** TILL HERE


    // ***** THIS CODE IS FOR CHEKING THE HEADERS 

     token = strtok(tags[0]," ");
     strcpy(req,token);
     token = strtok(NULL," ");
     token = strtok(NULL," ");
     strcpy(type,token);

     printf("\n The request type is: %s",req);
     printf("\n The HTTP type is: %s", type);

     fprintf(logfile, " %s\n", req);
     fprintf(logfile, " %s\n", type);
     fprintf(logfile, " %s\n", tags[1]);


     if((strstr(req,"GET")==NULL) && (strstr(req,"HEAD")==NULL)){
      option = 2;
     }


    // ***** TILL HERE  
   

    // ***** THIS IS THE CONDITION WHEN HEADERS AND THE URI IS CORRECT   

    

     
    
    len = strlen(host_name);
    printf("\n The host name specified by bowser is: %s \n",host_name);
   // printf("\n The length of host name has: %d \n", len);

    // The following code is to get the IP address for the URI specified by the user

    if ( (he = gethostbyname( host_name ) ) == NULL) 
     {
        // get the host info
        herror("gethostbyname");
        
     }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
      {
        //Return the first one;
        strcpy(host_IP , inet_ntoa(*addr_list[i]) );
        
      }

     // printf("\n The IP address of %s is : %s",host_name,host_IP); 
      fprintf(logfile, " %s\n", host_name);
      fprintf(logfile, " %s\n", host_IP);
    //bzero(recv_buffer,4096);
    //bzero(request,4096);


    // Till Now we have processed data sent by browser 

    // Creating the socket to talk to Web Server

     if(option == 0){ 

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(web_port_number);                         // Converting the port number to the network order

    if (inet_pton(AF_INET, host_IP, &servaddr.sin_addr) <= 0)             // Converting the IP address specified from string to network order
    err_quit("inet_pton error for IP: %s", host_IP);


    
     // Connecting the socket to web server
      if (connect(server_sock, (SA *) &servaddr, sizeof(servaddr)) < 0){
           err_sys("Connection Establishment Error");
      }
     else{
           printf("\n The Server socket is set up.. ");
      }
      
      // Sending the data to the webserver
      bzero(send_buffer,50960);
      printf("\n The data to be sent to web server is: %s", recv_buffer);
      strcpy(send_buffer,recv_buffer);
      bytes =  Write(server_sock, send_buffer,strlen(send_buffer));

      if(bytes == -1){
        printf("\n There was an error in sending data to the web server.. \n");
      }
      else{
        printf("\n The number of bytes written are : %d\n",bytes);
      }
      
      // Receiving the data from the web server
      bytes = 1;
      bzero(recv_buffer,4096);
      bzero(request,50960);

             
           //  bytes = read(server_sock, request, 50960);
             while((bytes = read(server_sock, recv_buffer, 4096))>0){
                
               if(bytes > 0) {
                 // strcat(request,recv_buffer);
                printf("\n The data received from web server is: %s \n",recv_buffer);
               }
                                

             } 

             close(server_sock);

            // printf("\n The web server returns: %s\n",recv_buffer);
           /*  while(1){

             ioctl(server_sock, FIONREAD, &bytes);
             printf("\n The number of bytes to be received from web server is: %d\n",bytes);

             if(bytes > 4096){
                chunks = bytes/4096;
                  for(i=0;i<chunks;i++){
                        bzero(recv_buffer,4096);
                        bytes = read(server_sock, recv_buffer, 4096);
                        strcat(request,recv_buffer);
                    }
                  }
                else if(bytes == 0) {
                  break;
               }
               else{
                 bytes = read(server_sock, request, bytes);
               }
         }*/
        i = 1;
       /* while(bytes > 0){
             
             bzero(recv_buffer,4096);
             bytes = read(server_sock, recv_buffer, 4096);
             printf("\n The data received from web server is: %s",recv_buffer);
             if(bytes == 0){
                break;
             }
             else if(bytes < 0){
              printf("\n Error in receiving bytes from web server..\n");
              break;
             }
             
             
             strcat(request,recv_buffer);
             
             
              
      }*/

         printf("\n Hello.. We are out of loop..\n");
         printf("\n The Data received from the web server is: %s\n",recv_buffer);

         // Sending the data from Web Server to the Browser
         bzero(send_buffer,50960);
         strcpy(send_buffer,recv_buffer);

         bytes =  Write(client_sock, send_buffer,strlen(send_buffer));

         fprintf(logfile, "  SUCCESSFUL TRANSMISSION\n");
         fclose(logfile);


       }

       // **** THIS CONDITION IS FOR FORBIDDEN WEBSITES

       if(option == 1){
        bzero(send_buffer,50960);
        strcpy(send_buffer,"HTTP/1.1 403 Forbidden \r\n\r\n Sorry But this Site is FORBIDDEN For Access !! \n");
         bytes =  Write(client_sock, send_buffer,strlen(send_buffer));
         fprintf(logfile, "  FILTERED REQUEST !!\n");
         fprintf(logfile, " ERROR CODE: 403\n");
         fclose(logfile);

       }
      //  **** THIS CONDITION IS FOR BAD REQUESTS
       if(option == 2){
        bzero(send_buffer,50960);
        strcpy(send_buffer,"HTTP/1.1 501 Not Implemented \r\n\r\n Sorry But this Request is Not Implemented by the proxy !! \n");
         bytes =  Write(client_sock, send_buffer,strlen(send_buffer));
         fprintf(logfile, "FILTERED REQUEST !! \n");
         fprintf(logfile, " ERROR CODE: 501\n");
         fclose(logfile);

       }






}  // The end of thread   



 


int main(int argc, char **argv){

int n;
int i;
pthread_t   threads[10];
int         th_ret[10];
int         proxysocket;            // Socket Descriptor for proxy server 
struct      sockaddr_in servaddr;   // Structure for server address
int         port_number;
int         connection_queue = 10;     // Number of connection acceptable
int         client_socket[10];      // Socket Descriptor for the connections 
int         count = 0;


//Creating the proxy socket 
proxysocket = Socket(AF_INET, SOCK_STREAM, 0); 

if(proxysocket == -1){
    printf("\n There was an error in creating the socket..");
  }

else{
  printf("\n The proxy socket is up and running..%d\n", proxysocket);
  }


bzero(&servaddr, sizeof(servaddr));
port_number = 1560;                            // Assigning the Port Number for the Proxy Server

servaddr.sin_family      = AF_INET;
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
servaddr.sin_port        = htons(port_number);
    
// Binding the Addresses with the Socket Descriptor
n = Bind(proxysocket, (SA *) &servaddr, sizeof(servaddr));
   
    if(n == -1){
      printf("\n There was an error in Binding the socket..");
    }
    else{
      printf("\n Binding is done.. \n");
    }


// Listening for the connections on the specified port
    
    Listen(proxysocket, connection_queue);

    
// Entring the infinite loop
for( ; ; ){
     
     printf("\n Entered the infinite loop \n");

     if( (client_socket[count] = Accept(proxysocket, (SA *) NULL, NULL)) == -1){       // The client socket is created which will be passed to threads
        printf("\n The connection at client socket had an error");           
      }
      printf("\n Accepted the connection from browser.. with socket as : %d\n",client_socket[count]);

      th_ret[count] = pthread_create(&threads[count], NULL, client_thread, (void *) &client_socket[count]);

      if(th_ret[count]){
          printf("\n There was an error in creating thread: %d",count+1);
      }
      else{
        printf("\n The thread is created.. with count %d \n",count);
      }

      count++;


      for(i=0;i<=count;i++){
      if(pthread_join(threads[i], NULL) != 0) {

            fprintf(stderr, "Error joining thread\n");
            //return 2;
          }
          else{
            printf("\n Thread returned with count : %d", i);
            close(client_socket[i]);

            count--;
          }
       }

       
       


     }  // Infinite Loop



return 0;

} // Main Function