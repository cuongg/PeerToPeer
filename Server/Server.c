#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include "format.h"

static void *Handling(void *arg){
	int newfd;
	newfd = *( (int*) arg);
	free(arg);
	pthread_detach(pthread_self());
	unsigned char bufferr_receiver[2000];

	while (1) {

		/* code */
		int number_byte_read;
		bzero(bufferr_receiver, sizeof(bufferr_receiver));

		number_byte_read = read(newfd,bufferr_receiver,sizeof(bufferr_receiver));
		if((char)bufferr_receiver[0] == '0'){ // FileLocationRequest (bufferr_receiver[0] == 32)
			struct FileLocationRequest* request = &bufferr_receiver;
		}
		else{ //ListFileReport
			struct ListFileReport* request = &bufferr_receiver;
		}

	}

}
int main(int argc,char **agrv){
	int sockfd;
	int *newfd;

	//fd_set rfds;
	//fd_set afds;

	pthread_t tid;
	struct sockaddr_in sockaddr,cliaddr;
	socklen_t cliaddr_len ;
	char bufferrec[256];
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
		perror("socket!!!");
		//return 0;
	}
	int enable = 1;
	//set sockopt to many socket use a same port
  if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEPORT, &enable, sizeof(int)) < 0){
    	perror("setsockopt(SO_REUSEADDR) failed");
    }

	/*setup parameter for socket*/
	bzero(&sockaddr, sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(14459);
	sockaddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd, (struct sockaddr*) &sockaddr, sizeof(sockaddr)) < 0) {
		perror("bind!!!");
	}

	if(listen(sockfd, 5) < 0) {
		perror("listen");
	}


	while(1){
			newfd = malloc(sizeof(int));
			cliaddr_len = sizeof(cliaddr);

			printf("Waitting.... \n");
			fflush(stdout);
			*newfd = accept(sockfd, (struct sockaddr*) &cliaddr, &cliaddr_len);
			if(*newfd < 0) { //error
				perror("accept");
				continue;
			}
			else{
		 		struct sockaddr_in* pVAddr = &cliaddr;
				struct in_addr ipAddr = pVAddr->sin_addr;
				char str[4];
				inet_ntop( AF_INET, &ipAddr, str, 32);
				unsigned short port;
				port = ntohs(pVAddr->sin_port);
				printf("%s %hu \n",str,port);
				pthread_create(&tid, NULL, &Handling, (void *) newfd);
			}



	}
	return 0;

}
