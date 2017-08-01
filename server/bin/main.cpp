#include <arpa/inet.h>         // for inet_ntop
#include <netdb.h>             // for addrinfo, freeaddrinfo, gai_strerror
#include <netinet/in.h>        // for INET6_ADDRSTRLEN, sockaddr_in, sockadd...
#include <stdio.h>             // for NULL, perror, fprintf, printf, stderr
#include <stdlib.h>            // for exit
#include <string.h>            // for memset
#include <sys/select.h>        // for FD_ISSET, FD_SET, FD_ZERO, fd_set, select
#include <sys/socket.h>        // for sockaddr_storage, accept, bind, listen
#include <unistd.h>            // for close
#include <map>
#include <set>
#include <strings.h>
#include <iostream>
#include <stdlib.h>
#include "UserRepository.h"

using namespace std;
#define PORT "9034" // port we're listening on
#define STDIN 0

map <string, int> mapUserToSocket;

int main(){
    UserRepository* userRepository;

    fd_set master;   // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    int fdmax;       // maximum file descriptor number

    int listener;                       // listening socket descriptor
    int newfd;                          // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256]; // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1; // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;
        
    FD_ZERO(&master); // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }
    
    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
	FD_SET(STDIN, &master);    

    // main loop
    for (;;) {
        read_fds = master; // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)){
                if (STDIN == i) {
					char buf[80];
					read(STDIN, buf, 80);
                    
                    if (0==strncmp("lu\n", buf,3)) {
                        // map <string, int>::iterator uit;
                        // for (uit=mapUserToSocket.begin(); 
                        //     uit != mapUserToSocket.end() ; ++uit) 
                        //         cout << *uit.first << std::endl;
                    } else if (0 == strncmp("register ", buf, 9)) {
                        char *username;
                        char *password;
                        sscanf(buf, "register %s %s", username, password);
                        UserRepository::create(std::string(username), std::string(password));
                    }
                    // TODO other commands
				}
            }
        }
    }

    return 0;
}