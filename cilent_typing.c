#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>


int main(int argc, char const *argv[])
{
	//share sockfd
	char buffer[256];
	// int sockfd;
	while(1){
        printf("Please enter the message: ");
		bzero(buffer,256);
		fgets(buffer,255,stdin);

		//write(sockfd,buffer,strlen(buffer));
	}
}
