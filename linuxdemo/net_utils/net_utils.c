#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>

#include "net_utils.h"

/********************************* Stateless APIs *********************************/

/*
 * SIOCGIFNAME_TEST
 */
int SIOCGIFNAME_TEST()
{
	int i;
	int ret;
	int sock;
	struct ifreq ifreq_r;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket\n");
		return -1;
	}

	for (i = 1; i < 10; i++) {
		ifreq_r.ifr_ifindex = i;
		ret = ioctl(sock, SIOCGIFNAME, &ifreq_r);
		if (ret < 0) {
			printf("(%d)NIC\n", (i - 1));
			break;
		}
		printf("NIC[%s]\n", ifreq_r.ifr_name);
	}

	close(sock);
	return 0;
}

/*
 * obtain all NICs(Network Interface Card) base on SIOCGIFCONF ioctl
 * eg: lo/eth0/wlan0
 */
int obtain_all_NICs()
{
	int i;
	int sock;
	int interfaces_num;
	struct ifreq ifr[10];
	struct ifconf ifc;

	ifc.ifc_len = sizeof(ifr);
	ifc.ifc_req = ifr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket\n");
		return -1;
	}

	if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
		perror("ioctl SIOCGIFCONF\n");
		return -1;
	}

	interfaces_num = ifc.ifc_len / sizeof(struct ifreq);
	printf("(%d)NIC:\n", interfaces_num);

	for (i = 0; i < interfaces_num; ++i) {
		struct ifreq *p_req = &ifr[i];
		printf("NIC[%s]\n", p_req->ifr_name);
	}

	close(sock);
	return 0;
}

/*
 *	obtain mac addr
 */
int obtain_mac_addr(char *mac_addr, char *nic_name)
{
	int sock;
	struct ifreq ifreq;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	strcpy(ifreq.ifr_name, nic_name);
	ioctl(sock, SIOCGIFHWADDR, &ifreq);
	sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
	        (unsigned char)ifreq.ifr_hwaddr.sa_data[0],
	        (unsigned char)ifreq.ifr_hwaddr.sa_data[1],
	        (unsigned char)ifreq.ifr_hwaddr.sa_data[2],
	        (unsigned char)ifreq.ifr_hwaddr.sa_data[3],
	        (unsigned char)ifreq.ifr_hwaddr.sa_data[4],
	        (unsigned char)ifreq.ifr_hwaddr.sa_data[5]);

	close(sock);
	return 0;
}

/*
 *	obtain ip addr
 */
int obtain_ip_addr(char *mac_addr, char *nic_name)
{
	int sock;
	struct ifreq ifreq;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	strcpy(ifreq.ifr_name, nic_name);
	ioctl(sock, SIOCGIFADDR, &ifreq);
	sprintf(mac_addr, "%s", inet_ntoa(((struct sockaddr_in*) & (ifreq.ifr_addr))->sin_addr));

	close(sock);
	return 0;
}

/*
 *	obtain gateway addr
 */
/* analysis route info from kernel */
static void parse_route_info(struct nlmsghdr *p_nlmsghdr, struct route_info_s *p_route_info)
{
	int rtmsg_len;
	char iface_name[32];
	struct rtmsg *p_rtmsg;
	struct rtattr *p_rtattr;
	struct in_addr tmp_in_addr;

	p_rtmsg = (struct rtmsg *)NLMSG_DATA(p_nlmsghdr);
	/*
	 * If the route is not for AF_INET or does not belong to main routing table
	 * then return
	 */
	if ((p_rtmsg->rtm_family != AF_INET) || (p_rtmsg->rtm_table != RT_TABLE_MAIN)) {
		return;
	}

	p_rtattr = (struct rtattr *)RTM_RTA(p_rtmsg);
	rtmsg_len = RTM_PAYLOAD(p_nlmsghdr);

	for (; RTA_OK(p_rtattr, rtmsg_len); p_rtattr = RTA_NEXT(p_rtattr, rtmsg_len)) {
		switch (p_rtattr->rta_type) {
		case RTA_OIF:
			if_indextoname(*(int *)RTA_DATA(p_rtattr), iface_name);
			strcpy(p_route_info->iface_name, iface_name);
			break;
		case RTA_GATEWAY:
			tmp_in_addr.s_addr = *(u_int *)RTA_DATA(p_rtattr);
			strcpy(p_route_info->gate_way_str, inet_ntoa(tmp_in_addr));
			p_route_info->gate_way = tmp_in_addr;
			break;
		case RTA_PREFSRC:
			tmp_in_addr.s_addr = *(u_int *)RTA_DATA(p_rtattr);
			//printf("src : %s\n", inet_ntoa(tmp_in_addr));
			break;
		case RTA_DST:
			tmp_in_addr.s_addr = *(u_int *)RTA_DATA(p_rtattr);
			//printf("dst : %s\n", inet_ntoa(tmp_in_addr));
			break;
		}
	}

	return;
}

