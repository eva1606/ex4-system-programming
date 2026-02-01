#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <pthread.h>

#define BUF_SIZE 4096

static inline void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

#endif
