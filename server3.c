#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
    
#define TRUE   1 
#define FALSE  0 
    
int main(int argc , char *argv[])  {  
	int PORT;
	PORT  = atoi(argv[1]);
    int opt = TRUE;  
    int master_socket , addrlen , new_socket , client_socket[30] ,max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
    char buffer[1025];  //data buffer of 1K 
    fd_set readfds;  
    char *message = "you are now connected \r\n";  
    
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  {  
        client_socket[i] = 0;  
    }  
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
        
    //bind the socket to port 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){ 
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 10 pending connections for the master socket 
    if (listen(master_socket, 10) < 0) {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
        
        int upc , n;    
    long long total[30]  = {0};
    char stotal[16];  
    while(TRUE)  {
        int flag=0;
        //printf("I was here1 \n");  

        //clear the socket set 
        FD_ZERO(&readfds);  
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
            
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  {  
            //socket descriptor 
            sd = client_socket[i];   
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);    
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }
  
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
      
        if ((activity < 0) && (errno!=EINTR)){  
            printf("select error");  
        }  
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  {
            flag=1;  

            //printf("I was here2 \n");  

            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
            //inform user of socket number - used in send and receive commands 
            printf("\nNew connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
          
            //send new connection greeting message 
            // if( send(new_socket, message, strlen(message), 0) != strlen(message) )  {  
            //     perror("send");  
            // }  
                
            // puts("Welcome message sent successfully");  
                
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  {  
                //if position is empty 
                if( client_socket[i] == 0 )  {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of socket_fd %d as CLIENT_%d\n" ,new_socket, i);    
                    total[i]  = 0;
                    break;  
                }  
            }
        }  
       

        //else its some IO operation on some other socket
        if(flag==0){
            for (i = 0; i < max_clients; i++)  {
            //printf("I was here3 \n");  
            sd = client_socket[i];
            //printf("client_socket_id %d as CLIENT_%d\n" ,sd, i);    

            if (FD_ISSET( sd , &readfds))  {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0){
                    //printf("I was here4 \n");  

                    // getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen); 
                    // total[i]=0;
                    // char response[100];
                    // strcpy(response, "1protocolerror");
                    // send(sd, response, strlen(response), 0);
                    total[i]=0;
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;
                    break;  
                    
                }   
                else{
                    if(buffer[0]=='1'){
                        //printf("I was here5 \n");  
                        printf("\n CLIENT_%d Close Request \n total is %lld\n",i,total[i]);
                        char response2[100];
                        //response[0] = '0';
                        sprintf(stotal, "%lld", total[i]);
                        //printf("stotal is: %s, total is %lld\n",stotal,total[i]);
                        strcat(response2, "Total cost is: ");
                        strcat(response2, stotal);
                        printf(" Response= %s\n",response2);
                        send(sd, response2, strlen(response2), 0);
                        close( sd );  
                        client_socket[i] = 0;
                        memset(response2,0,sizeof(response2));

                        break;  
                    }  //send total amount here 
                    upc = (buffer[1] - '0')*100 + (buffer[2]-'0')*10 + (buffer[3]-'0');
                    n = (buffer[4] - '0')*100 + (buffer[5]-'0')*10 + (buffer[6]-'0');
                    //printf("%s\n", buf );
                    printf("\n CLIENT_%d Request\n upc : %d\n number %d \n",i, upc , n );
                    //send(new_socket, "success", strlen("success"), 0);


                    FILE * fp = fopen("database.txt", "r");
                    char temp[100], name[100], price_string[100], upc_str[100];
                    int upc1, price;
                    //printf("I was here6 \n");  
                    
                    while((fgets(temp, 100, fp)!= EOF)){

                        const char s[2] = ",";
                        char *token;
                               
                        /* get the first token */
                        token = strtok(temp, s);
                        strcpy(upc_str , token);
                        upc1 = atoi(token);
                        /* walk through other tokens */
                        int z=0;
                        while( token != NULL ) {
                                  
                                   
                                  if(z==1)strcpy(name , token); 
                                          
                                  if(z==2) strcpy(price_string , token);
                                  token = strtok(NULL, s);
                                  z++;
                        }
                        //printf("upc: %d, name: %s, price: %d\n", upc1, name, price);
                        if(upc == upc1){//send 0 price name
                            char response[100]= {'*'};
                            response[0] = '0';
                            int x = strlen(price_string)-1;
                            strncat(response, price_string, x);
                            strcat(response, name);
                            price = atoi(price_string);
                            total[i] += price * n;
                            printf(" Response : %s \n Total is %lld\n",response,total[i] );
                            send(sd, response, strlen(response), 0);
                            memset(response,0,sizeof(response));

                            break;
                        }

                    }
                    if(upc != upc1)
                    {   char response1[100];
                        strcpy(response1, "UPC not found in the database");
                        send(sd, response1, strlen(response1), 0);
                        memset(response1,0,sizeof(response1));

                    }
                    break;

                }    
            }  
        }
        }
          
    }  
        
    return 0;  
}  