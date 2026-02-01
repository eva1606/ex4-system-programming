#include "common.h"

typedef struct {
    int id;
    int port;
} arg_t;

static void *run_client(void *arg) {
    arg_t *a = arg;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket");

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(a->port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        die("connect");

    char msg[128];
    snprintf(msg, sizeof(msg), "hello from client %d", a->id);
    send(fd, msg, strlen(msg), 0);

    char buf[BUF_SIZE] = {0};
    recv(fd, buf, sizeof(buf), 0);

    printf("[client %d] received: %s\n", a->id, buf);
    close(fd);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    pthread_t t[5];
    arg_t args[5];

    for (int i = 0; i < 5; i++) {
        args[i].id = i + 1;
        args[i].port = port;
        pthread_create(&t[i], NULL, run_client, &args[i]);
    }

    for (int i = 0; i < 5; i++)
        pthread_join(t[i], NULL);

    return 0;
}
