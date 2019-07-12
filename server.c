#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 		//close()
#include <errno.h>

int main(int argc, char const *argv[]){
	int sock;
	int listener;
	char buf[1024];
	int bytes_read;
	struct sockaddr_in serv_addr;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0) {
		perror("socket");
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(3333);

	if(bind(listener, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind");
		exit(2);
	}

	listen(listener,1);

	while(1){
		sock = accept(listener, NULL, NULL);
		if(sock < 0){
			perror("accept");
			exit(3);
		}
		while(1){
			bytes_read = recv(sock, buf, 1024, 0);
			if(bytes_read <= 0) break;
			send(sock, buf, bytes_read, 0);
		}
		close(sock);
	}

	return 0;
}