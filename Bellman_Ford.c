
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "myunp.h"
#include "mytimer.h"



// Declaring the Timers 
mytimer_t timerA = TIMER_INIT;
mytimer_t timerB = TIMER_INIT;
mytimer_t timerC = TIMER_INIT;
mytimer_t timerD = TIMER_INIT;


// *** Defining a Structure For printing the table ***

struct Node{
  char  Node_Name[50];
  int   Distance; 
  char  Source[50];

};


struct Node nodes[20];  

struct  sockaddr_in nodeaddr;
struct  sockaddr_in neighboraddr[10];
struct  sockaddr_in clientaddr;

int   Node_Socket;            // The Socket for the Node
int   bytes_sent;
int   bytes_received;
int   metric[15];
char  names[10][15];
char  message_buffer[4096];
char  read_buffer[4096];
char  write_buffer[4096];
char  Node_IP[50];
char  Node_Port[50];
char  Neighbors_Port[50][15];
char  Neighbors_IP[50][15];
int   No_of_Neighbors;
int   No_of_nodes = 0;
int   addrlen;
int   recv_metric[15];
char  Neighbors[10][15];
time_t  timer;
int   timer_c[3];

int   i,j;


// **** THIS IS THE FUNCTION TO PRINT OUT THE TABLE ***

void print_table(struct Node in_node){

  
  printf("\n");
  printf("Node \t %s",in_node.Node_Name);
  if(in_node.Distance >= 17){
    printf("\t Distance \t %c ",'X');
   }
  else{
    printf("\t Distance \t %d ",in_node.Distance);
  }
  printf("\n");
}

// ********* CALL BACK FUNCTIONS FOR OUR TIME-OUT TIMERS ************

void remove_B(time_t now){
  int y;
  timer_c[0] = 0;
  for(y=0;y<No_of_nodes;y++){
    
      if(strstr(nodes[y].Source,Neighbors[0]) != NULL){
      nodes[y].Distance = 17;
    }
  }
}

void remove_C(time_t now){
  int y;
  timer_c[1] = 0;
  for(y=0;y<No_of_nodes;y++){
    
     if(strstr(nodes[y].Source,Neighbors[1]) != NULL){
      nodes[y].Distance = 17;
    }
  }
}

void remove_D(time_t now){
  int y;
  timer_c[2] = 0;
  for(y=0;y<No_of_nodes;y++){
    
     if(strstr(nodes[y].Source,Neighbors[3]) != NULL){
      nodes[y].Distance = 17;
    }
  }
}

// ********** TILL HERE ****************


