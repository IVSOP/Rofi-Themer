#include "crash_print.hpp"
#include "message.hpp"

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

int main (int argc, char **argv) {

    CRASH_IF(argc < 2, "Insuficient arguments: need query to send");

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    CRASH_IF(sockfd < 0, "Error opening socket");

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;

    // this is also VERY bad and WILL crash for big strings, rewrite, the daemon has this as well wtf
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    socklen_t data_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family); // wtf????? should I just use sizeof(addr))???

    CRASH_IF(connect(sockfd, (struct sockaddr *)&server_addr, data_len) < 0, "Error connecting to daemon at " + std::string(server_addr.sun_path) + "\n");

    Message msg;
    msg.type = READ;
    // strncpy(msg.str, QUERY, MESSAGE_STR_SIZE); wrong, query is in the message type and not as a string
    strncpy(msg.str, argv[1], MESSAGE_STR_SIZE - 1); // for the compiler to shut up. I use write() instead of print so it should be fine either way

    write(sockfd, &msg, sizeof(Message));

    OutMessage outmsg;
    /* ssize_t bread = */ read(sockfd, &outmsg, sizeof(OutMessage));
    // printf("Received %ld bytes\n", bread);
    // eh could be a print but whatever
    write(STDOUT_FILENO, outmsg.str, outmsg.len);


    close(sockfd);
    return 0;
}
