#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int find_index(char a[], int num_elements, char value);

int main(int argc, char *argv[])
{
	fd_set rfds;
	fd_set master;
	int sockfd, newsockfd, portno;
	int count = 0;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int ret, text;
	int soc[26];
	int soc_count = 0;
	char name[26];

	for (int z = 0; z < 26; ++z)
	{
		name[z] = 'a' + z;
	}

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}
	FD_ZERO(&master);
	FD_SET(sockfd,&master);
	FD_ZERO (&rfds);
	FD_SET (sockfd, &rfds);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,5);

    while(1){
        	if(count % 1000 == 0) printf("======================= %d =======================\n", count);

        	rfds = master;
        	ret = select(FD_SETSIZE,&rfds,NULL,NULL,NULL);
        	printf("Events = %d\n", ret);
	
			/* check new connection */
        	if (FD_ISSET(sockfd,&rfds)){
				clilen = sizeof(cli_addr);
				newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                	printf("Newsockfd = %d\n", newsockfd);
             	if (newsockfd < 0){
                	error("ERROR on accept");
				}
				soc[soc_count] = newsockfd;
             	if (soc_count > 26){
                	error("ERROR maximum cilent");
				}	
				soc_count++;
				FD_SET (newsockfd,&master);
        	}
        	
			/*check message in socket buffer*/
        	for (int i = 0; i < FD_SETSIZE && ret > 0; ++i){
				if(FD_ISSET(i,&rfds)&&(i != sockfd)){
				ret--;
				if (ret < 0){
			    	error("ERROR reading from socket");
				} else {
					bzero(buffer,256);
					text = recv(i,buffer,255,0);
					printf("Here is the message: %s\n",buffer);
					
					/*ACK msg recive*/
					if (strncmp(buffer,"!!",2) == 0)
					{
						send(soc[find_index(name ,26 ,buffer[3])],buffer,5,0);
						
					}
					/*direct msg recive*/
					else if (strncmp(buffer,"/",1) == 0)
					{
						send(soc[find_index(name ,26 ,buffer[1])],buffer,256,0);
					}
					/*broadcast msg recive*/
					else{
						text = send(i,"I got your message\n",18,0);
						if (text < 0){
					    	error("ERROR writing to socket");
						}		
						for (int x = 0; x < FD_SETSIZE; ++x){
							if (x != i && FD_ISSET(x,&master) && (x != sockfd)){
								printf("send to monitor\n");
								text = send(x,buffer,18,0);
								if (text < 0){
									error("ERROR writing to socket");
								}
							}
						}
					}	
				}
            }
    	}
		count++;
	}

	close(newsockfd);
	close(sockfd);
	return 0; 
}

int find_index(char a[], int num_elements, char value)
{
   int i;
   for (i=0; i<num_elements; i++)
   {
	 if (a[i] == value)
	 {
	    return(i);  /* it was found return index */
	 }
   }
   return(-1);  /* if it was not found */
}
