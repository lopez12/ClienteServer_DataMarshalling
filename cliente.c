/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
#define on_print(...) { fprintf(stderr, __VA_ARGS__); }
 
int main(int argc , char *argv[])
{
    if(argc<2) on_error ("Usage %s [port]\n",argv[0]);
    int sock;
    struct sockaddr_in server;
    unsigned char message_init[2] , server_reply[2000], to_send[255];
    unsigned char *message, *p_send, *p_checksum;
    int buffer_size = 0;
    int port_number = atoi(argv[1]);
    int checksum = 0;
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( port_number );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
    while(1)
    {
        printf("Enter message : ");
        scanf("%s" , to_send);
	p_send = to_send;
        message_init[0] = 170;
	message_init[1] = strlen(to_send);
	buffer_size = strlen(to_send);
	message = message_init;
	strcat(message,p_send);
	while(buffer_size--)
	{
		checksum += *p_send;
		p_send++;
	}
        //Send some data
	on_print("%i\n",checksum);
	//p_checksum = &checksum;
	//strcat(message,p_checksum);
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
         
        puts("Server reply :");
        puts(server_reply);
    }
     
    close(sock);
    return 0;
}
