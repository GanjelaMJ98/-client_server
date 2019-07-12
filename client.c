#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 		//close()
#include <errno.h>

char message[] = "Hello\n";
char buf[sizeof(message)];

int main(){
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		exit(1);
	}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(3333);

	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("connect");
		exit(2);
	}
	printf("Sending...\n");
	send(sock, message, sizeof(message), 0);
	printf("Reception...\n");
	recv(sock, buf, sizeof(message), 0);

	printf("%s\n", buf);
	close(sock);

	return 0;
}