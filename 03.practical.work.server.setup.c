#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>

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
	int port_num = 8784;
   
 	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port_num);

	if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "ERROR: Binding on listen socket failed.");
		exit(1);
	}
	
	printf("Listening on port %d...\n", port_num);
	if (listen(listen_fd, 10)) {
		fprintf(stderr, "ERROR: Listen failed.");
		exit(1);
	}
	
	int clen = sizeof(client_addr);
   
	// accepting new connection
	int new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &clen);
	if (new_fd < 0) {
		fprintf(stderr, "ERROR: accept failed.");
		exit(1);
	}
	
	printf("New connection accepted.\n");
	printf("Just close all connections and exit.\n");
	
	close(new_fd);
	close(listen_fd);
	
	return 0;
}