static int read_netlink_msg(int sock, char *buf, int seq_num, int pid)
{
	int r_len = 0;
	int valid_len = 0;
	struct nlmsghdr *p_nlmsghdr;

	do {
		if ((r_len = recv(sock, buf, BUFSIZE - valid_len, 0)) < 0) {
			perror("read netlink socket : ");
			return -1;
		}

		p_nlmsghdr = (struct nlmsghdr *)buf;

		if ((NLMSG_OK(p_nlmsghdr, r_len) == 0) || (p_nlmsghdr->nlmsg_type == NLMSG_ERROR)) {
			perror("Error in recieved packet");
			return -1;
		}

		if (p_nlmsghdr->nlmsg_type == NLMSG_DONE) {
			break;
		} else {
			buf += r_len;
			valid_len += r_len;
		}

		if ((p_nlmsghdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			break;
		}
	} while ((p_nlmsghdr->nlmsg_seq != seq_num) || (p_nlmsghdr->nlmsg_pid != pid));

	return valid_len;
}

int obtain_gateway_route(struct route_info_s *p_route_info)
{
	int len;
	int sock;
	int msg_seq = 0;
	char msg_buf[BUFSIZE];
	struct nlmsghdr *p_nlmsg;

	if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
		perror("socket create\n");
		return -1;
	}

	memset(msg_buf, 0, BUFSIZE);
	p_nlmsg = (struct nlmsghdr *)msg_buf;
	/* Length of message */
	p_nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	/* Get the routes from kernel routing table */
	p_nlmsg->nlmsg_type = RTM_GETROUTE;
	/* The message is a request for dump */
	p_nlmsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
	/* Sequence of the message packet */
	p_nlmsg->nlmsg_seq = msg_seq++;
	/* PID of process sending the request */
	p_nlmsg->nlmsg_pid = getpid();

	if (send(sock, p_nlmsg, p_nlmsg->nlmsg_len, 0) < 0) {
		printf("sent to netlink error\n");
		return -1;
	}

	memset(msg_buf, 0, BUFSIZE);
	if ((len = read_netlink_msg(sock, msg_buf, msg_seq, getpid())) < 0) {
		printf("read from netlink error\n");
		return -1;
	}

	p_nlmsg = (struct nlmsghdr *)msg_buf;
	for (; NLMSG_OK(p_nlmsg, len); p_nlmsg = NLMSG_NEXT(p_nlmsg, len)) {
		parse_route_info(p_nlmsg, p_route_info);
	}

	close(sock);
	return 0;
}

/* copy from tencent/mars which base on libnatpmp */
int obtain_gateway_proc(struct in_addr *addr)
{
	long d, g;
	char buf[256];
	int line = 0;
	FILE * f;
	char * p;

	f = fopen("/proc/net/route", "r");
	if (!f) {
		return -1;
	}

	while (fgets(buf, sizeof(buf), f)) {
		if (line > 0) {
			p = buf;
			while (*p && !isspace(*p)) {
				p++;
			}

			while (*p && isspace(*p)) {
				p++;
			}

			if (sscanf(p, "%lx%lx", &d, &g) == 2) {
				if (d == 0) { /* default */
					addr->s_addr = (in_addr_t)g;
					fclose(f);
					return 0;
				}
			}
		}
		line++;
	}
	/* default route not found ! */
	if (f) {
		fclose(f);
	}

	return -1;
}

/*
 *	obtain nic state
 */
char *obtain_nic_state(char *nic_name)
{
	int sock = 0;
	struct ifreq ifr;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("open socket error:\n");
		return NULL;
	}

	strcpy(ifr.ifr_name, nic_name);

	if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl SIOCGIFFLAGS error:\n");
		printf("failed to get NIC(%s) state\n", nic_name);
		close(sock);
		return NULL;
	}

	close(sock);

	if (ifr.ifr_flags & IFF_RUNNING) {
		return "UP";
	} else {
		return "DOWN";
	}
}

