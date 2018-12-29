#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
#define on_print(...) { fprintf(stderr, __VA_ARGS__); }
 
//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    if(argc<2) on_error ("Usage %s [port]\n",argv[0]);
    int socket_desc , client_sock , c , *new_sock;
    int port_number = atoi(argv[1]);
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port_number );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    unsigned char *message , client_message[2000], *init_message, *real_incomedata;
    int buffersize = 0;
    int calcualted_checksum = 0;
     
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
	message = client_message;
	on_print("%i\n",*message);
	if(*message != 170)
	{
		puts("Incorrect Start Frame");
	}
	message++;
	buffersize = (int) *message;
	on_print("%i\n",buffersize);
	init_message = message;
	//init_message++;init_message++;init_message++;
	message++;message++;message++;message++;
	while(buffersize--)
	{
		on_print("%c\n",*message);
		real_incomedata = message;
		calcualted_checksum += *message;
		message++;
		init_message++;
		real_incomedata++;	
	}
	on_print("%i\n",calcualted_checksum);
	if(calcualted_checksum != *message)
	{
		puts("Correct CheckSum");
	}
        //Send the message back to client
        write(sock ,  init_message , strlen(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}
