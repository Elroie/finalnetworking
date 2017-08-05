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

#define MYPORT 4950    // the port users will be connecting to
#define STDIN 0
#define STDOUT 0
#define MAXBUFLEN 100
#define PORT 9034
#define MAXDATASIZE 100

int udpSocket=0;
int tcpSocket=0;
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
    int sockfd;
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    struct sockaddr_in server_addr;
	int addr_len, numbytes;
    char buf[MAXBUFLEN];
	fd_set master;
	fd_set readfds;
	int fdmax;

    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) goto out;

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(MYPORT);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(udpSocket,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))) goto out2;
	FD_SET(STDIN, &master );
	FD_SET(udpSocket, &master);
	fdmax=udpSocket;
	for(;;){
		readfds=master;
		select(fdmax,&readfds, NULL, NULL, NULL);
		if (FD_ISSET (STDIN, &readfds)) {
			int inputlen=read(STDIN, buf, MAXBUFLEN);
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
				memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct 
				if (connect(tcpSocket, (struct sockaddr *)&server_addr,
                                          sizeof(struct sockaddr)) == -1) {
					perror("connect");
					exit(1);
				}
				printf ("Connected to the server successfully.");
				FD_SET(tcpSocket, &master);
			}
			else if (0 == strncmp("lu", buf, 2)) {
				int outputlen = write(tcpSocket, buf, 100);

			}
			else if (0 == strncmp("login", buf, 5)) {
				int outputlen = write(tcpSocket, buf, 100);
			}
			else if (0 == strncmp("register ", buf, 9)) {
				int outputlen = write(tcpSocket, buf, 100);

			}
			

		}  
		/*if (udpSocket && FD_ISSET(udpSocket, &readfds)) {
			addr_len = sizeof(struct sockaddr);
			if ((numbytes=recvfrom(sockfd,buf, MAXBUFLEN-1, 0,
                       (struct sockaddr *)&their_addr, &addr_len)) == -1) {
						perror("recvfrom");
						exit(1);
			}
		//printf("got packet from %s\n",inet_ntoa(their_addr.sin_addr));
		//printf("packet is %d bytes long\n",numbytes);
		//buf[numbytes] = '\0';
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
		}
		*/
		if (tcpSocket && FD_ISSET(tcpSocket, &readfds)) {
			if ((numbytes=recv(tcpSocket, buf, MAXDATASIZE-1, 0)) > 0) {
				// TODO
			} else {
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
