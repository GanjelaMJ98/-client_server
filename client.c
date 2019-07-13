#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 		//close()
#include <errno.h>
#include <string.h>
#include <stdio.h>

char message[] = "Hello\n";
char buf[sizeof(message)];


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

	// Отправка серверу первого сообщения и печать ответа
	char connection[50];
	sprintf(connection, "%s connected from %s:%d", client.name, client.address, client.port);
	printf("%s",connection);

	char buf_con[sizeof(connection)];
	send(sock, connection, sizeof(connection),0);
	recv(sock, buf_con, sizeof(connection), 0);
	printf("%s\n", buf_con);

	// Поток, принимающий сообщения от сервера
	char msg[256];
	switch(fork()){
		case -1:
			perror("fork");
			break;
		case 0:
			
			while(1){
				recv(sock, msg, sizeof(msg), 0);
				printf("%s\n", msg);
			}

	}
	// Отправка сообщений на сервер
	while(1){
		char message[50];
		char input[30];
		scanf("%s", input);
		sprintf(message, "%s: %s", client.name, input);
		send(sock, message, sizeof(message), 0);
	}

	close(sock);

	return 0;
}