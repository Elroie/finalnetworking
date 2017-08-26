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
#include <string.h>
#include <stdlib.h>

using namespace std;

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
	int mynumber;
	int opponentguess;
	int opponentres;
	int dice1;
	int dice2;
	int opponentscore;


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
				int numbytes = sendto(udpSocket,buf,sizeof(buf),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
				turn=false;
			}


		}  


		if (udpSocket && FD_ISSET(udpSocket, &readfds)) {
			addr_len = (unsigned int) sizeof(their_addr);
			/*if ((numbytes = recvfrom(udpSocket,buf, MAXBUFLEN-1, 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
						perror("recvfrom");
						exit(1);
			}*/
		int numbytes = recvfrom(udpSocket,buf, MAXBUFLEN-1, 0,(struct sockaddr *)&their_addr, &addr_len);

//		cout << "buf is" << buf << endl;

		if (0 == strncmp("I lost", buf, 5) && ingame) {
			turn=false;
			ingame=false;
			printf("The game is over, you won. Please run game/lu to start another game\n");
			char winmessage[256];
			sprintf(winmessage, "i won %s\n", std::to_string(score).c_str());
			int outputlen = write(tcpSocket, winmessage, sizeof(winmessage));

		}
		else if (0 == strncmp("I won", buf, 4) && ingame) {
			turn=false;
			ingame=false;
			printf("The game is over. you lost. Please run game/lu to start another game\n");

		}
		else if  (0 == strncmp("bad guess", buf, 9) && ingame){
			printf("Choose number between  2 to 12 \n");
			scanf("%d",&mynumber);
			printf("It's your opponent turn... waiting for his roll...\n");
		}
		else {


			opponentres=atoi(buf);

			dice1 = div(opponentres, 100).quot;
			dice2 = div(opponentres % 100, 10).quot;
			opponentscore = opponentres % 10;

			cout << "Dice no 1: " << dice1 << " Dice no 2: " << dice2 << endl;

			if(dice1 + dice2 == mynumber){
				score++;
				cout << "You scored the correct number!" << endl;
			}else{
				cout << "You missed!" << endl;
			}
			cout << "You: " << score << " Opponent: " << opponentscore << endl;

			if(opponentscore >= 3 && score < 3){
				char winmessage[256]="I lost\n";
				int numbytes = sendto(udpSocket,winmessage,sizeof(winmessage),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
				ingame=false;
				printf("The game is over. you lost. Please run game/lu to start another game\n");

			}else if(score >= 3 && opponentscore < 3){
				char winmessage[256]="I won\n";
				int numbytes = sendto(udpSocket,winmessage,sizeof(winmessage),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
				ingame=false;
				printf("The game is over, you won. Please run game/lu to start another game\n");

				sprintf(winmessage, "i won %s\n", std::to_string(score).c_str());
				int outputlen = write(tcpSocket, winmessage, sizeof(winmessage));
			}
			else{
				char badguess[256]="bad guess\n";
				int numbytes = sendto(udpSocket,badguess,sizeof(badguess),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
				turn=true;


				printf("Choose number between  2 to 12 \n");
				scanf("%d",&mynumber);

				printf("Rolling the dices.. no more bets please\n");

				dice1 = rand() % 6 + 1;
				dice2 = rand() % 6 + 1;

				cout << "Dice no 1: " << dice1 << " Dice no 2: " << dice2 << endl;

				if(dice1 + dice2 == mynumber){
					score++;
					cout << "You scored the correct number! your score is: " << score << endl;
				}else{
					cout << "You missed! your score is: " << score << endl;
				}


				char roll[3] = {};
				strcat(roll , std::to_string(dice1).c_str());
				strcat(roll , std::to_string(dice2).c_str());
				strcat(roll , std::to_string(score).c_str());

				int numbytes2 = sendto(udpSocket,roll,sizeof(roll),0,(struct sockaddr *)&their_addr,sizeof(their_addr));

//				char badguess[256]="bad guess\n";
//				int numbytes = sendto(udpSocket,badguess,sizeof(badguess),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
//				turn=true;
//				printf("Opponent failed. Please guess your opponent number\n");
//
//				dice1 = rand() % 6 + 1;
//				dice2 = rand() % 6 + 1;
//
//				char roll[2] = {};
//				strcat(roll , std::to_string(dice1).c_str());
//				strcat(roll , std::to_string(dice2).c_str());
//				int numbytes1 = sendto(udpSocket,roll,sizeof(roll),0,(struct sockaddr *)&their_addr,sizeof(their_addr));
			}


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
				char secondIPport[100];
					if (0 == strncmp("PlayS",buf, 5)){
						sscanf (buf, "PlayS\: %s",secondIPport);
						inet_ntop(AF_INET, &(their_addr.sin_addr), secondIPport, INET6_ADDRSTRLEN);
						their_addr.sin_family = AF_INET;         // host byte order
						their_addr.sin_port = htons(theirport);     // short, network byte order
						memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct
						ingame=true;
						turn=true;
						//start the game
						printf("Chicago Game is starting!!!\n");
						printf("Choose number between  2 to 12 \n");
						scanf("%d",&mynumber);

						printf("Rolling the dices.. no more bets please\n");

						dice1 = rand() % 6 + 1;
						dice2 = rand() % 6 + 1;

						cout << "Dice no 1: " << dice1 << " Dice no 2: " << dice2 << endl;

						if(dice1 + dice2 == mynumber){
							score++;
							cout << "You scored the correct number! your score is: " << score << endl;
						}else{
							cout << "You missed! your score is: " << score << endl;
						}


						char roll[3] = {};
						strcat(roll , std::to_string(dice1).c_str());
						strcat(roll , std::to_string(dice2).c_str());
						strcat(roll , std::to_string(score).c_str());

						int numbytes2 = sendto(udpSocket,roll,sizeof(roll),0,(struct sockaddr *)&their_addr,sizeof(their_addr));

					}
					else if (0 == strncmp("PlayR",buf, 5)){
						sscanf (buf, "PlayR\: %s",secondIPport);
						inet_ntop(AF_INET, &(their_addr.sin_addr), secondIPport, INET6_ADDRSTRLEN);
						their_addr.sin_family = AF_INET;         // host byte order
						their_addr.sin_port = htons(theirport);     // short, network byte order
						memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct
						ingame=true;
						printf("Chicago Game is starting!!!\n");
						printf("Choose number between  2 to 12 \n");
						scanf("%d",&mynumber);
						printf("It's your opponent turn... waiting for his roll...\n");

					}
					else if (0 == strncmp("Got game invitation from",buf, 24) && pendingInvite==false){
						pendingInvite=true;
						printf("Do you want to play? (Y/N)\n");

					}
					else{
						printf("Got from server:  %s \n", buf);
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
