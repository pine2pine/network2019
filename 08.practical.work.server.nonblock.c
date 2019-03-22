#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

#define PORT 8784
#define MAX_MSG_SIZE 100
#define MAX_NB_CLIENTS 40

unsigned nb_clients = 0;
int client_sock[MAX_NB_CLIENTS];

// Check if the client k has sent something. If yes, print the message.
// If the client has diconnected, shutdown the socket properly.
void read_client(unsigned k)
{
    char msg[MAX_MSG_SIZE];
    char reply[MAX_MSG_SIZE];

    int nbytes = read( client_sock[k], msg, MAX_MSG_SIZE);

    if ( (nbytes < 0) && (errno != EAGAIN) ) {
        // an error other than non-blocking timeout
        fprintf(stderr, "ERROR: read failed");
        exit(-1);
    }

    if (nbytes > 0) {
        msg[MAX_MSG_SIZE - 1] = 0; // making sure the string is terminated
        printf("CLIENT %d says: %s\n", k+1, msg);
    }

    if (nbytes == 0) {
        // client has disconnected
        shutdown(client_sock[k], SHUT_RDWR);
        close(client_sock[k]);

        for (int i = k+1; i < nb_clients; i++) {
            client_sock[i-1] = client_sock[i];
        }
        nb_clients--;
        printf("\n\n### CLIENT %d disconnected. Now there're %d clients.\n\n", k+1, nb_clients);
    }
}

// Check if the user has typed something. If yes, broadcast the message.
void handle_input()
{
    char msg[MAX_MSG_SIZE];
    struct pollfd mypoll = { STDIN_FILENO, POLLIN };

    switch (poll(&mypoll, 1, 1000)) {
        case -1:
            fprintf(stderr, "ERROR: poll failed");
            exit(-1);
        case 0:
            // timeout, do nothing
            break;
        default:
            // there's data to be read from stdin
            fgets(msg, MAX_MSG_SIZE, stdin);
            // stripping newline char at the end, and send it to all clients
            msg[strlen(msg) - 1] = 0; 
            for (int i = 0; i < nb_clients; i++) {
                if (write(client_sock[i], msg, strlen(msg) + 1) < 0) {
                    fprintf(stderr, "ERROR: write failed");
                    exit(-1);
                }
            }
            break;
    }

}
      
int main( int argc, char *argv[] )
{
	struct sockaddr_in server_addr, client_addr;
   
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		fprintf(stderr, "ERROR: Opening listen socket failed.");
		exit(1);
	}

    // try to reuse the listening port
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // make listen_fd non-blocking
    int flags = fcntl(listen_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(listen_fd, F_SETFL, flags);

	// init server address structure
	memset((char *) &server_addr, 0, sizeof(server_addr));
   
 	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "ERROR: Binding on listen socket failed.");
		exit(1);
	}
	
	if (listen(listen_fd, 10)) {
		fprintf(stderr, "ERROR: Listen failed.");
		exit(1);
	}
    printf("### Waiting for client on port %d...\n\n", PORT);

    // main server loop
    while (1) {
        // check if there's a new connection from a client
	    int new_fd = accept(listen_fd, NULL, NULL);
	    if ( (new_fd == -1) && (errno != EAGAIN) ) {
		    fprintf(stderr, "ERROR: accept failed.");
		    exit(1);
	    }
        if (new_fd >= 0) {
            client_sock[nb_clients] = new_fd;
            nb_clients++;
            printf("### New client connected. Now, there're %d clients.\n\n", nb_clients);

            // make this new connection non-blocking
            int flags = fcntl(new_fd, F_GETFL, 0);
            flags |= O_NONBLOCK;
            fcntl(new_fd, F_SETFL, flags);
        }

        // if there's no client yet, keep checking for new connection
        if (nb_clients) {
            // Check if the user has typed something. If yes, broadcast the message.
            handle_input();

            // Polling each client, and handle the message
            for (int i = 0; i < nb_clients; i++) {
                read_client(i);
            }
        }
    }
	
	close(listen_fd);
	return 0;
}
