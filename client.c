#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 		//close()
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define MAX_CONNECT_TRY 5

struct Client{
	char name[50];
	char address[50];
	int port;  
}Client;

void printClient(struct Client client){
	printf("%s %s:%d\n", client.name, client.address, client.port);
}

void registration(struct Client *client){
	printf("Name: ");
	scanf("%s", client->name);
	sprintf(client->address,"%s", "localhost");
	client->port = 1000;
	//printf("Address: ");
	//scanf("%s", client->address);
	//printf("Port: ");
	//scanf("%d", &client->port);
}

int main(){
	// Регистрация клиента
	struct Client client;
	struct Client* pc = &client;
	registration(pc);
	//printClient(client);
	
	// Соединение с сервером
	int sock;
	int conn = 0;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		exit(1);
	}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(3333);

	while(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		conn++;
		printf("Trying %d to connect to the server...\n",conn);
		if(conn == MAX_CONNECT_TRY){
			perror("Connect error");
			exit(2);
		}
		sleep(2);
	}

	// Отправка серверу первого сообщения и печать ответа
	char connection[50];
	sprintf(connection, "%s connected from %s:%d", client.name, client.address, client.port);
	printf("Trying to communicate with the server...\n");
	while(send(sock, connection, sizeof(connection),0) < 0){
		perror("send");
		sleep(1);
	}
	// Первое сообщение от сервера
	char buf_con[sizeof(connection)];
	if(recv(sock, buf_con, sizeof(connection), 0) > 0) printf("%s\n", buf_con);;
	
	// Процесс, принимающий сообщения от сервера
	char msg[256];
	switch(fork()){
		case -1:
			perror("fork");
			break;
		case 0:
			while(1){
				if(recv(sock, msg, sizeof(msg), 0) <= 0){
					printf("Server is down\n");
					_exit(2);
				}
				printf("%s\n", msg);
			}
	}

	// Отправка сообщений на сервер
	while(1){
		char message[50];
		char input[30];
		scanf("%s", input);
		sprintf(message, "%s: %s", client.name, input);
		if(send(sock, message, sizeof(message), 0) < 0 ) perror("Send");
	}

	close(sock);

	return 0;
}