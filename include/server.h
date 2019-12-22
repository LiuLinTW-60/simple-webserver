#ifndef SERVER_H
#define SERVER_H

typedef struct server {
    int fd;
    char *path;
    char *port;
} Server;


Server *serverNew(char*, char*);
void serverServe(Server *);

#endif