#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

char *get_family(struct addrinfo *p_addrinfo)
{
	char *p;

	switch (p_addrinfo->ai_family) {
	case AF_INET:
		p = "inet";
		break;
	case AF_INET6:
		p = "inet6";
		break;
	case AF_UNIX:
		p = "unix";
		break;
	case AF_UNSPEC:
		p = "unspecified";
		break;
	default:
		p = "unknown";
	}

	return p;
}

char *get_type(struct addrinfo *p_addrinfo)
{
	char *p;

	switch (p_addrinfo->ai_socktype) {
	case SOCK_STREAM:
		p = "stream";
		break;
	case SOCK_DGRAM:
		p = "datagram";
		break;
	case SOCK_SEQPACKET:
		p = "seqpacket";
		break;
	case SOCK_RAW:
		p = "raw";
		break;
	default:
		p = "unknown";
	}

	return p;
}

char *get_protocol(struct addrinfo *p_addrinfo)
{
	char *p;

	switch (p_addrinfo->ai_protocol) {
	case 0:
		p = "default";
		break;
	case IPPROTO_TCP:
		p = "TCP";
		break;
	case IPPROTO_UDP:
		p = "UDP";
		break;
	case IPPROTO_RAW:
		p = "raw";
		break;
	default:
		p = "unknown";
	}

	return p;
}

char *get_flags(struct addrinfo *p_addrinfo)
{
	char *p;

	if (p_addrinfo->ai_flags == 0) {
		p = "0";
	} else {
		if (p_addrinfo->ai_flags & AI_PASSIVE)
			p = "passive";
		if (p_addrinfo->ai_flags & AI_CANONNAME)
			p = "canon";
		if (p_addrinfo->ai_flags & AI_NUMERICHOST)
			p = "numhost";
#if defined(AI_NUMERICSERV)
		if (p_addrinfo->ai_flags & AI_NUMERICSERV)
			p = "numserv";
#endif
#if defined(AI_V4MAPPED)
		if (p_addrinfo->ai_flags & AI_V4MAPPED)
			p = "v4mapped";
#endif
#if defined(AI_ALL)
		if (p_addrinfo->ai_flags & AI_ALL)
			p = "all";
#endif
	}

	return p;
}

#define INETx_ADDRSTRLEN INET_ADDRSTRLEN

/*
 * this function is not thread safe
 * copy from shairport
 */
static const char* format_address(struct sockaddr *fsa)
{
	static char string[INETx_ADDRSTRLEN];
	void *addr;
#ifdef AF_INET6
	if (fsa->sa_family == AF_INET6) {
		struct sockaddr_in6 *sa6 = (struct sockaddr_in6*)(fsa);
		addr = &(sa6->sin6_addr);
	} else
#endif
	{
		struct sockaddr_in *sa = (struct sockaddr_in*)(fsa);
		addr = &(sa->sin_addr);
	}
	return inet_ntop(fsa->sa_family, addr, string, sizeof(string));
}

void dump_addrinfo(struct addrinfo *p_addrinfo)
{
	printf("%-16s%-16s%-16s%-16s%-16s\n", "family", "flags", "type", "protocol", "addr");
	printf("%-16s%-16s%-16s%-16s%-16s\n", get_family(p_addrinfo), get_flags(p_addrinfo),
	       get_type(p_addrinfo), get_protocol(p_addrinfo), format_address(p_addrinfo->ai_addr));

	printf("\n");
}

int GETADDRINFO_TEST()
{
	int ret;

	struct addrinfo hints;
	struct addrinfo *p_addrinfo;
	struct addrinfo *result_addr_list;

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = 0;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if ((ret = getaddrinfo("www.baidu.com", NULL, &hints, &result_addr_list)) != 0) {
		printf("getaddrinfo error: %s", gai_strerror(ret));
	}

	for (p_addrinfo = result_addr_list; p_addrinfo != NULL; p_addrinfo = p_addrinfo->ai_next) {
		dump_addrinfo(p_addrinfo);
	}

	/* should free addrinfo list */
	freeaddrinfo(result_addr_list);

	return 0;
}