// ************ OUR THREAD FUNCTION **************
void *RIP_Receive(void *number){
        struct  sockaddr_in tempaddr;
        int *number_of_nodes = (int *)number;
        int listening_socket;
        int port;
        fd_set readfd;
        struct timeval tv_1;
        int maxfd;
        int count;
        char *temp;
        char  port_buffer[20];
        char *token;  
        char   index[20];
        int   dtn;
        
        int   check = 1;

        int x;
        bzero(port_buffer,20);

        listening_socket = Socket(AF_INET, SOCK_DGRAM, 0);

         maxfd = listening_socket+1;   // *******  IMPORTANT PART OF SELECT **********
         bzero(read_buffer,4096);
         addrlen = sizeof(clientaddr);


        memset((char *) &tempaddr, 0, sizeof(tempaddr)); 
        tempaddr.sin_family      = AF_INET;
        tempaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        tempaddr.sin_port        = htons(atoi(Node_Port));
    
        // Binding the Addresses with the Socket Descriptor
        Bind(listening_socket, (SA *) &tempaddr, sizeof(tempaddr));

        while(1){

          FD_ZERO(&readfd);
          FD_SET(listening_socket,&readfd);
          tv_1.tv_sec = 10;
          tv_1.tv_usec = 0;

          select(maxfd, &readfd, NULL, NULL, &tv_1);
          //printf("\n The Select is not blocking.. \n");

          if(FD_ISSET(listening_socket,&readfd)){
             bytes_received = recvfrom(listening_socket, read_buffer, 4096, 0, (struct sockaddr *) &clientaddr, &addrlen);
          }

          if(bytes_received == -1){
            printf("\n There was an error in receiving the message.. \n");
          }
          else{
            printf("\n Just received the message: %s\n",read_buffer);
          }

          port = ntohs(clientaddr.sin_port);
          port = port -1;
          sprintf(port_buffer,"%d",port);
          printf("\n The Port where the message came from is: %s",port_buffer);

          if(port == -1){
            check = 2;
          }

          if(check == 1){

         // printf("\n The number of neighbors are: %d",No_of_Neighbors);

         // printf("\n The Neighbors and theirs attributes are: ");
          for(x=0;x<3;x++){
              timer_c[x] = 0;

          }

          for(x=0;x<No_of_Neighbors;x++){

            if(strstr(Neighbors_Port[x],port_buffer) != NULL){
              printf("\n The neighbor sending the message is: %s",Neighbors[x]);
              strcpy(index,Neighbors[x]);
              timer_c[x] = 1;
            }
          }

          if(timer_c[0] == 1){
            timer_start(&timerB, 20, remove_B);
          }

          if(timer_c[1] == 1){
            timer_start(&timerC, 20, remove_C);
          }

          if(timer_c[2] == 1){
            timer_start(&timerD, 20, remove_D);
          }
           

          
          //printf("\n The name of the neighbor sending the packet is: %s",index);

          for(x=0;x<No_of_nodes;x++){
            if(strstr(nodes[x].Node_Name,index) != NULL){
                 dtn = nodes[x].Distance;
            }
          }
          
          printf("\n The distance to the neighbor sending packet is: %d",dtn);

          
          count = 0;
          token = strtok(read_buffer," ");
          token = strtok(NULL," ");

            while( token != NULL ) 
             {
                recv_metric[count] = atoi(token);
                //printf("\n just copied %d",recv_metric[count]);
                count++; 
                token = strtok(NULL," ");

             }

            /* for(x=0;x<count;x++){
              printf("\n The distance to node %s is:%d",nodes[x].Node_Name,recv_metric[x]);
             } */
            
           // printf("\n The number of nodes in the topology are: %d\n",No_of_nodes);


    
// ***************** HERE WE HAVE APPLIED OUR ALGORITHM FOR IMPLEMENTING BELLMAN FORD ***************

            
            for(x=0;x<No_of_nodes;x++){
              if(strstr(nodes[x].Source,index) != NULL){
                nodes[x].Distance = (dtn + recv_metric[x]);
              }

              else if(nodes[x].Distance > (dtn + recv_metric[x])) {
                   nodes[x].Distance = (dtn + recv_metric[x]);
                   strcpy(nodes[x].Source,index);
               }
            }

            
// *****************  TILL HERE ************************          
          
          }

          time(&timer);

          printf("\n The update happened at %s",ctime(&timer));

          for(x=0;x<No_of_nodes;x++){
              print_table(nodes[x]);
          }

          check = 1;
          

        }


}


// ************** FUNCTION TO SEND THE UPDATES TO ALL THE NEIGHBORS ***************
void update_time(time_t now){
        char header[50];

       for(i=0;i<No_of_Neighbors;i++){
          bzero(header,50);
          strcpy(header,message_buffer);
          bytes_sent = sendto(Node_Socket, header, strlen(header),0,(struct sockaddr *) &neighboraddr[i], sizeof(neighboraddr[i]));
          if(bytes_sent == -1){
              printf("\n There was an error in sending the message buffer..");
           }
           else{
            //printf("\n Sent the message_buffer %s\n",message_buffer);
           }
        }

}


// ************** TILL HERE **********************





