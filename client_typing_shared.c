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

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char const *argv[])
{
	//share sockfd
	    //_______________________shared memory_____________________
    struct region {        /* Defines "structure" of shared memory */
	    int len;
	    char buffer_write[MAX_LEN];
	};
	struct region *rptr;
	//char * temp_rptr;
	int shmfd;

	shmfd = shm_open(argv[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
	   for example, rptr->len */

    //_______________________shared memory_____________________
	// int sockfd;
	while(1){
        printf("Please enter the message: ");
		// bzero(rptr->buffer_write,256);
		fgets(rptr->buffer_write,255,stdin);
		printf("%s\n", rptr->buffer_write);

		//write(sockfd,buffer,strlen(buffer));
	}
}
