#include "errors.h"
#include "message.h"

// like 50% of these are unused
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#ifndef SOCK_PATH
	#define SOCK_PATH "build/Themer-socket"
#endif

int main () {

	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        puts("Error opening socket");
        return EXIT_FAILURE;
    }

	struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;

	// this is also VERY bad and WILL crash for big strings, rewrite, the daemon has this as well wtf
	strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

	socklen_t data_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family); // wtf????? should I just use sizeof(addr))???

	if (connect(sockfd, (struct sockaddr *)&server_addr, data_len) < 0) {
        printf("Error connecting to daemon at %s\n", server_addr.sun_path);
        return EXIT_FAILURE;
    }

	Message msg;
	msg.type = MENU;
	// strncpy(msg.str, QUERY, MESSAGE_STR_SIZE); wrong, query is in the message type and not as a string

	char *info = getenv("ROFI_INFO");
	if (info != nullptr) {
		strncpy(msg.str, info, MESSAGE_STR_SIZE - 1);  // for the compiler to shut up. I use write() instead of print so it should be fine either way
	}


	write(sockfd, &msg, sizeof(Message));

	OutMessage outmsg;
	/* ssize_t bread = */ read(sockfd, &outmsg, sizeof(OutMessage));
	// printf("Received %ld bytes\n", bread);
	// ugly but has to be done this way, string might have many \0
	write(STDOUT_FILENO, outmsg.str, outmsg.len);


	close(sockfd);
	return 0;
}
