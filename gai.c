/*
 * $Id: gai.c,v 1.1 2002/02/27 15:22:22 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    struct addrinfo *addr;
    int errcode;
    struct sockaddr_in  *addr_in;
    struct sockaddr_in6 *addr_in6;

    char *hostname = NULL;
    char *servname = NULL;

    if (argc != 2 && argc != 3) {
	fprintf(stderr, "Usage: %s host [service]\n", argv[0]);
	exit(1);
    }

    if (argc > 1 && strcmp(argv[1], "-") != 0)
	hostname = argv[1];
    if (argc > 2 && strcmp(argv[2], "-") != 0)
	servname = argv[2];

    if ((errcode = getaddrinfo(hostname, servname, NULL, &addr)) != 0) {
	fprintf(stderr, "%s\n", gai_strerror(errcode));
	exit(1);
    }

    printf("flags famly type proto  len\n");
    for (; addr; addr = addr->ai_next) {
	printf("%05x %5d %4d %5d %4d  ",
	       addr->ai_flags,
	       addr->ai_family, addr->ai_socktype, addr->ai_protocol,
	       addr->ai_addrlen);
	switch (addr->ai_family) {
	    char buf[INET6_ADDRSTRLEN];
	case AF_INET:
	    addr_in = (struct sockaddr_in *)(addr->ai_addr);
	    printf("%s %d\n",
		   inet_ntop(AF_INET, &addr_in->sin_addr, buf, sizeof(buf)),
		   ntohs(addr_in->sin_port));
	    break;
	case AF_INET6:
	    addr_in6 = (struct sockaddr_in6 *)(addr->ai_addr);
	    printf("%s %d\n",
		   inet_ntop(AF_INET6, &addr_in6->sin6_addr, buf, sizeof(buf)),
		   ntohs(addr_in6->sin6_port));
	    break;
	}
    }

    freeaddrinfo(addr);

    return 0;
}
