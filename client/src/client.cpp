/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>

#define MYPORT 4950    // the port users will be connecting to
#define STDIN 0
#define STDOUT 0
#define MAXBUFLEN 256
#define PORT 9034
#define MAXDATASIZE 256

int udpSocket=0;
int tcpSocket=0;
bool pendingInvite=false;
bool ingame=false;
bool turn = false;
/*
class game {
	virtual int handleInput(char * userInput, int inputLen,
						char * sendToServer, int* Serverlen,
						char * sentToPeer, int *peerLen)=0;
	virtual int HandlePeer (char * peerInput, int inputLen,
						char * sentToPeer, int *peerLen,
						char * sendToServer, int *serverLen,
						char * output, int *outputLen)=0;
	
};

class RockPaperScissors : public game
{
	
};

int RockPaperScissors::handleInput (char * userInput, int inputLen,
						char * sendToServer, int * Serverlen,
						char * sentToPeer, int * peerLen)
{
								// TODO
};

int RockPaperScissors::handlePeer(char * peerInput, int inputLen,
						char * sentToPeer, int * peerLen,
						char * sendToServer, int * serverLen,
						char * output, int * outputLen)
{
	// TODO						
}

class RockPaperScissors rps;
*/
int main(void)
{
	int user;
	short myport,theirport;
    int sockfd;
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    struct sockaddr_in server_addr;
	unsigned int addr_len;
	int numbytes;
    char buf[MAXBUFLEN];
	fd_set master;
	fd_set readfds;
	int fdmax;
	int score=0;


	printf("Please insert client number\n");
	scanf("%d",&user);
	if(user==1){
		myport=4095;
		theirport=4096;
	}
	else{
		myport=4096;
		theirport=4095;
	}


    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) goto out;

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(myport);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(udpSocket,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))) goto out2;
	FD_SET(STDIN, &master );
	FD_SET(udpSocket, &master);
	fdmax=udpSocket;
	for(;;){
		memset(buf,0, 255);
		readfds=master;
		select(fdmax+1,&readfds, NULL, NULL, NULL);
		if (FD_ISSET (STDIN, &readfds)) {
			int inputlen=read(STDIN, buf, MAXBUFLEN);
			printf("buf is %s \n",buf);
			if (!ingame){
				if (0 == strncmp("c ", buf, 2)) {
					struct hostent *he;
					if ((he=gethostbyname(buf+2)) == NULL) {  // get the host info
						perror("gethostbyname");
						exit(1);
					}
					if ((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
						perror("socket");
						exit(1);
					}
					server_addr.sin_family = AF_INET;    // host byte order
					server_addr.sin_port = htons(PORT);  // short, network byte order
					server_addr.sin_addr = *((struct in_addr *)he->h_addr);
					memset(&(server_addr.sin_zero), '\0', 8);  // zero the rest of the struct
					if (connect(tcpSocket, (struct sockaddr *)&server_addr,
											  sizeof(struct sockaddr)) == -1) {
						perror("connect");
						exit(1);
					}
					printf("Connected to the server successfully.\n");
					if (tcpSocket > fdmax){
						fdmax=tcpSocket;
					}
					FD_SET(tcpSocket, &master);
				}
				else if (0 == strncmp("lu", buf, 2)) {
					int outputlen = write(tcpSocket, buf, sizeof(buf));

				}
				else if (0 == strncmp("login", buf, 5)) {
					int outputlen = write(tcpSocket, buf, sizeof(buf));
				}
				else if (0 == strncmp("register ", buf, 9)) {
					int outputlen = write(tcpSocket, buf, sizeof(buf));

				}

				//play with random user
				/*else if (0 == strncmp("play\n", buf, 5)) {
								int outputlen = write(tcpSocket, buf, sizeof(buf));

				}*/
				//play with specific user
				else if (0 == strncmp("play", buf, 4)) {
					printf("play command\n");
					int outputlen = write(tcpSocket, buf, sizeof(buf));

				}

				else if (0 == strncmp("y", buf, 1) && pendingInvite) {
					pendingInvite=false;
					int outputlen = write(tcpSocket, buf, sizeof(buf));

				}

				else if (0 == strncmp("n", buf, 1) && pendingInvite) {
					pendingInvite=false;
					int outputlen = write(tcpSocket, buf, sizeof(buf));

				}




			}
			else if (ingame && turn){
				score++;
				if (score>3){
					char winmessage[256]="I won\n";
					int numbytes = sendto(udpSocket,winmessage,sizeof(winmessage),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
					turn=false;
					ingame=false;
					printf("The game is over you won. Please run game/lu to start another game\n");
					int outputlen = write(tcpSocket, winmessage, sizeof(winmessage));

				}
				else{
					int numbytes = sendto(udpSocket,buf,sizeof(buf),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
					printf("Sent UDP message in size of %d\n",numbytes);
					turn=false;
				}


			}


		}  


		if (udpSocket && FD_ISSET(udpSocket, &readfds)) {
			printf("Passed the udpsocket condition \n");
			addr_len = (unsigned int) sizeof(their_addr);
			/*if ((numbytes = recvfrom(udpSocket,buf, MAXBUFLEN-1, 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
						perror("recvfrom");
						exit(1);
			}*/
		int numbytes = recvfrom(udpSocket,buf, MAXBUFLEN-1, 0,(struct sockaddr *)&their_addr, &addr_len);
		printf("Got: %s\n", buf);
		turn=true;
		if (0 == strncmp("I won", buf, 5) && ingame) {
			turn=false;
			ingame=false;
			printf("The game is over you lost. Please run game/lu to start another game\n");

		}

		//printf("got packet from %s\n",inet_ntoa(their_addr.sin_addr));
		//printf("packet is %d bytes long\n",numbytes);
		//buf[numbytes] = '\0';
			/*
		char sendToServer[100], sentToPeer[100], outputToUser[100];
		int peerLen,serverLen, outputLen;
		rps.handlePeer(buf,numbytes,
					sendToPeer, &peerLen, 
					sendToServer, &serverLen,
					outputToUser, &outputLen);
		if (tcpSocket && serverLen)
			send(tcpSocket, sendToServer, serverLen,0);
		if (outputLen) write (STDOUT, outputToUser, outputLen);
		if (peerLen) sendto(udpSocket, sendToPeer, peerLen, 0,
         (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
		*/
		}

		if (tcpSocket && FD_ISSET(tcpSocket, &readfds)) {
			if ((numbytes=recv(tcpSocket, buf, MAXDATASIZE-1, 0)) > 0) {
				printf("Got from server:  %s \n", buf);
				char secondIPport[100];
					if (0 == strncmp("PlayS",buf, 5)){
						sscanf (buf, "PlayS\: %s",secondIPport);
						printf("Ready to play game with (Sender) %s\n",secondIPport);
						inet_ntop(AF_INET, &(their_addr.sin_addr), secondIPport, INET6_ADDRSTRLEN);
						their_addr.sin_family = AF_INET;         // host byte order
						their_addr.sin_port = htons(theirport);     // short, network byte order
						memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct
						ingame=true;
						turn=true;
						//start the game
						printf("say hey\n");


						/*char message[256]="This is the first UDP message\n";
						int numbytes = sendto(udpSocket,message,sizeof(message),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
						printf("Sent UDP message in size of %d\n",numbytes);*/

					}
					if (0 == strncmp("PlayR",buf, 5)){
						sscanf (buf, "PlayR\: %s",secondIPport);
						printf("Ready to play game with (Receiver) %s\n",secondIPport);
						inet_ntop(AF_INET, &(their_addr.sin_addr), secondIPport, INET6_ADDRSTRLEN);
						their_addr.sin_family = AF_INET;         // host byte order
						their_addr.sin_port = htons(theirport);     // short, network byte order
						memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct
						ingame=true;

					}


					if (0 == strncmp("Got game invitation from",buf, 24) && pendingInvite==false){
						pendingInvite=true;
						printf("Do you want to play? (Y/N)\n");

					}


			} 	else{
				FD_CLR(tcpSocket, &master);			
				tcpSocket = 0;
			}
		}
	}
out2:
    close(udpSocket);
out:
    return 0;
} 
