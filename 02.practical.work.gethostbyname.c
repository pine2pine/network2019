#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	struct hostent *host = 0;
	char inputStr[50];

    if (argc < 2) {
    	scanf("%s", inputStr);
	}
	else {
		strcpy(inputStr, argv[1]);
	}
	
	printf("Looking up host name: %s\n", inputStr);
	host = gethostbyname(inputStr);

	if (!host) {
		fprintf(stderr, "Lookup Failed: %s\n", hstrerror(h_errno));
		return 0;
	}

	printf("IP addresses:\n");
	
	unsigned int i=0;
	
	while (host->h_addr_list[i] != NULL) {
		printf( "%s\n", inet_ntoa( *(struct in_addr*)(host->h_addr_list[i])));
		i++;
	}
	printf("\n");
	
	return 0;
}
