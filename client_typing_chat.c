/*Client for typing*/

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
    int typed;
    int len;
};
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char const *argv[])
{
	//_______________________shared memory_____________________
    
	struct region *rptr;
	
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
	   for example, rptr->buffer_write */

    //_______________________shared memory_____________________

	rptr->typed = 0;
	rptr->send = 1;
	while(1){
		if (strcmp(rptr->buffer_write,"/exit\n") == 0)
		{
			break;
		}else
			if(rptr->send == 1){
				bzero(rptr->buffer_write,256);
				printf("Please enter the message: ");
				fgets(rptr->buffer_write,255,stdin);
				rptr->len = strlen(rptr -> buffer_write) - 1;
				rptr->send = 0;
				rptr->typed = 1;
			}
	}
	usleep(100);
	bzero(rptr->buffer_write,256);
}
