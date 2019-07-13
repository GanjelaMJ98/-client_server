#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 		//close()
#include <errno.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_CLIENTS 10
int Connections[MAX_CLIENTS];		
int Counter = 0;

void* ClientHandler(void *arg){
	int client = *((int*)arg);
	char msg[256];
	int bytes_read = 0;
	while(1){
		bytes_read = recv(client, msg, sizeof(msg), 0);
		if(bytes_read <= 0) continue;
		printf("--------\n");
		printf("%s\n", msg);
		printf("--------\n");

		/*	Дублирование всем, кроме отправителя
		 	TODO: для первого клиента Counter = 1, для второго 2 и тд.
				Необходимо чтобы при подключении 2х и более клиентов Counter во всех потоках
				был равен количеству подключенных клиентов.	
		*/
		for(int i = 0; i < Counter; i++){
			if(Connections[i] == client) continue;
			send(Connections[i], msg, sizeof(msg), 0);
		}
	}
}


int main(int argc, char const *argv[]){
	int port = 3333;
	int sock, tmp;
	int listener;
	char buf[1024];
	int bytes_read;
	struct sockaddr_in serv_addr;
	pthread_t threads[MAX_CLIENTS];

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
	for(int i = 0; i < MAX_CLIENTS; i++){

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
			tmp = pthread_create(&threads[i], NULL, ClientHandler, &Connection);
			if(tmp != 0){
				perror("pthread_create");
				exit(2);
			}
		}	
	}
	close(listener);
	for(int j = 0; j < MAX_CLIENTS; j++){
		tmp = pthread_join(threads[j], NULL);
		if(tmp != 0){
			perror("join");
			exit(3);
		}
	}
	
	return 0;
}