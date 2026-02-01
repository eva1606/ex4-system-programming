#include "common.h"

static int connected_clients = 0;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static ssize_t send_all(int fd, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t n = send(fd, p + total, len - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

static void *client_thread(void *arg) {
    int fd = *(int *)arg;
    free(arg);

    char buf[BUF_SIZE];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n > 0) {
        for (ssize_t i = 0; i < n; i++)
            buf[i] = toupper((unsigned char)buf[i]);

        send_all(fd, buf, n);
    }

    close(fd);

    pthread_mutex_lock(&clients_mutex);
    connected_clients--;
    pthread_mutex_unlock(&clients_mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) die("socket");

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) die("bind");
    if (listen(server_fd, 10) < 0) die("listen");

    printf("Server listening on 127.0.0.1:%d\n", port);

    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, NULL, NULL);
        if (*client_fd < 0) die("accept");

        pthread_mutex_lock(&clients_mutex);
        connected_clients++;
        pthread_mutex_unlock(&clients_mutex);

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, client_fd);
        pthread_detach(tid);
    }
}
