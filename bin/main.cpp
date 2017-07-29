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
#include <string>
#include "UserRepository.h"

using namespace std;
#define PORT "9034" // port we're listening on
#define STDIN 0

int main(){
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

    UserRepository userRepository;

    return 0;
}