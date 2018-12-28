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

		/* code */

		bzero(bufferr_receiver, sizeof(bufferr_receiver));

		read(newfd,bufferr_receiver,sizeof(bufferr_receiver));
		if((char)bufferr_receiver[0] == '0'){ // FileLocationRequest (bufferr_receiver[0] == 32)
					unsigned char packet_respond[2000];
					bzero(packet_respond, sizeof(packet_respond));
					int point_respon = 5;// location to save ip addresses in packet_respond
					char md5[8],name[16] = "./";
					//struct in_addr IP[256];
					char num_of_host = 0;
					int size_of_file = 0;
					memcpy(&md5,bufferr_receiver[1],sizeof(uint8_t));

					strcpy(name, "./");
					strcat(name,md5);
					if(access(name,F_OK) == -1){


						printf("file not exists!");
						int zero = 0;
						char zero_char = 0;
						memcpy(packet_respond,&zero,sizeof(int));
						memcpy(&(packet_respond[4]),&zero_char,sizeof(char));

					}
					else{
						FILE *md5_file = fopen(md5,"r");
						while(1){

							if (feof(md5_file)){
									break;
							}
							struct in_addr ip;
							char line[30];
							fgets(line, 40, (FILE*)md5_file);
							char *ipstr,*sizestr;
							ipstr = (char *) malloc(sizeof(char) * 18);// maximum of ip addr is 15 characters
							sizestr = (char *) malloc(sizeof(char) * 20);

							int space_index = strcspn (line," ");
							memcpy(ipstr,line,space_index);
							memcpy(sizestr,&(line[space_index+1]),(int)strlen(line)-space_index);

							int size_ = atoi(sizestr);
							if(size_ != size_of_file){
								size_of_file = size_;
								memcpy(packet_respond,&size_of_file,sizeof(int));// write size of file
							}
							inet_pton(AF_INET,ipstr,&ip);
							num_of_host++;
							memcpy(&(packet_respond[point_respon]),&ip,sizeof(struct in_addr)); // write one ip address
							point_respon += sizeof(struct in_addr);

							free(ipstr);
							free(sizestr);


						}
						memcpy(&(packet_respond[4]),&num_of_host,sizeof(char));// write total number of ip addresses
						fclose(md5_file);

					}
					write(newfd,packet_respond,sizeof(packet_respond));
		}
		else{ //ListFileReport
			int point = 1;
			uint8_t quantity;
		//	struct ListFileReport *report;
			//report = (struct ListFileReport *) malloc(sizeof(struct ListFileReport));
			memcpy(&quantity,bufferr_receiver[point],sizeof(uint8_t));
			point++;
			int size_file_zone;
			size_file_zone = 1 + quantity*8;
			char md5[8];
			int size_file = 0;
			for(int i = 0;i < quantity;i++){
					memcpy(md5,bufferr_receiver[point],8);
					point+=8;
					FILE *md5_file = fopen(md5,"a");

					struct sockaddr_in addr;
				  socklen_t addr_size = sizeof(struct sockaddr_in);
				  getpeername(newfd, (struct sockaddr *)&addr, &addr_size);
				  char *clientip;
					clientip = (char *) malloc(sizeof(char)*20);
				  strcpy(clientip, inet_ntoa(addr.sin_addr));

					//fwrite(clientip,sizeof(char),20,md5_file);
					fprintf(md5_file,"%s ",clientip);
					//memcpy(&size_file,bufferr_receiver[size_file_zone + i*sizeof(int)],sizeof(int));
					memcpy(&size_file,bufferr_receiver[size_file_zone + i*4],4);
				//	char size[20];
					//sprintf(str, "%d", someInt);
					fprintf(md5_file,"%d\n",size_file);
					fclose(md5_file);
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

			//printf("Waitting.... \n");
			//fflush(stdout);
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
