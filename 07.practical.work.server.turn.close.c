#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 8784
#define MAX_MSG_SIZE 100

// a chat session with a client through the socket msg_fd
// (msg_fd is the socket returned from an accept call earlier)
// The function returns when either the client disconnects or user types "\dc"
void handle_chat(int msg_fd)
{
    char msg[MAX_MSG_SIZE];
    char reply[MAX_MSG_SIZE];

    while (1) {
        printf(">>> CLIENT: ");
        int nbytes = recv( msg_fd, msg, MAX_MSG_SIZE, 0);
        if (nbytes < 0) {
            fprintf(stderr, "recv failed");
            exit(-1);
        }

        if (nbytes == 0) {
            // client has disconnected
            return;
        }
        
        msg[MAX_MSG_SIZE - 1] = 0; // making sure the string is terminated
        printf("%s\n", msg);

        printf(">>> Reply?: ");
        fgets(reply, MAX_MSG_SIZE, stdin);

        // handle "/dc"
        if (strncmp(reply, "/dc", 3) == 0) {
            return;
        }

        reply[strlen(reply) - 1] = 0; // stripping newline char at the end
        if (send(msg_fd, reply, strlen(reply) + 1, 0) < 0) {
            fprintf(stderr, "ERROR: send failed");
            exit(-1);
        }
        printf("---------- Reply sent. Next turn! -------------\n");
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

    // main server loop
    while (1) {
        printf("### Waiting for client on port %d...\n\n", PORT);

        // accepting new connection
        int clen = sizeof(client_addr);
	    int data_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &clen);
	    if (data_fd < 0) {
		    fprintf(stderr, "ERROR: accept failed.");
		    exit(1);
	    }
        printf("### New client connected.\n\n");

        handle_chat(data_fd);

	    printf("\n\n### Client disconnected.\n\n");
        shutdown(data_fd, SHUT_RDWR);
        close(data_fd);
    }
	
	close(listen_fd);
	return 0;
}
