/*
 * $Id: udp-echo.c,v 1.1 2001/02/26 13:11:30 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    int sock, length;
    struct sockaddr_in addr;
    int ip_addr, port;

    ip_addr = INADDR_ANY;
    port    = atoi(argv[1]);

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("socket");
	exit(1);
    }

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(ip_addr);
    addr.sin_port        = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	perror("bind");
	exit(1);
    }

    length = sizeof(addr);
    if (getsockname(sock, (struct sockaddr *)&addr, &length) < 0) {
	perror("getsockname");
	exit(1);
    }

    for (;;) {
	char buf[8192];
	int nbytes, length;
	struct sockaddr_in peer;

	length = sizeof(peer);
	if ((nbytes = recvfrom(sock, buf, sizeof(buf), 0,
			       (struct sockaddr *)&peer, &length)) < 0) {
	    perror("recvfrom");
	}
	if (sendto(sock, buf, nbytes, 0,
		   (struct sockaddr *)&peer, length) < 0) {
	    perror("sendto");
	}
    }
    close(sock);

    return 0;
}
