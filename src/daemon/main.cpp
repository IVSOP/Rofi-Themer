#include "errors.h"
#include "data.h"
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

#define MAX_CONN 1


#ifndef SOCK_PATH
    #define SOCK_PATH "build/Themer-socket"
#endif

// wraps output in message just for ease of use, might change, don't really like this
void reply(const std::string &string, int clientSock) {
    OutMessage msg;
    // strncpy(msg.str, string.c_str(), MESSAGE_STR_SIZE); WRONG, STRING MIGHT HAVE SEVERAL \0
    // also missing check for size of string too big
    memcpy(msg.str, string.c_str(), string.size());
    msg.len = string.size();
    if (write(clientSock, &msg, sizeof(OutMessage)) < 0) {
        print_error("Error writing data"); // stdout and stderr will be closed but whatever
        exit(EXIT_FAILURE);
    }
    // write(STDOUT_FILENO, &msg, sizeof(OutMessage));
}

void handleMessage(Data &data, const Message &msg, int clientSock, const std::string &dataDir) {
    std::string string = msg.str; // unecessary copy?
    switch (msg.type) {
        case READ:
            reply(data.read(string), clientSock);
            break;
        case MENU:
            reply(data.menuFirst(string), clientSock);
            data.saveTo(dataDir);
            break;
        default:
            reply("error", clientSock);
            break;
    }
}

// argv[1] == directory with data
// argv[2] == directory to place the socket
// since this is a daemon and paths are lost, please enter all paths as hardcoded
int main (int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Insuficient arguments: need dataset path\n");
        return EXIT_FAILURE;
    }
    //////////////////////////////////////////////// creating unix socket

    // Create a Unix domain socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        print_error("Error creating socket\n");
        return EXIT_FAILURE;
    }

    // Define the address structure for the socket
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
    unlink(addr.sun_path);
    socklen_t len = strlen(addr.sun_path) + sizeof(addr.sun_family); // wtf????? should I just use sizeof(addr))???

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&addr, len) < 0) {
        print_error("Error binding socket\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(sockfd, MAX_CONN) < 0) {
        print_error("Error listening on socket\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    //////////////////////////////////////////////// parsing theme file
    const std::string dataDir = argv[1];
    Data data(dataDir); // will parse the data
#ifdef DEBUG
    data.print();
#endif

    // data.print();

    // std::string input = "rofi/*";
    // puts(data.read(input).c_str());

    // std::string input = "1/dunst";
    // std::string res = data.menuFirst(input);

    //////////////////////////////////////////////// creating daemon
#ifndef DEBUG
    pid_t pid, sid;

    // Fork the process
    pid = fork();

    if (pid < 0) {
    // error
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process
        exit(EXIT_SUCCESS);
    }

    // Set file mode creation mask to 0
    umask(0);

    // Create a new session
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Change the working directory to root
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
#endif


    //////////////////////////////////////////////// listen for requests on the socket

    Message msg;
    while (true) {
        socklen_t sock_len = sizeof(addr);
        int clientSock = accept(sockfd, (struct sockaddr*)&addr, &sock_len);
        if (clientSock < 0) {
            print_error("Error accepting connection\n");
            close(sockfd);
            return EXIT_FAILURE;
        }

        read(clientSock, &msg, sizeof(Message));
#ifdef DEBUG
        puts("received:");
        puts(msg.str);
#endif
        handleMessage(data, msg, clientSock, dataDir);

        close(clientSock);
    }


    close(sockfd);
    return 0;
}
