#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8784

int main(int argc, char **argv)
{
	char host_name[50];

    if (argc < 2) {
    	scanf("%s", host_name);
	}
	else {
	    strcpy(host_name, argv[1]);
	}
	
    // Resolving host name to IP address
	//printf("Looking up host name: %s\n", inputStr);
	struct hostent *server = gethostbyname(host_name);
	if (!server) {
		fprintf(stderr, "ERROR: Unknown host: %s\n", host_name);
		exit(-1);
	}

    // creating local socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
    { 
        fprintf(stderr, "ERROR: Failed to create socket.\n"); 
        exit(-1); 
    }

    // building the server address struct using the IP address we've resolved
    struct sockaddr_in server_addr;

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(PORT);
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);

    // connecting to the remote server
    if (connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "ERROR: Cannot connect to server.\n"); 
        exit(-1); 
    }

    printf("Connected to server.\n");

    return 0;
}