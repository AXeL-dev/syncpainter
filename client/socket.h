#ifndef _SOCKET_H
#define _SOCKET_H

/* En-tete */
#if defined (WIN32) /* Si nous sommes sous Windows */
    #include <winsock2.h>
    typedef int socklen_t;
#elif defined (linux) /* Sinon, si nous sommes sous Linux */
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(s) close(s)
    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
#endif
/* Constantes */
#define PORT 23
#define SOCKET_TAILLE 32

#endif
