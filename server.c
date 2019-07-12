#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char const *argv[]){
	int sock;
	int listener;
	char buf[1024];
	int bytes_read;
	struct sockaddr_in serv_addr;

	listener = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(3333);

	bind(listener, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(listener,1);

	while(1){
		sock = accept(listener, NULL, NULL);
		while(1){
			bytes_read = recv(sock, buf, 1024, 0);
			if(bytes_read <= 0) break;
			send(sock, buf, bytes_read, 0);
		}
		close(sock);
	}

	return 0;
}