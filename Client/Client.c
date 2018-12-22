#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "format.h"

int main(int argc,char **agrv){

	char buffsz[10];

	int	sockfd;

	struct sockaddr_in	servaddr;
	char ipsever[20];
	printf("Please enter the ipsever: ");
	bzero(ipsever,sizeof(ipsever));
	fgets(ipsever,sizeof(ipsever),stdin);
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
		perror("socket!!!");
		return 0;
	}

	/*setup parameter for socket*/
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(14459);
	servaddr.sin_addr.s_addr =  inet_addr(ipsever);


	/*check and set buffer's size*/
	int show_buffer_size = 0;
	socklen_t	optlen;
	optlen = sizeof(show_buffer_size);
	if(getsockopt(sockfd, SOL_SOCKET,SO_RCVBUF, &show_buffer_size, &optlen) < 0)
	perror("SO_RCVBUF getsockopt error");
	printf("check buffer's size defaults : %d\n",show_buffer_size);
	printf("buffer's size: ");
	bzero(buffsz,sizeof(buffsz));
	fgets(buffsz,sizeof(buffsz),stdin);
	int buffsize = atol(buffsz);
	if (setsockopt(sockfd, SOL_SOCKET,SO_RCVBUF, &buffsize, sizeof(buffsize)) < 0){
			perror("setsockopt SO_RCVBUF failed");
	}
	if(getsockopt(sockfd, SOL_SOCKET,SO_RCVBUF, &show_buffer_size, &optlen) < 0)
		perror("SO_RCVBUF getsockopt error");
	printf("check buffer's size (after setting it): %d\n",show_buffer_size);


	printf("choose option: 1 <update list file> ; 2 <download file>; 3 <quit> \n option:");
	int option = 0;
	scanf("%d",&option);
	
	/**/
	while(1){
		if(option == 1){
			/*code for update list file feature*/
			if( connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0 )
			{
				perror("connect");
				return 0;
			}
		}
		else if(option == 2){
			/*code for send request to download file */
			if( connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0 )
			{
				perror("connect");
				return 0;
			}
			char name[100] = "./";
			char mode[2] = "wb";
			char buffer[256];

			struct FileLocationRequest request_pkt ;
			request_pkt.type = '1';
			printf("Please enter the file's name: ");
			bzero(buffer,sizeof(buffer));
			strcpy(name, "./");
			fgets(buffer,256,stdin);



		}
		else if(option == 3){
			return 0;
		}
		else{
			/**/
			printf("wrong option");

		}
	}

	return 0;

}
