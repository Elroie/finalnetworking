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
using namespace std;
#define PORT "9034" // port we're listening on
#define STDIN 0


map <string, string> userPassword;
map <string, int>    mapUserToSocket; 
map <int, string> socketToUser;
map <int, string> socketToIPPort; 
std::set <int> playingClients; 
// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void readUsers(){
	char *username;
	char *password;
	FILE * users;
	users=fopen("users.txt","r");
	while (!feof(users)) {
		fscanf(users, "%s,%s\n", username, password);
		pair <string,string>  newUser;
		newUser.first=username;
		newUser.second=password;
		userPassword.insert(newUser);
	}

}

void handleStdin(char * buf)
{
	if (0==strncmp("lu\n", buf,3)) {
		map <string, int>::iterator uit;
		for (uit=mapUserToSocket.begin(); 
			uit != mapUserToSocket.end() ; ++uit) 
				cout << *uit.first << std::endl;
	} else if (0 == strncmp("register ", buf, 9) {
		
		sscanf(buf, "register %s %s", username, password);
		FILE * users=fopen("users.txt","a");
		fprintf (users, "%s,%s\n", username, password);
		fclose (users);
	}
		// TODO other commands
}


int main(void) {
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
    readUsers();
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
        // run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds))
				if (STDIN == i) {
					char buf[80];
					read(STDIN, buf, 80);
					handleStdin(buf);
				}
                else if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
						// TODO add client to maps
                        printf("selectserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(
                                   remoteaddr.ss_family,
                                   get_in_addr((struct sockaddr *)&remoteaddr),
                                   remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                    }
                }
                else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
						string username = socketToUser[i];

						mapUserToSocket.erase(username); 
						socketToUser.erase(i);
						socketToIPPort.erase(i);
						
                        close(i);           // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        if (0 == strncmp("play ",buf, 5)) {
							char secondClient[10];
							sscanf (buf, "play %s", secondClient);
							int secondSocket = userToSocket[secondClient];
							string secondIPPort = socketToIPPort[secondSocket];
							write (i, secondIPPort.c_str(), secondIPPort.length());
							write (secondSocket, socketToIPPort[i].c_str(),
								socketToIPPort[i].length());
						} else if (0 == strncmp ("login ",  buf, 6)) {
							char username[50];
							char password[50];
							sscanf (buf, "login %s %s", username, password);
							if (userPassword[username]==password) { 
								write (i, "login successful", 16);
								pair <int, string> newClient;
								newClient.first=i;
								newClient.second=username;
								socketToUser.insert(newClient);
								pair <string, int> newClient2;
								newClient2.first=username;
								newClient2.second=i;
								mapUserToSocket.insert(newClient2);
							}else 
								write (i, "login failed",12)
						} 
                        else if (){}// keep parsing
							
                    }
                } // END handle data from client
            }     // END got new incoming connection
        }         // END looping through file descriptors
    }             // END for(;;)--and you thought it would never end!

    return 0;
}