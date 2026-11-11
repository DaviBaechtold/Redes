#include "ptr.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>

static int trocar(int fd, const char *tipo, const char *corpo) {
    char resposta[4], dados[PTR_MAX + 1];
    printf("Enviado: %s|%03zu|%s\n", tipo, strlen(corpo), corpo);
    if (!ptr_enviar(fd, tipo, corpo) || ptr_receber(fd, resposta, dados) != 1) {
        fprintf(stderr, "Conexao encerrada ou resposta invalida\n"); return 0;
    }
    printf("Resposta: %s|%03zu|%s\n", resposta, strlen(dados), dados);
    fflush(stdout);
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "Uso: %s IP_DO_SERVIDOR\n", argv[0]); return 1; }
    signal(SIGPIPE, SIG_IGN);
    struct sockaddr_in servidor = {0};
    servidor.sin_family = AF_INET; servidor.sin_port = htons(8080);
    if (inet_pton(AF_INET, argv[1], &servidor.sin_addr) != 1) { fprintf(stderr, "IP invalido\n"); return 1; }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }
    if (connect(fd, (struct sockaddr *)&servidor, sizeof servidor) < 0) { perror("connect"); close(fd); return 1; }
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    puts("Conectado. Velocidade a cada 5 segundos; digite REQ ou SAIR.");
    time_t proximo = time(NULL);
    for (;;) {
        time_t agora = time(NULL);
        if (agora >= proximo) {
            char velocidade[4]; snprintf(velocidade, sizeof velocidade, "%d", 60 + rand() % 31);
            if (!trocar(fd, "VEL", velocidade)) break;
            proximo = time(NULL) + 5;
        }
        fd_set leitura; FD_ZERO(&leitura); FD_SET(STDIN_FILENO, &leitura);
        struct timeval espera = { .tv_sec = proximo > time(NULL) ? proximo - time(NULL) : 0, .tv_usec = 0 };
        int pronto = select(STDIN_FILENO + 1, &leitura, NULL, NULL, &espera);
        if (pronto < 0) { if (errno == EINTR) continue; perror("select"); break; }
        if (pronto) {
            char comando[128];
            if (!fgets(comando, sizeof comando, stdin)) break;
            comando[strcspn(comando, "\n")] = '\0';
            if (!strcmp(comando, "SAIR")) break;
            if (!strcmp(comando, "REQ")) { if (!trocar(fd, "REQ", "")) break; }
            else puts("Comando disponivel: REQ ou SAIR");
        }
    }
    close(fd); return 0;
}
