#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BACKLOG 10 // how many pending connections queue will hold

using namespace std;

int main(int argc, char* argv[])
{
	// Check the argument
	if(argc < 2) {
		perror("Press the port (1025~)\n");
		exit(1);
	}

	// Put the value of argument
	//const char* my_ip = "127.0.0.1"; // Configure it to be Loop-back IP
	unsigned short int my_port = (unsigned short int)atoi(argv[1]);

	// Making socket with IPv4 and TCP
	int my_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int new_fd;

	// If could not create a socket, Return error message
	if(my_sockfd < 0) {
		perror("Could not create a socket\n");
		exit(1);
	}

	// Make the struct sockaddr_in to initialize the IP address
	struct sockaddr_in my_sockaddr_in, their_sockaddr_in;
	int sin_size;
	
	bzero(&my_sockaddr_in, sizeof(struct sockaddr_in));
	my_sockaddr_in.sin_family = AF_INET;
	my_sockaddr_in.sin_port = htons(my_port);

	// InADDR_ANY allows clients to connect to any one of the host's IP address
	my_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// Transfer IP address into sockaddr_in struct
	/*
	if(inet_pton(AF_INET, my_ip, &my_sockaddr_in.sin_addr) != 1) {
		perror("Could not transfer IP\n");
		exit(1);
	}
	*/

	// Bind into the specific IP and port
	if(bind(my_sockfd, (struct sockaddr*)&my_sockaddr_in, sizeof(my_sockaddr_in)) == -1) {
		perror("Could not bind\n");
		exit(1);
	}

	// Listen from my socket
	if(listen(my_sockfd, BACKLOG) == -1) {
		perror("Could not listen\n");
		exit(1);
	}

	// Accept connecting from client
	while(1) {
		sin_size = sizeof(struct sockaddr_in);
		
		if((new_fd = accept(my_sockfd, (struct sockaddr*) &their_sockaddr_in, (socklen_t*)  &sin_size)) == -1) {
			perror("Could not accept");
			continue;
		}

		printf("server: got connection from %s\n", inet_ntoa(their_sockaddr_in.sin_addr));
	}

	return 0;
}
