/*
 * $Id: gethost.c,v 1.1 1996/09/19 07:27:33 urs Exp $
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

void gethost();

int wide = 0;

int main(argc, argv)
    char **argv;
{
    while (++argv, --argc)
	gethost(*argv);

    return 0;
}

void gethost(host)
    char *host;
{
    struct hostent *hp;
    struct in_addr addr;

    printf("%s: ", host);

    if (inet_aton(host, &addr)
	&& (hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET))) {
	puts(hp->h_name);
    } else if (hp = gethostbyname(host)) {
	struct in_addr addr;
	if (hp->h_addrtype != AF_INET) {
	    fputs("Unknown address type\n", stderr);
	    return;
	}
	memcpy(&addr, hp->h_addr, hp->h_length);
	puts(inet_ntoa(addr));
    } else {
	puts("unknown host");
    }
}
