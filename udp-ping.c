/*
 * $Id: udp-ping.c,v 1.1 2001/02/26 13:11:30 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void ping(int sock, struct sockaddr_in *addr);
void wait(int sock);

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in addr;
    int port;
    char *host;
    struct hostent *hp;

    int do_connect = 0;

    if (strcmp(argv[1], "-c") == 0) {
	do_connect = 1;
	argc--, argv++;
    }

    host = argv[1];
    port = atoi(argv[2]);

    if (!(hp = gethostbyname(host))) {
	fprintf(stderr, "%s: Unknown host\n", host);
	exit(1);
    }

    addr.sin_family      = AF_INET;
    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
    addr.sin_port        = htons(port);

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("socket");
	exit(1);
    }

    if (do_connect && connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	perror("connect");

    if (do_connect) {
	ping(sock, NULL);
	wait(sock);
	ping(sock, NULL);
	wait(sock);
    } else {
	ping(sock, &addr);
	wait(sock);
	ping(sock, &addr);
	wait(sock);
    }

    close(sock);

    return 0;
}

void ping(int sock, struct sockaddr_in *addr)
{
    static char msg[] = "abc\n";

    if (addr) {
	if (sendto(sock, msg, strlen(msg), 0,
		   (struct sockaddr *)addr, sizeof(*addr)) < 0)
	    perror("sendto");
    } else {
	if (send(sock, msg, strlen(msg), 0) < 0)
	    perror("send");
    }
}

void wait(int sock)
{
    char msg[8192];
    struct sockaddr_in peer;
    int length = sizeof(peer);

    if (recvfrom(sock, msg, sizeof(msg), 0,
		 (struct sockaddr *)&peer, &length) < 0) {
	perror("recv");
    } else {
	printf("dgram received from: %s:%d\n", inet_ntoa(peer.sin_addr),
	       ntohs(peer.sin_port));
    }
}
