/*
 * $Id: udp-ping.c,v 1.2 2003/03/07 08:20:55 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void ping(int sock, struct sockaddr_in *addr, int seqno);
void wait(int sock);

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in addr;
    int port;
    char *host;
    struct hostent *hp;
    int i;

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
	for (i = 0; i < 8; i++) {
	    ping(sock, NULL, i);
	    wait(sock);
	    sleep(1);
	}
    } else {
	for (i = 0; i < 8; i++) {
	    ping(sock, &addr, i);
	    wait(sock);
	    sleep(1);
	}
    }

    close(sock);

    return 0;
}

struct message {
    int seqno;
    struct timeval time;
};

void ping(int sock, struct sockaddr_in *addr, int seqno)
{
    struct message msg;

    msg.seqno = seqno;
    gettimeofday(&msg.time, NULL);

    if (addr) {
	if (sendto(sock, &msg, sizeof(msg), 0,
		   (struct sockaddr *)addr, sizeof(*addr)) < 0)
	    perror("sendto");
    } else {
	if (send(sock, &msg, sizeof(msg), 0) < 0)
	    perror("send");
    }
}

void wait(int sock)
{
    char buf[8192];
    struct sockaddr_in peer;
    int length = sizeof(peer);
    int nbytes;
    struct timeval now;
    int seqno, delay;

    if ((nbytes = recvfrom(sock, buf, sizeof(buf), 0,
			   (struct sockaddr *)&peer, &length)) < 0) {
	perror("recv");
    } else if (nbytes != sizeof(struct message)) {
	fprintf(stderr, "wrong-sized dgram received\n");
    } else {
	struct message *msg = (struct message *)buf;

	gettimeofday(&now, NULL);
	seqno = msg->seqno;
	delay = 1000000 * (now.tv_sec - msg->time.tv_sec)
	    + (now.tv_usec - msg->time.tv_usec);
	printf("dgram received from: %s:%d, seqno = %d, delay = %.3fms\n",
	       inet_ntoa(peer.sin_addr), ntohs(peer.sin_port),
	       seqno, delay / 1000.0);
    }
}