/********************************* State APIs *********************************/

/*
 * bind NIC
 */
int bind_NIC(int sock, char *nic_name)
{
	struct ifreq req;

	strncpy(req.ifr_name, nic_name, strlen(nic_name) + 1);

	if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &req, sizeof(struct ifreq)) < 0) {
		perror("failed to bind to NIC\n");
		return -1;
	}

	printf("bind NIC(%s)\n", nic_name);

	return 0;
}

/*
 * set socket reuseaddr
 */
int set_socket_reuseaddr(int sock, int optval)
{
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

/*
 * dump tcp info
 */
int dump_tcp_info(struct tcp_info *info)
{
	printf("tcpi_state           =  %02x\n" ,  (uint32_t)info->tcpi_state);
	printf("tcpi_ca_state        =  %02x\n" ,  (uint32_t)info->tcpi_ca_state);
	printf("tcpi_retransmits     =  %02x\n" ,  (uint32_t)info->tcpi_retransmits);
	printf("tcpi_probes          =  %02x\n" ,  (uint32_t)info->tcpi_probes);
	printf("tcpi_backoff         =  %02x\n" ,  (uint32_t)info->tcpi_backoff);
	printf("tcpi_options         =  %02x\n" ,  (uint32_t)info->tcpi_options);
	printf("tcpi_rto             =  %02x\n" ,  info->tcpi_rto);
	printf("tcpi_snd_mss         =  %02x\n" ,  info->tcpi_snd_mss);
	printf("tcpi_rcv_mss         =  %02x\n" ,  info->tcpi_rcv_mss);
	printf("tcpi_unacked         =  %02x\n" ,  info->tcpi_unacked);
	printf("tcpi_sacked          =  %02x\n" ,  info->tcpi_sacked);
	printf("tcpi_lost            =  %02x\n" ,  info->tcpi_lost);
	printf("tcpi_retrans         =  %02x\n" ,  info->tcpi_retrans);
	printf("tcpi_fackets         =  %02x\n" ,  info->tcpi_fackets);
	printf("tcpi_last_data_sent  =  %02x\n" ,  info->tcpi_last_data_sent);
	printf("tcpi_last_ack_sent   =  %02x\n" ,  info->tcpi_last_ack_sent);
	printf("tcpi_last_data_recv  =  %02x\n" ,  info->tcpi_last_data_recv);
	printf("tcpi_last_ack_recv   =  %02x\n" ,  info->tcpi_last_ack_recv);
	printf("tcpi_pmtu            =  %02x\n" ,  info->tcpi_pmtu);
	printf("tcpi_rcv_ssthresh    =  %02x\n" ,  info->tcpi_rcv_ssthresh);
	printf("tcpi_rtt             =  %02x\n" ,  info->tcpi_rtt);
	printf("tcpi_rttvar          =  %02x\n" ,  info->tcpi_rttvar);
	printf("tcpi_snd_ssthresh    =  %02x\n" ,  info->tcpi_snd_ssthresh);
	printf("tcpi_snd_cwnd        =  %02x\n" ,  info->tcpi_snd_cwnd);
	printf("tcpi_advmss          =  %02x\n" ,  info->tcpi_advmss);
	printf("tcpi_reordering      =  %02x\n" ,  info->tcpi_reordering);
	printf("tcpi_rcv_rtt         =  %02x\n" ,  info->tcpi_rcv_rtt);
	printf("tcpi_rcv_space       =  %02x\n" ,  info->tcpi_rcv_space);
	printf("tcpi_total_retrans   =  %02x\n" ,  info->tcpi_total_retrans);

	return 0;
}

/*
 * get tcp sock info
 */
int gettcpsockinfo(int sock, struct tcp_info *info)
{
	int length = sizeof(struct tcp_info);
	return getsockopt( sock, IPPROTO_TCP, TCP_INFO, (void *)info, (socklen_t *)&length);
}

/*
 * set tcp sock keepalive
 */
int set_tcp_keepalive(int sock, int keepalive, int keepidle, int keepinterval, int keepcount)
{
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive ));
	setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle ));
	setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval ));
	setsockopt(sock, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount ));

	return 0;
}