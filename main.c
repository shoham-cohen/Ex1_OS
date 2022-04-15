#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define length 50
#define PORT 8080

int main(){

    char command[length] = "initialized";
    bool send_it = false;
    int client_sock;
    while(strcmp("EXIT", command) != 0){
        
        char cwd[256];
        // print the current working directory using getcwd(), https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
        if (getcwd(cwd, sizeof(cwd)) == NULL){
            perror("getcwd() error");
        }
        else{
            printf("current working directory is: %s\n", cwd);
        }
        printf("yes master? ");
        fgets(command, length, stdin);
        char *contain;
        contain = strstr(command, "ECHO");
        if(contain){
            if(send_it == false){
                for (int i = 5; i < length && command[i] != '\0'; i++){
                    printf("%c", command[i]);
                }
                printf("\n");
            }  
            else{
                for(int i = 5; i < length && command[i] != '\0'; i++){
                    send(client_sock, &command[i], sizeof(&command[i]), 0);
                }
                send(client_sock, "\n", sizeof("\n"), 0);
            }
        }

        else if (strncmp(command, "TCP PORT", 8) == 0){
            char *local_ip = "127.0.0.1";
            struct sockaddr_in server;
            client_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(client_sock == -1){
                printf("cant create the socket\n");
                exit(1);
            }
            printf("socket created successfully\n");
            printf("connecting to server...\n");
            bzero(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = PORT;
            server.sin_addr.s_addr = inet_addr(local_ip);
            int con = connect(client_sock, (struct sockaddr*)&server, sizeof(server));
            if(con == -1){
                printf("couldnt connect to server\n");
                exit(1);
            }
            printf("connected to the server\n");
            send_it = true;
        }

        else if (strncmp(command, "LOCAL", 5) == 0){
            send_it = false;
            if(client_sock != 0){
                close(client_sock);
                printf("disconnected\n");
            }
        }

        else if(strncmp(command, "DIR", 3) == 0){
            DIR *d;
            struct dirent *dir;
            d = opendir(".");
            if(d){
                while ((dir = readdir(d)) != NULL) {
                    printf("%s\n", dir->d_name);
                }
                closedir(d);
            }
            else{
                printf("error openning the directory :(\n");
                break;
            }
        }

        else if(strncmp(command, "CD", 2) == 0){
            strncpy(command, command + 3, sizeof(command) - 3);
			command[strlen(command)-1]='\0';
			chdir(command);
            // The chdir command is a system call which is used to change the current working directory.
            // https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
        }

        //unlink() is a system call https://linuxhint.com/unlink-system-call-in-c/
        else if(strncmp(command, "DELETE", 6) == 0){
            strncpy(command, command + 7, sizeof(command) - 7);
			command[strlen(command) - 1]= '\0';
			if (unlink(command)){
				printf("cant delete the file\n");
			}
			else
				printf("file deleted succesfully\n");
        }

        else if (strncmp(command, "COPY", 4) == 0){
            memcpy(command, command + 5, length - 5);
            char *src;
            char *dest;
            src = strtok(command, " ");
            dest = strtok(NULL, " ");
            char char_to_read;
            FILE *src_file = fopen(src, "rb");
            FILE *dest_file = fopen(dest, "wb");
            while (fread(&char_to_read, 1, 1, src_file) == 1){
                fwrite(&char_to_read, 1, 1, dest_file);
            }
            fclose(src_file);
            fclose(dest_file);
        }
    }

}