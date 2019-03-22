#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <poll.h>

#define PORT 8784
#define MAX_MSG_SIZE 100

int main(int argc, char **argv)
{
    if (argc < 2) {
    	printf("usage: %s <hostname>\n", argv[0]);
        exit(-1);
	}
	
    // Resolving host name to IP address
	struct hostent *server = gethostbyname(argv[1]);
	if (!server) {
		fprintf(stderr, "ERROR: Unknown host: %s\n", argv[1]);
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

    printf("Connected to server.\n\n");

    // make sock_fd non-blocking
    int flags = fcntl(sock_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sock_fd, F_SETFL, flags);

    char msg[MAX_MSG_SIZE];
    char reply[MAX_MSG_SIZE];
    bool quit = false;

    while (!quit) {
        // check if the user has typed something
    	struct pollfd mypoll = { STDIN_FILENO, POLLIN };

        switch (poll(&mypoll, 1, 1000)) {
            case -1:
                fprintf(stderr, "ERROR: poll failed");
                exit(-1);
            case 0:
                // timeout, do nothing
                break;
            default:
                // there's something to read from stdin
                fgets(msg, MAX_MSG_SIZE, stdin);

                // handle "/quit"
                if (strncmp(msg, "/quit", 5) == 0) {
                    quit = true;
                    break;
                }
                // stripping newline char at the end and send it to the server
                msg[strlen(msg) - 1] = 0; 
                write( sock_fd, msg, strlen(msg) + 1);
                break;
        }

        if (!quit) {
            int nbytes = recv( sock_fd, reply, MAX_MSG_SIZE, 0);
            if ((nbytes < 0) && (errno != EAGAIN)) {
                // an error that is not non-blocking error
                fprintf(stderr, "ERROR: recv failed");
                exit(-1);
            }
            if (nbytes == 0) {
                // the server disconnected
                break;
            }
            if (nbytes > 0) {
                reply[MAX_MSG_SIZE - 1] = 0; // making sure the string is terminated
                printf("### SERVER says: %s\n", reply);
            }
        }
    }

    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    printf("\n### Disconnected from the server.\n\n");
    return 0;
}