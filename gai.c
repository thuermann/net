/*
 * $Id: gai.c,v 1.5 2011/01/31 22:47:00 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

static int usage(const char *name)
{
    fprintf(stderr,
	    "Usage: %s [-46][-t type][-p proto][-f flags] host [service]\n",
	    name);
}

int main(int argc, char **argv)
{
    int opt;
    struct addrinfo *addr, hints;
    int errcode;
    char *p;
    int neg;

    char *hostname = NULL;
    char *servname = NULL;
    int flags      = AI_PASSIVE;
    int family     = AF_UNSPEC;
    int type       = 0;
    int protocol   = 0;
    int errflg     = 0;

    while ((opt = getopt(argc, argv, "46t:p:f:")) != -1) {
	switch (opt) {
	case '4':
	    family = AF_INET;
	    break;
	case '6':
	    family = AF_INET6;
	    break;
	case 'f':
	    for (p = strtok(optarg, ","); p; p = strtok(NULL, ",")) {
		if (p[0] == '!' || p[0] == '-')
		    neg = 1, p++;
		else
		    neg = 0;
		if (strcmp(p, "passive") == 0)
		    neg ? (flags &= ~AI_PASSIVE) : (flags |= AI_PASSIVE);
	    }
	    break;
	case 't':
	    if (strcmp(optarg, "stream") == 0)
		type = SOCK_STREAM;
	    else if (strcmp(optarg, "dgram") == 0)
		type = SOCK_DGRAM;
	    else
		fprintf(stderr, "Unknown socket type\n");
	    break;
	case 'p':
	    if (strcmp(optarg, "tcp") == 0)
		protocol = IPPROTO_TCP;
	    else if (strcmp(optarg, "udp") == 0)
		protocol = IPPROTO_UDP;
	    else
		fprintf(stderr, "Unknown protocol\n");
	    break;
	default:
	    errflg = 1;
	    break;
	}
    }

    if (errflg || argc - optind != 1 && argc - optind != 2) {
	usage(argv[0]);
	exit(1);
    }

    if (strcmp(argv[optind], "-") != 0)
	hostname = argv[optind];
    optind++;
    if (argc - optind > 0 && strcmp(argv[optind], "-") != 0)
	servname = argv[optind];

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags    = flags;
    hints.ai_family   = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    if ((errcode = getaddrinfo(hostname, servname, &hints, &addr)) != 0) {
	fprintf(stderr, "%s\n", gai_strerror(errcode));
	exit(1);
    }

    printf("flags famly type proto  len\n");
    for (; addr; addr = addr->ai_next) {
	char host[NI_MAXHOST], serv[NI_MAXSERV];

	getnameinfo(addr->ai_addr, addr->ai_addrlen,
		    host, sizeof(host), serv, sizeof(serv),
		    NI_NUMERICHOST | NI_NUMERICSERV);

	printf("%05x %5d %4d %5d %4d  %s %s\n",
	       addr->ai_flags,
	       addr->ai_family, addr->ai_socktype, addr->ai_protocol,
	       addr->ai_addrlen, host, serv);
    }

    freeaddrinfo(addr);

    return 0;
}
