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
#include "HashUtils.h"
#include "ScoreRepository.h"
#include <iostream>

using namespace std;
#define PORT "9034" // port we're listening on
#define STDIN 0

map <string, int> userToSocket; 
map <int, string> socketToUser;
map <int, string> socketToIPPort;
map <int, int> pendingInvitations;

static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(){
    UserRepository userRepository;
    ScoreRepository scores;

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
        cout << "inside main loop" << endl;

        read_fds = master; // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)){
                if (i==STDIN) {
                		cout << "inside STDIN" << endl;
                    if (0==strncmp("lu\n", buf,3)) {
                        cout << "inside lu" << endl;
                        std::vector<string> onlineUsers = userRepository.getOnlineUsers();
                        std::vector<string>::iterator uit;
                        for (uit=onlineUsers.begin(); uit != onlineUsers.end() ; ++uit) {
                            cout << "some names" << endl;
                            cout << *uit << std::endl;
                        }
                    } else if (0 == strncmp("register ", buf, 9)) {
                         cout << "inside register" << endl;
                        char *username;
                        char *password;
                        sscanf(buf, "register %s %s", username, password);
                        userRepository.add(std::string(username), std::string(password));
                        cout << "added user" << endl;
                    }
                    // TODO other commands
				}
                else if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } 
                    else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }

                        std::string ip = std::string(inet_ntop(
                                   remoteaddr.ss_family,
                                   get_in_addr((struct sockaddr *)&remoteaddr),
                                   remoteIP, INET6_ADDRSTRLEN));

                        pair<int, string> keyValue;
                        keyValue.first = newfd;
                        keyValue.second = ip;
                        socketToIPPort.insert(keyValue);

                        cout << "new connection from " << ip << " on " << newfd << endl;
                    }
                }
                else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("socket %d hung up\n", i);
                        } 
                        else {
                            perror("recv");
                        }

						string username = socketToUser[i];

						userToSocket.erase(username); 
						socketToUser.erase(i);
						socketToIPPort.erase(i);
						userRepository.setUserAvailability(username, false);

                        close(i);           // bye!
                        FD_CLR(i, &master); // remove from master set
                    }
                    else {
                        cout << "buf is" << buf << endl;
                        if (0==strncmp("lu\n", buf,3)) {
                        	   cout << "inside lu" << endl;
                            std::vector<string> onlineUsers = userRepository.getOnlineUsers();
                            std::vector<string>::iterator uit;
                            for (uit=onlineUsers.begin(); uit != onlineUsers.end() ; ++uit){
                                    cout << *uit << std::endl;
                                    write(i, &(*uit), uit->length()+1);

                            } 
                                    
                        } else if (0 == strncmp("register ", buf, 9)) {
                            cout << "inside register" << endl;
                            char *username = (char*)malloc(sizeof(char)*10);;
                            char *password = (char*)malloc(sizeof(char)*10);;
                            sscanf(buf, "register %s %s", username, password);
                            userRepository.add(std::string(username), std::string(password));
                            cout << "added user" << endl;
                        }
                        else if (0 == strncmp("play ",buf, 5)) {
							char secondClient[10];
							sscanf (buf, "play %s", secondClient);
							int secondSocket = userToSocket[secondClient];
							string secondIPPort = socketToIPPort[secondSocket];
                            pair<int, int> gameRequest;
                            gameRequest.first = secondSocket;
                            gameRequest.second = i;
                            pendingInvitations.insert(gameRequest);
                            string invitationMessage= "Got game invitation from " + socketToUser[i] + "\n";
							write (secondSocket, invitationMessage.c_str(), invitationMessage.length());
						}
                        else if (0 == strncmp("y\n ",buf, 2)) {
							if (pendingInvitations.count(i) > 0){
                                // send approval with ip of the user for the sender.
                                int senderSocket = pendingInvitations[i];
                                string ip = socketToIPPort[i];
                                string message = "PlayS: " + ip;
                                write(senderSocket, message.c_str(), message.length());
                                message = "PlayR: " + socketToIPPort[senderSocket];
                                write(i, message.c_str(), message.length());
                                pendingInvitations.erase(i);

                            }
                            else {
                                // send to socket... no pending invitations.
                                string message = "No pending invitations.";
                                write(i, message.c_str(), message.length());
                            }
						}
                        else if (0 == strncmp("n\n ",buf, 2)) {
							if (pendingInvitations.count(i) > 0){
                                // send approval with ip of the user for the sender.
                                int senderSocket = pendingInvitations[i];
                                string ip = socketToIPPort[i];
                                string message = "invitation rejected";
                                write(senderSocket, message.c_str(), message.length());
                                pendingInvitations.erase(i);
                            }
                            else {
                                // send to socket... no pending invitations.
                                string message = "No pending invitations.";
                                write(i, message.c_str(), message.length());
                            }
						}
                        else if (0 == strncmp("play random\n",buf, 12)) {
                            std::vector<string> onlineUsers = userRepository.getOnlineUsers();
                            string username = socketToUser[i];
                            string randomUser = "";

                            // while random user is the same user... choose another random user.
                            do {
                                int randomIndex = rand() % onlineUsers.size();
                                randomUser = onlineUsers[randomIndex];
                            } while(username == randomUser);
                            string ip = socketToIPPort[userToSocket[randomUser]];

                            write(i, ip.c_str(), ip.length());
						}
                        else if (0 == strncmp ("login ",  buf, 6)) {
							char username[50];
							char password[50];
							sscanf (buf, "login %s %s", username, password);
							if (userRepository.login(username, password)) { 
                                cout << "login successful" << endl;
								write (i, "login successful", 16);
                                
                                pair<int, string> socToUser;
                                pair<string, int> userToSoc;
                                socToUser.first = i;
                                socToUser.second = username;
                                userToSoc.first = username;
                                userToSoc.second = i;
                                socketToUser.insert(socToUser);
                                userToSocket.insert(userToSoc);
							}
                            else {
                                cout << "login failed" << endl;
								write (i, "login failed",12);
                            } 
						}
                        else if (0 == strncmp ("score board\n",  buf, 12)) {
                            vector<string> scoreBoard = scores.getScoreBoard();

                            std::vector<string>::iterator uit;
								for (uit=scoreBoard.begin(); uit != scoreBoard.end() ; ++uit) {
									write(i, &(*uit), uit->length()+1);
								}
						}
                        else if (0 == strncmp ("i won ",  buf, 6)) {
                        	int score = 0;
                        	sscanf (buf, "i won %d", score);
                            scores.updateUserScore(socketToUser[i], score);
						}
                    }
                }
            }
        }
    }

    return 0;
}
