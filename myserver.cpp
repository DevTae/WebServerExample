#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char* argv[])
{
	// Check the argument
	if(argc < 2) {
		printf("Press the port (1025~)\n");
		return 1;
	}

	// Put the value of argument
	const char* my_ip = "127.0.0.1";
	unsigned short int my_port = (unsigned short int)atoi(argv[1]);

	// Making socket with IPv4 and TCP
	int my_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// If could not create a socket, Return error message
	if(my_sockfd < 0) {
		printf("Could not create a socket\n");
		return 1;
	}

	// Make the struct sockaddr_in to initialize the IP address
	struct sockaddr_in my_sockaddr_in;
	
	bzero(&my_sockaddr_in, sizeof(struct sockaddr_in));
	my_sockaddr_in.sin_family = AF_INET;
	my_sockaddr_in.sin_port = htons(my_port);
	
	// Transfer IP address into sockaddr_in struct
	if(inet_pton(AF_INET, my_ip, &my_sockaddr_in.sin_addr) != 1) {
		printf("Could not transfer IP\n");
		return 1;
	}

	// Bind

	return 0;
}
