#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 		//close()
#include <errno.h>
#include <stdio.h>

int Connections[10];		
int Counter = 0;

void ClientHandler(int index, int* c){
	char msg[256];
	int bytes_read = 0;
	while(1){
		int Counter = *c;

		bytes_read = recv(Connections[index], msg, sizeof(msg), 0);
		if(bytes_read <= 0) continue;
		printf("_____%d\n", Counter);
		printf("%s\n", msg);
		printf("_____\n");

		/*	Дублирование всем, кроме отправителя
		 	TODO: для первого клиента Counter = 1, для второго 2 и тд.
				Необходимо чтобы при подключении 2х и более клиентов Counter во всех потоках
				был равен количеству подключенных клиентов.	
		*/
		for(int i = 0; i < Counter; i++){
			if(i == index) continue;
			send(Connections[i], msg, sizeof(msg), 0);
		}
	}
}


int main(int argc, char const *argv[]){
	int port = 3333;
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
	serv_addr.sin_port = htons(port);

	if(bind(listener, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind");
		exit(2);
	}

	listen(listener,SOMAXCONN);

	printf("Server started on ort %d, sock: %d\n", port, listener);
	

	int Connection;
	for(int i = 0; i < 10; i ++){

		// Соединение с клиентом
		Connection = accept(listener, NULL, NULL);
		if(Connection < 0){
			perror("accept");
			exit(3);
		}else{
			printf("Client Connected: %d\n", Connection);
			char msg[50] = "Hello";
			send(Connection, msg, sizeof(msg), 0);
			Connections[i] = Connection;
			Counter++;
			
			// Создание потока обработки сообщений
			switch(fork()){
				case -1:
					perror("fork");
					break;
				case 0:
					ClientHandler(i, &Counter);
					_exit(0);
			}
		}	
	}
	close(listener);
	return 0;
}