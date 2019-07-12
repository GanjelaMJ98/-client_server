#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char message[] = "Hello\n";
char buf[sizeof(message)];

int main(){
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_LOOPBACK;
	addr.sin_port = htons(3333);

	connect(sock, (struct sockaddr*)&addr, sizeof(addr));

	send(sock, message, sizeof(message), 0);
	recv(sock, buf, sizeof(message), 0);

	printf("%s\n", buf);
	close(sock);

	return 0;
}