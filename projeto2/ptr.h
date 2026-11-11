#ifndef PTR_H
#define PTR_H
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PTR_MAX 999

/* TCP é um fluxo: uma chamada pode transferir somente parte da mensagem. */
static int ptr_io(int fd, void *buffer, size_t tamanho, int enviar) {
    char *p = buffer;
    while (tamanho) {
        ssize_t n = enviar ? send(fd, p, tamanho, 0) : recv(fd, p, tamanho, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return 0;
        p += n;
        tamanho -= (size_t)n;
    }
    return 1;
}

static int ptr_enviar(int fd, const char *tipo, const char *corpo) {
    char mensagem[PTR_MAX + 9];
    size_t tamanho = strlen(corpo);
    if (strlen(tipo) != 3 || tamanho > PTR_MAX) return 0;
    snprintf(mensagem, sizeof mensagem, "%s|%03zu|%s", tipo, tamanho, corpo);
    return ptr_io(fd, mensagem, 8 + tamanho, 1);
}

/* Retorna -1 para cabeçalho inválido, pois não há como reenquadrar o fluxo. */
static int ptr_receber(int fd, char tipo[4], char corpo[PTR_MAX + 1]) {
    char h[8];
    if (!ptr_io(fd, h, sizeof h, 0)) return 0;
    if (h[3] != '|' || h[7] != '|' || !isdigit((unsigned char)h[4]) ||
        !isdigit((unsigned char)h[5]) || !isdigit((unsigned char)h[6])) return -1;
    int n = (h[4]-'0')*100 + (h[5]-'0')*10 + h[6]-'0';
    memcpy(tipo, h, 3); tipo[3] = '\0';
    if (!ptr_io(fd, corpo, (size_t)n, 0)) return 0;
    corpo[n] = '\0';
    if (memchr(corpo, '\0', (size_t)n)) return -1;
    return 1;
}
#endif
