#include "ptr.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void *atender(void *arg) {
    int fd = *(int *)arg;
    free(arg);
    char tipo[4], corpo[PTR_MAX + 1];
    int resultado;
    while ((resultado = ptr_receber(fd, tipo, corpo)) == 1) {
        printf("Recebido %s: %s\n", tipo, corpo);
        fflush(stdout);
        if (!strcmp(tipo, "REQ")) {
            if (!ptr_enviar(fd, *corpo ? "ERR" : "RES", *corpo ? "Payload incorreto" : "Lentidao ao longo da rota")) break;
        } else if (!strcmp(tipo, "VEL")) {
            int valido = *corpo != '\0';
            for (size_t i = 0; corpo[i]; i++) if (!isdigit((unsigned char)corpo[i])) valido = 0;
            if (strlen(corpo) > 3 || (valido && atoi(corpo) > 999)) valido = 0;
            if (!ptr_enviar(fd, valido ? "ACK" : "ERR", valido ? "" : "Payload incorreto")) break;
        } else if (!ptr_enviar(fd, "ERR", "Comando invalido")) break;
    }
    if (resultado == -1) ptr_enviar(fd, "ERR", "Cabecalho invalido");
    close(fd);
    return NULL;
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);
    int servidor = socket(AF_INET, SOCK_STREAM, 0), opcao = 1;
    if (servidor < 0) { perror("socket"); return 1; }
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &opcao, sizeof opcao);
    struct sockaddr_in endereco = {0};
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco.sin_port = htons(8080);
    if (bind(servidor, (struct sockaddr *)&endereco, sizeof endereco) < 0 || listen(servidor, 32) < 0) {
        perror("bind/listen"); close(servidor); return 1;
    }
    puts("Servidor TCP escutando na porta 8080"); fflush(stdout);
    for (;;) {
        struct sockaddr_in cliente; socklen_t tamanho = sizeof cliente;
        int fd = accept(servidor, (struct sockaddr *)&cliente, &tamanho);
        if (fd < 0) { if (errno != EINTR) perror("accept"); continue; }
        printf("Cliente %s:%u conectado\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port)); fflush(stdout);
        int *arg = malloc(sizeof *arg);
        if (!arg) { perror("malloc"); close(fd); continue; }
        *arg = fd;
        pthread_t thread;
        int erro = pthread_create(&thread, NULL, atender, arg);
        if (erro) { fprintf(stderr, "pthread_create: %s\n", strerror(erro)); free(arg); close(fd); continue; }
        pthread_detach(thread);
    }
}
