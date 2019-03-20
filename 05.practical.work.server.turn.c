#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 8784
#define MAX_MSG_SIZE 100

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
	
	printf("Listening on port %d...\n", PORT);
	if (listen(listen_fd, 10)) {
		fprintf(stderr, "ERROR: Listen failed.");
		exit(1);
	}
	
	int clen = sizeof(client_addr);
   
	// accepting new connection
	int data_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &clen);
	if (data_fd < 0) {
		fprintf(stderr, "ERROR: accept failed.");
		exit(1);
	}
	
	printf("Client connection accepted.\n\n");

    char msg[MAX_MSG_SIZE];
    char reply[MAX_MSG_SIZE];

    while (1) {
    	printf(">>> CLIENT: ");
        recv( data_fd, msg, MAX_MSG_SIZE, 0);
        msg[MAX_MSG_SIZE - 1] = 0; // making sure the string is terminated
        printf("%s\n", msg);

        printf(">>> Reply?: ");
        scanf("%s", reply);
        reply[MAX_MSG_SIZE - 1] = 0;
        send(data_fd, reply, strlen(reply) + 1, 0);
        printf("---------- Reply sent. Next turn! -------------\n");
    }
	
	close(data_fd);
	close(listen_fd);
	
	return 0;
}