int main(int argc, char **argv){

char 	name[15];
char	Node_Name[15];
int 	Port_Number;  
pthread_t   thread; 

FILE 	*configuration_file;
FILE 	*neighbor_file;
char 	*token;


char  message[50];

char  Source[10][15];
char	Destination[10][15];

int   thread_return;



char  temp_node[50];
char  temp_IP[50];
char  temp_port[50];
char  temp_buffer[50];



int   check_s = 0;
int   check_d = 0;
int   check=0;
int 	Distance_Metric[15];
char  temp_distance[10][15];
int 	distance[15];
int 	count=0;
int 	temp_count = 0;


int   pos;

fd_set  readfd,rset;
struct  timeval tv,tv_1;
int   maxfd;

     if(argc < 3){
		printf("\n The Node Name and the Port Number have not been mentioned properly..");
		exit(0);
     }

     strcpy(name,argv[1]);         // Copying the node name specified into the Array
     Port_Number = atoi(argv[2]);  // Copying the port number to the datatype
     strcpy(Node_Name,name);

     printf("\n The node name is: %s",name);
     //printf("\n The port number of the node is: %d \n",Port_Number);



// ****** THIS SECTION IS FOR FIGURING OUT THE NUMBER OF NODES IN THE TOPOLOGY *******

     configuration_file = fopen("neighbor.config","r");

    // printf("\n The configuration file read: \n");

     while (fgets(read_buffer, sizeof(read_buffer), configuration_file)) {
        
       // printf("\n Inside the while loop.. \n");
     	if(strstr(read_buffer,"#")==NULL){

       //	printf("\n Inside the if condition.. \n");	

     	token = strtok(read_buffer," ");

            while( token != NULL ) 
             {
                strcpy(Source[count],token);
                //printf("\n Just copied %s \n",Source[count]);
                token = strtok(NULL," ");
                strcpy(Destination[count],token);
                //printf("\n Just copied %s \n",Destination[count]);
                token = strtok(NULL," ");
                strcpy(temp_distance[count],token);
                //printf("\n Just copied %s \n",temp_distance[count]);
                distance[count] = atoi(temp_distance[count]);
                token = strtok(NULL," ");

             } 
            
            if(count == 0){
              strcpy(names[No_of_nodes],Source[count]);
              No_of_nodes++;
              strcpy(names[No_of_nodes],Destination[count]);
              No_of_nodes++;
            } 
            else{
              for(i=0;i<No_of_nodes;i++){
                if(strstr(names[i],Source[count])!=NULL){
                  check_s = 1;
                }
                if(strstr(names[i],Destination[count])!= NULL){
                  check_d = 1;
                }
              }
              if(check_s == 0){
                strcpy(names[No_of_nodes],Source[count]);
                No_of_nodes++;
              }
              if(check_d == 0){
                strcpy(names[No_of_nodes],Destination[count]);
                No_of_nodes++;
              }
            }           


            
            count++;
            check_s = 0;
            check_d = 0;

            
            
      
          } // if condition
          //printf("\n Outside the if condition.. \n");
             
       }   // While loop

       //printf("\n Outside the while loop.. \n");

       fclose(configuration_file);


// ********  TILL HERE WE HAVE FIGURED OUT THE NUMBER OF NODES IN THE TOPOLOGY *****





     /*  printf("\n The nodes are: ");
       for(i=0;i<No_of_nodes;i++){
        printf("\n node: %s",names[i]);
       } */

       count = 0;

       for(i=0;i<No_of_nodes;i++){
         if(strstr(Node_Name,names[i])!= NULL){
          pos=i;
          metric[i] = 0;
         // printf("\n The metric at position %d is 0",i);
         }
       }



// **************  THIS PART OF THE CODE IS TO FIGURE OUT THE NEIGHBORS AND TO GET THE DISTANCE METRICS BETWEEN NEIGHBORS *******

       configuration_file = fopen("neighbor.config","r");

    // printf("\n The configuration file reads: \n");

      while (fgets(read_buffer, sizeof(read_buffer), configuration_file)) {
        
       // printf("\n Inside the while loop.. \n");
        if(strstr(read_buffer,"#")==NULL){

       // printf("\n Inside the if condition.. \n");  

         token = strtok(read_buffer," ");

            while( token != NULL ) 
             {
                strcpy(Source[count],token);
                //printf("\n Just copied %s \n",Source[count]);
                token = strtok(NULL," ");
                strcpy(Destination[count],token);
                //printf("\n Just copied %s \n",Destination[count]);
                token = strtok(NULL," ");
                strcpy(temp_distance[count],token);
                //printf("\n Just copied %s \n",temp_distance[count]);
                distance[count] = atoi(temp_distance[count]);
                token = strtok(NULL," ");

             } 

             if(strstr(Node_Name,Source[count]) != NULL){
               strcpy(Neighbors[temp_count],Destination[count]);
               for(i=0;i<No_of_nodes;i++){
                if(strstr(names[i],Neighbors[temp_count])!= NULL){
                  metric[i] = distance[count];
                }
               }
               temp_count++;

             }

             else if(strstr(Node_Name,Destination[count])!= NULL){
                strcpy(Neighbors[temp_count],Source[count]);
                for(i=0;i<No_of_nodes;i++){
                  if(strstr(names[i],Neighbors[temp_count])!=NULL){
                    metric[i] = distance[count];
                  }
                }
                temp_count++;
             }




            count++;
            

            
            
      
          } // if condition
          //printf("\n Outside the if condition.. \n");
             
       }   // While loop

       //printf("\n Outside the while loop.. \n");

       fclose(configuration_file);


// ************** TILL THIS PART WE HAVE FIGURED OUT THE NEIGHBORS AND THEIR METRICS ***************       


       for(i=0;i<No_of_nodes;i++){
          for(j=0;j<temp_count;j++){
             if(strstr(names[i],Neighbors[j])!=NULL){
              check=1;
             }
          }
          if(check == 0){
            if(i != pos){
              metric[i] = 17;
            }
          }
          check=0;
       }

      // printf("\n The node name is: %s",Node_Name);

       // *** This loop is for getting the neighbors of the node

      /* for(i=0;i<count;i++){
       	 if(strstr(Node_Name,Source[i]) != NULL){
           strcpy(Neighbors[temp_count],Destination[i]);
           Distance_Metric[temp_count] = distance[i];
           metric[i] = distance[i];
           temp_count++;
       	  }
          else{
            metric[i] = 17;
          }


       }*/

       

       



// ********** THIS PART OF CODE GETS THE IP AND PORT NUMBERS OF THE NEIGHBORS ***************

       No_of_Neighbors = temp_count;

       temp_count = 0;

       bzero(read_buffer,4096);


       neighbor_file = fopen("node.config","r");

      // printf("\n The configuration file reads: \n");

       while (fgets(read_buffer, sizeof(read_buffer), neighbor_file)) {
        
       // printf("\n Inside the while loop.. \n");
       if(strstr(read_buffer,"#")==NULL){

       // printf("\n Inside the if condition.. \n");  

            token = strtok(read_buffer," ");

            while( token != NULL ) 
             {
                strcpy(temp_node,token);
                //printf("\n Just copied %s \n",Source[count]);
                token = strtok(NULL," ");
                strcpy(temp_IP,token);
                //printf("\n Just copied %s \n",Destination[count]);
                token = strtok(NULL," ");
                strcpy(temp_port,token);
                //printf("\n Just copied %s \n",temp_distance[count]);
                token = strtok(NULL," ");

             } 
                       
            
      
            } // if condition
          //printf("\n Outside the if condition.. \n");

            if(strstr(temp_node,Node_Name) != NULL){
              strcpy(Node_IP,temp_IP);
              strcpy(Node_Port,temp_port);
            }
            else{
              for(i=0;i<No_of_Neighbors;i++){
                if(strstr(temp_node,Neighbors[i])!= NULL){
                  strcpy(Neighbors_IP[i],temp_IP);                  
                  strcpy(Neighbors_Port[i],temp_port);
                 }
              }   // for loop
            }    // else condition
             
          }   // While loop

          //printf("\n Outside the while loop.. \n");

        fclose(neighbor_file);

// *********** TILL HERE WE HAVE GOT THE IP ADDRESS AND THE PORT NUMBERS OF THE NEIGHBORS  *************



        printf("\n The node %s has an IP %s and port %s",Node_Name,Node_IP,Node_Port);

         printf("\n And the details of their neighbours are as follows:");

        for(i=0;i<No_of_Neighbors;i++){
          printf("\n The neighbour %s has an IP %s and a port %s with distance",Neighbors[i],Neighbors_IP[i],Neighbors_Port[i]);
        }

        
        for(i=0;i<No_of_nodes;i++){
          strcpy(nodes[i].Node_Name,names[i]);
          nodes[i].Distance = metric[i]; 
          strcpy(nodes[i].Source,Node_Name);
        }


// **************** This piece of code is for initializing the node socket and the nodeaddr structure *****************
        // Initializing the Socket Descriptor for the Node
        Node_Socket = Socket(AF_INET, SOCK_DGRAM, 0);


        memset((char *) &nodeaddr, 0, sizeof(nodeaddr)); 
        nodeaddr.sin_family      = AF_INET;
        nodeaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        nodeaddr.sin_port        = htons((atoi(Node_Port))+1);
    
        // Binding the Addresses with the Socket Descriptor
        Bind(Node_Socket, (SA *) &nodeaddr, sizeof(nodeaddr));

// ***************** TILL HERE ***********************************************************
        time(&timer);

        printf("\n The update happened at %s",ctime(&timer));

        for(i=0;i<No_of_nodes;i++){
          print_table(nodes[i]);
        }
   


//************  This piece of code is for initializing the neighboraddr structures for the neighbors ****************


        for(i=0;i<No_of_Neighbors;i++){
          memset((char *) &neighboraddr[i], 0, sizeof(neighboraddr[i])); 
          neighboraddr[i].sin_family = AF_INET;
          neighboraddr[i].sin_port   = htons(atoi(Neighbors_Port[i]));                         //Converting the port number to the network order
          if (inet_pton(AF_INET, Neighbors_IP[i], &neighboraddr[i].sin_addr) <= 0)       // Converting the IP address specified from string to network order
          err_quit("inet_pton error for IP: %s", Neighbors_IP[i]);
        }  


        thread_return = pthread_create(&thread,NULL,RIP_Receive,(void *) &No_of_Neighbors);
        if(thread_return){
           printf("\n There was an error in creating the Thread..");
        }

        // Starting the timer which will send the updates every 5 seconds 
        timer_start_periodic(&timerA, 5, update_time); 

//****************** TILL HERE **********************************************************
 
while(1){
         
        


        tv_init(&tv);
        tv_timer(&tv, &timerA);
        tv_timer(&tv, &timerB);
        tv_timer(&tv, &timerC);
        tv_timer(&tv, &timerD);


// ******************* CREATING THE MESSAGE BUFFER WITH ALL THE METRICS *****************
        strcpy(message_buffer,"UPDATE");

        for(i=0;i<No_of_nodes;i++){
          sprintf(message," %d",nodes[i].Distance);
          strcat(message_buffer,message);
        }
// ********************** TILL HERE *****************************************************




        


// **************** THIS IS THE CODE FOR CHECKING THE TIMERS **********************

        FD_ZERO(&rset);

        if (select(1, &rset, NULL, NULL, &tv) < 0)
        {
            err_quit("select() < 0, strerror(errno) = %s\n", strerror(errno));
        }

        timer_check(&timerA);
        timer_check(&timerB);
        timer_check(&timerC);
        timer_check(&timerD);

// ***************  TILL HERE *********************************************                 

         
          

// ********** THIS CODE STRIPS UP THE RECEIVED MESSAGE **************
          

// *********** TILL HERE **********************



          
          

}








        






















    



     return 0;




}   // Main Function