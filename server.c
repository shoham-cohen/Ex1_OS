#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define local_ip "127.0.0.1"
#define Port 8080
#define size 120

int main(){
    int server_sock = socket(AF_INET,SOCK_STREAM, 0);
    if(server_sock == -1){
        printf("cant create the socket\n");
        exit(1);
    }
    printf("socket created successfully\n");
    int set = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if(set == -1){
        printf("cant set the socket");
        exit(1);
    }
    printf("socket set\n");
    fflush(stdin);
    struct sockaddr_in server_addr, client_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(local_ip);
    server_addr.sin_port = Port;
    int binded = bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(binded == -1){
        printf("cant bind");
        exit(1);
    }
    printf("socket bind\n");
    fflush(stdin);
    listen(server_sock, 1);
    socklen_t size_of_addr = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &size_of_addr);
    if(client_sock == -1){
        printf("cant accept");
        exit(1);
    }
    printf("client accepted\n");
    fflush(stdin);
    for(;;){
        char buffer[size];
        bzero(buffer, size);
        read(client_sock, buffer, sizeof(buffer));
        if (strncmp("EXIT", buffer, 4) == 0 || (strncmp(buffer, "LOCAL", 4)) == 0){
            close(client_sock);
            break;
        }
        printf("%s\n", buffer);
    }

}