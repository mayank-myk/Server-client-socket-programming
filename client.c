// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
  
int main(int argc, char const *argv[]){
    struct sockaddr_in address;
    char server_address[20];
    strcpy(server_address, argv[1]);
    int PORT = atoi(argv[2]);
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, server_address, &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }
    char input[50];
    int flag=0;
    for (int i = 0; i < 10; ++i){
    
        printf("\nRequest no: %d \nType ur message here: ",i);
        scanf("%s",input);
        if(input[0]=='1'){
            flag=1;
            break;
        }
        send(sock , input , strlen(input) , 0 );
        printf("%s ..... message sent\n\n",input);
        if ((valread = read( sock , buffer, 1024)) == 0){
            printf("check for errror\n");
            break;
        }
        printf("Reply from server: %s\n",buffer );
        if(buffer[0]=='y'){
            memset(buffer,0,sizeof(buffer));
            if ((valread = read( sock , buffer, 1024)) == 0){
                printf("check for errror\n");
                break;
            }
            printf("Reply from server: %s\n",buffer );

        }
        memset(buffer,0,sizeof(buffer));

    }
    
    // printf("type ur message here: ");
    // scanf("%[^\n]",input);
    // while(input[0]!='1'){
    //     send(sock , input , strlen(input) , 0 );
    //     printf("\n%s ..... message sent\n",input);
    //     if ((valread = read( sock , buffer, 1024)) == 0){
    //         printf("check for errror\n");
    //         break;
    //     }
    //     printf("Reply from server: %s\n",buffer );
    //     printf("type ur message here: ");
    //     scanf("%[^$]",input);
    //     printf("ur input was: %s\n",input);
    // }

    if(flag==1){
        send(sock , input , strlen(input) , 0 );
        printf("\n%s ..... message sent\n",input);
        valread = read( sock , buffer, 1024);
        printf("Reply from server: %s\n",buffer );

    }
    
    
    return 0;
}