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


// Global Variables for Storing IP Addresses and Ports of the server
char        IP_address[10][100];
char        Port_Number[10][100];
struct      sockaddr_in servaddr;
int         offset_size[10];
int         chunk_size[10];
char        name[256];
int         number_of_servers;
int         max = 0;


void *client_thread(void *struct_vals){

  char      send_buffer[256];
  char      recv_buffer[256];
  char      tok[256];
  char      buffer[256];
  int       offset_size;
  int       chunk_size;
  int       id;
  int       err;
  int       bytes;
  int       client_socket, n;
  int       port_number;
  FILE      *output_file;
  off_t      position;
  int        charsToDelete;




  thread_struct *thread_data;
  thread_data = (thread_struct *)struct_vals;

  offset_size = thread_data->offset_size;
  chunk_size = thread_data->chunk_size;
  id = thread_data->thread_id;

    printf("\n The id is: %d", id);
    printf("\n The offset_size is: %d", offset_size);
    printf("\n The chunk size is: %d", chunk_size);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(client_socket == -1){
       printf("\n There was an error in creating the socket..");
      }

    port_number = atoi(Port_Number[id]);                               //Assigning the port number
     
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port_number);                         //Converting the port number to the network order

    if (inet_pton(AF_INET, IP_address[id], &servaddr.sin_addr) <= 0)  // Converting the IP address specified from string to network order
    err_quit("inet_pton error for IP: %s", IP_address[id]);

    if (connect(client_socket, (SA *) &servaddr, sizeof(servaddr)) < 0){
    err_sys("Connection Establishment Error");
    }
    else{
      printf("\n Client_Thread %d is UP and RUNNING !!  ",id+1);
    }


    bzero(tok,256); 
    strcpy(tok,"GET-DATA"); // The token which will notify server that we are asking for size of file


        // Sending the Token for Retreiving data
        bytes =  Write(client_socket, tok,strlen(tok));

        if(bytes == -1){
              printf("\n Writing error in the token..");
        }
        // Waiting for an ACK for the token
        while( (n = read(client_socket, buffer, 256)) != 0){
            if(strstr(buffer, "ACK") != NULL){
                 break;
            }
          }

        // Sending the file name to the server
        bzero(send_buffer,256);
        strcpy(send_buffer,name);
        bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

        if(bytes == -1){
              printf("\n Writing error..");
        }

        // Waiting for an ACK for the file name
        while( (n = read(client_socket, buffer, 256)) != 0){
            if(strstr(buffer, "ACK") != NULL){
                 break;
            }
          }

  
     // Sending the offset size
     bzero(send_buffer,256);
     sprintf(send_buffer, "%d", offset_size);
     bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

        if(bytes == -1){
              printf("\n Writing error..");
        }

     bzero(send_buffer,256);
     strcpy(send_buffer,"ACK");
     bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

        if(bytes == -1){
              printf("\n Writing error..");
        }

        // Receiving an ACK for the Offset_Size
        while( (n = read(client_socket, buffer, 256)) != 0){
            if(strstr(buffer, "ACK") != NULL){
                 break;
            }
            
          }

        

      // Sending the Chunk_Size to the server

        bzero(send_buffer,256);
        sprintf(send_buffer, "%d", chunk_size);
        bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

        if(bytes == -1){
              printf("\n Writing error..");
        }

        bzero(send_buffer,256);
        strcpy(send_buffer,"ACK");
        bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

        if(bytes == -1){
              printf("\n Writing error..");
        }

       
           bzero(recv_buffer,256);

     
     
           output_file = fopen("Output_File.txt","w+");
           if(NULL == output_file)
           {
             printf("Error opening file");
            // return 1;
           }
       
      
            // Implementing fseek in the client thread

             err = fseek(output_file, offset_size, SEEK_SET);
               if(err < 0){
                printf("\n There was an error in traversing the file");
                }

            while((bytes = read(client_socket, recv_buffer, 256)) > 0){ 

               if(strstr(recv_buffer, "ENDOFFILE") != NULL ) {
                
              // This is for removing the END OF FILE part from our recv_buffer
               char *strlocation;
               strlocation = strstr(recv_buffer, "ENDOFFILE");
               strcpy (strlocation, strlocation + strlen ("ENDOFFILE")); 

                 
                // recvBuff[n] = 0;
               fwrite(recv_buffer, 1,bytes,output_file);
                
               break;
            }
            else{
              fwrite(recv_buffer, 1, bytes,output_file);
            }
           
           }

            bzero(recv_buffer,256);

            printf("\n The File has been written by thread %d \n",id+1);

            /*This piece of code is for removing extra characters from the file
            
            charsToDelete = 9;
            fseeko(output_file,-charsToDelete,SEEK_END);
            position = ftello(output_file);
            ftruncate(fileno(output_file), position);

            fclose(output_file);

           // pthread_exit((void*) struct_vals);
           */

}



int main(int argc, char **argv)
{

	int					client_socket, n;
	char				recv_buffer[256];
  char        tok[256];
	char 				send_buffer[256];
  char        buffer[256];
	
 // char        File_Name[20];
	
  thread_struct   thread_array[10];
	int         port_number;
	int 				bytes;
	char 				check[256];
	FILE 				*input_file;
	int 				choice = 0;
	int 				count = 0;
	int 				ch = 1;
	char        *token; // For Splitting up the input line from server-input file
  int         file_size;
  FILE        *output_file;
  int         file_exist;
  
  
  int         chunk_extra;
  int         i;
  pthread_t   threads[10];
  int         th_ret[10];
	

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


    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(client_socket == -1){
       printf("\n There was an error in creating the socket..");
      }

    port_number = atoi(Port_Number[0]);                               //Assigning the port number
     
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port_number);                         //Converting the port number to the network order

    if (inet_pton(AF_INET, IP_address[0], &servaddr.sin_addr) <= 0)  // Converting the IP address specified from string to network order
    err_quit("inet_pton error for IP: %s", argv[1]);

    if (connect(client_socket, (SA *) &servaddr, sizeof(servaddr)) < 0){
    err_sys("Connection Establishment Error");
    }
    else{
      printf("\n Client is UP and RUNNING !!  ");
    }
     
    bzero(tok,256); 
    strcpy(tok,"FILE-CHECK"); // The token which will notify server that we are asking for size of file

    bytes =  Write(client_socket, tok,strlen(tok));

        if(bytes == -1){
              printf("\n Writing error in the token..");
        }

    while( (n = read(client_socket, buffer, 256)) != 0){
            if(strstr(buffer, "ACK") != NULL){
                 break;
            }
          }
    

    printf("\n Please enter the name of the file you wish to retrieve: ");
    bzero(send_buffer,256);
    fgets(send_buffer,  sizeof(send_buffer), stdin); // Getting the name of the File from the user
    

    send_buffer[strcspn(send_buffer,"\n")] = '\0';   // To Remove the /n in fgets

    strcpy(name,send_buffer);

    bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

        if(bytes == -1){
              printf("\n Writing error..");
        }

     bzero(send_buffer,256);

     if( (n = read(client_socket, recv_buffer, 256)) != 0){
        if(strstr(recv_buffer, "NO FILE") != NULL){
          printf("\n The File does not exist on the server.. \n");
          file_exist = 0;
        }
        else{
          file_size = atoi(recv_buffer);
          file_size = file_size/10;
          printf("\n The size of the file is: %d \n",file_size);
          file_exist = 1;


        }

     }
     
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
     
    
     



     

  











    return 0;
}




