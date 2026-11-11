/*

servidor.c
*/


#include<stdio.h>
#include<string.h> //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h> //write

int main(int argc , char *argv[])
{

     int socket_desc , client_sock , c , read_size;
     struct sockaddr_in server , client;
     char client_message[2000];//Criar socket

     socket_desc = socket(AF_INET , SOCK_STREAM , 0);

     if (socket_desc == -1)
     {
           printf("Nao foi possivel criar o socket!\n");
     }

     puts("Socket criado!\n");

     //Preparar a estrutura sockaddr_in
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = INADDR_ANY;
     server.sin_port = htons( 10000 );

     //Bind
     if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
     {
           //Mensagem de erro
           perror("bind falhou. Erro!\n");
           return 1;
     }
     puts("bind OK!\n");

     //Listen
     listen(socket_desc , 3);

     //Esperando cliente.....
     puts("Esperando conexoes de clientes...\n");

     c = sizeof(struct sockaddr_in);
     client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

     if (client_sock < 0)
     {
             perror("accept falhou!\n");
             return 1;
     }

     puts("Conexao de cliente aceita!\n");
     //Recebe uma mensagem do cliente

     while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
     {
           //Envia mensagem de volta ao cliente

           printf("Cliente mandou: %s\n", client_message);
           fflush(stdout);

           write(client_sock , client_message , strlen(client_message));

           memset(client_message,'\0',sizeof(client_message));

     }

     if(read_size == 0)
     {
          puts("Cliente desconectado!\n");
          fflush(stdout);
     }

     else if(read_size == -1)
     {
           perror("recv falhou!\n");
     }

      return 0;
}
