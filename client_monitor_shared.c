/*Client monitor for event typing and reading*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/select.h>
#include <time.h>

#define MAX_LEN 256
struct region {        /* Defines "structure" of shared memory */
	char buffer_write[MAX_LEN];
	int send;
	int len;
};

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	fd_set rfds;
	fd_set master;
	int sockfd, portno, n ;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	struct timeval tv;
	char buffer_read[256];
	char buffer[256];

    //_______________________shared memory_____________________

	struct region *rptr;
	int shmfd;

	shmfd = shm_open(argv[3], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1){
		error("ERROR opening ");
		exit(1);
	}

	ftruncate(shmfd, sizeof(struct region));

	rptr = (struct region *)mmap(NULL, sizeof(struct region),PROT_READ | PROT_WRITE, 
				MAP_SHARED, shmfd, 0);
	if (rptr == MAP_FAILED){
		error("ERROR mapping ");
		exit(1);
	}
	
	/* Now we can refer to mapped region using fields of rptr;
	   for example, rptr->buffer_write */

    //_______________________shared memory_____________________

    /* Wait up to one microseconds. */
	tv.tv_sec = 1;
	tv.tv_usec = 1;

	if (argc < 4) {
		fprintf(stderr,"usage %s hostname port client_name\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
		error("ERROR opening socket");
	
	FD_ZERO (&master);
	FD_SET (sockfd, &master);
	FD_ZERO (&rfds);
	FD_SET (sockfd, &rfds);

	server = gethostbyname(argv[1]);

	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);


	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	rptr->send = 1;
	while(1){
		if(rptr->buffer_write[0] != 0 && rptr->buffer_write[rptr->len] == '\n'){
			send(sockfd,rptr->buffer_write,strlen(rptr->buffer_write),0);
			rptr->send = 1;
		}

        rfds = master;
		n = select(FD_SETSIZE,&rfds,NULL,NULL,&tv);
		if (n > 0){
			bzero(buffer_read,256);
			recv(sockfd,buffer_read,255,0);
			printf("%s\n",buffer_read);
		}	
		else if (n < 0) 
			error("ERROR reading from socket");

	}
	close(sockfd);
	return 0;
}
