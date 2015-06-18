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

	printf("listen\n");
     //////
/*  Do read = master (backup)
    Use ret
    FD_SET
    FD_CLR when not in use
    */ 
        while(1){
            	if(count % 1000 == 0){
            		printf("======================= %d =======================\n", count);

            	//printf("ret1 = %d\n", ret);
            	rfds = master;
            	ret = select(FD_SETSIZE,&rfds,NULL,NULL,NULL);
            	printf("Events = %d\n", ret);

            	// printf("for\n");
            	if (FD_ISSET(sockfd,&rfds)){
                    /* check new connection */
            		clilen = sizeof(cli_addr);
            		// printf("before accept\n");
            		newsockfd = accept(sockfd, 
            			(struct sockaddr *) &cli_addr, 
            			&clilen);
                    printf("Newsockfd = %d\n", newsockfd);
                 if (newsockfd < 0){
                    error("ERROR on accept");
                }
                FD_SET (newsockfd,&master);
            		// printf("accept\n");
            }

            for (int i = 0; i < FD_SETSIZE && ret > 0; ++i){
              if(FD_ISSET(i,&rfds)&&(i != sockfd)){
            			// printf("i = %d\n", i);
             // bzero(buffer,256);
                ret--;
                 if (ret < 0){
                    error("ERROR reading from socket");
                } else {
                    bzero(buffer,256);
                    text = recv(i,buffer,255,0);
                    printf("Here is the message: %s\n",buffer);
                //bzero(buffer,256);

                     text = send(i,"I got your message\n",18,0);
                            if (text < 0){
                            error("ERROR writing to socket");
                            }
                    for (int x = 0; x < FD_SETSIZE; ++x)
                    {
                        //printf("for\n");
                        if (x != i && FD_ISSET(x,&master) && (x != sockfd))
                        {
                            printf("send to monitor\n");
                            text = send(x,buffer,18,0);
                            if (text < 0){
                            error("ERROR writing to socket");
                            //printf("send\n");
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
