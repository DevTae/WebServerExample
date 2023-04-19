#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream> // string stream
#include <fstream> // file stream
#define BACKLOG 10 // how many pending connections queue will hold

using namespace std;

void cleanExit(int) { exit(0); }

map<string, string> mime_types = {
	{".gif", "image/gif"},
	{".jpeg", "image/jpeg"},
	{".mp3", "audio/mpeg3"},
	{".pdf", "application/pdf"}
};

string get_mime_type(string file_path) {
	size_t pos = file_path.find_last_of(".");

	if(pos != string::npos) {
		string ext = file_path.substr(pos);
		if(mime_types.find(ext) != mime_types.end()) {
			return mime_types[ext];
		}
	}

	return "text/plain";
}

string get_response_header(string file_path) {
	stringstream ss;
	ss << "HTTP/1.0 200 OK\r\nContent-Type: " << get_mime_type(file_path) << "\r\n\r\n";
	return ss.str();
}

void send_response(int their_sockfd, string file_path) {
	string response_header = get_response_header(file_path);

	ifstream ifs(file_path.substr(1));
	
	if(ifs.is_open()) {
		// if ifstream is opened, return file contents
		string line;
		while(getline(ifs, line)) {
			response_header += line + "\n";
		}
		ifs.close();
	} else {
		// if ifstream is not opened, return not found page
		response_header = "HTTP/1.0 404 Not Found\r\n\r\n";
	}

	send(their_sockfd, response_header.c_str(), response_header.length(), 0);
}

vector<string> split(string input, char delimeter) {
	vector<string> result;
	stringstream ss(input);
	string temp;
	
	while(getline(ss, temp, delimeter)) {
		result.push_back(temp);
	}

	return result;
}

int main(int argc, char* argv[])
{
	// Check the argument
	if(argc < 2) {
		fprintf(stderr, "Press the port (1025~)\n");
		exit(1);
	}

	// Add the signal function
	signal(SIGTERM, cleanExit);
	signal(SIGINT, cleanExit);

	// Put the value of argument
	//const char* my_ip = "127.0.0.1"; // Configure it to be Loop-back IP
	unsigned short int my_port = (unsigned short int)atoi(argv[1]);

	// Making socket with IPv4 and TCP
	int my_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int their_sockfd;

	// If could not create a socket, Return error message
	if(my_sockfd < 0) {
		perror("Could not create a socket\n");
		exit(1);
	}

	// Make the struct sockaddr_in to initialize the IP address
	struct sockaddr_in my_sockaddr_in, their_sockaddr_in;
	int their_sin_size;
	
	bzero(&my_sockaddr_in, sizeof(struct sockaddr_in));
	my_sockaddr_in.sin_family = AF_INET;
	my_sockaddr_in.sin_port = htons(my_port);

	// InADDR_ANY allows clients to connect to any one of the host's IP address
	my_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind into the specific IP and port
	if(bind(my_sockfd, (struct sockaddr*)&my_sockaddr_in, sizeof(my_sockaddr_in)) == -1) {
		perror("Could not bind\n");
		exit(1);
	}

	// Listen from my socket with in BACKLOG connections simultaneously
	if(listen(my_sockfd, BACKLOG) == -1) {
		perror("Could not listen\n");
		exit(1);
	}

	while(1) {
		their_sin_size = sizeof(struct sockaddr_in);
		
		// Accept connecting from client
		if((their_sockfd = accept(my_sockfd, (struct sockaddr*) &their_sockaddr_in, (socklen_t*)  &their_sin_size)) == -1) {
			perror("Could not accept. Try reconnect.");
			continue;
		}

		printf("server: got connection from %s\n", inet_ntoa(their_sockaddr_in.sin_addr));

		// Get the request header information
		char buffer[8190];
		if(read(their_sockfd, buffer, 8190) < 0) {
			fprintf(stderr, "Nothing received\n");
		}

		// Store the header information on each string object
		stringstream ss(buffer);
		string request_header = "";
		string request_method, file_path, http_version;
		string line;
		bool isFirstLine = true;

		while(getline(ss, line)) {
			if(isFirstLine) {
				vector<string> result = split(line, ' ');
				request_method = result[0];
				file_path = result[1];
				http_version = result[2];
				isFirstLine = false;
			}
			request_header.append(line);
			request_header.append("\n");
		}
		
		// Print the request message
		cout << request_header;

		// Send the response message if the request method is GET
		if (request_method == "GET") {
			send_response(their_sockfd, file_path);
		}

		// Close the client socket
		close(their_sockfd);
	}

	close(my_sockfd);

	return 0;
}
