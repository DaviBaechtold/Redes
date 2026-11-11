/*
cliente.c
*/

#include<stdio.h> //printf
#include<string.h> //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h> //inet_addr
#include<stdlib.h>
#include<unistd.h>

int main(int argc , char *argv[])
{
int sock;
struct sockaddr_in server;
char *message , *server_reply;

message= malloc(2000);
server_reply= malloc(2000);

//Criar socket
sock = socket(AF_INET , SOCK_STREAM , 0);
if (sock == -1)
{
printf("Nao pude criar o socket!\n");
}
puts("Socket criado!\n");
server.sin_addr.s_addr = inet_addr("127.0.0.1");
server.sin_family = AF_INET;
server.sin_port = htons( 10000 );
//Conectar ao servidor
if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
{
perror("conexao falhou. Error!\n");
return 1;
}
puts("Conectado ao servidor!\n");
//mantendo comunicacao com o servidor
while(1)
{

memset(message,'\0', sizeof(message));

printf("Entre com uma mensagem : ");

fgets(message,2000,stdin);

if((strlen(message)>0) && (message[strlen(message)-1] == '\n'))
     message[strlen(message)-1] = '\0';

//Enviando mensagem ao servidor

printf("Cliente enviou: %s\n",message);

if( send(sock , message , strlen(message) , 0) < 0)
{
puts("Send falhou!\n");
return 1;
}
//Recebendo retorno do servidor
if( recv(sock , server_reply , 2000 , 0) < 0)
{
puts("recv falhou!\n");
break;
}

puts("Servidor respondeu: ");

puts(server_reply);

memset(server_reply,'\0', sizeof(server_reply));

}

free(message);
free(server_reply);

close(sock);
return 0;
}
