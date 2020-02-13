#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_

#define BUFSIZE 8192

struct route_info_s {
	char iface_name[IF_NAMESIZE];
	char gate_way_str[64];
	struct in_addr gate_way;
};

/*
 * SIOCGIFNAME_TEST
 */
extern int SIOCGIFNAME_TEST();
/*
 * obtain all NICs(Network Interface Card) base on SIOCGIFCONF ioctl
 * eg: lo/eth0/wlan0
 */
extern int obtain_all_NICs();
/*
 *	obtain mac addr
 */
extern int obtain_mac_addr(char *mac_addr, char *nic_name);
/*
 *	obtain ip addr
 */
extern int obtain_ip_addr(char *ip_addr, char *nic_name);
/*
 *	obtain gateway addr
 */
extern int obtain_gateway_route(struct route_info_s *p_route_info);
extern int obtain_gateway_proc(struct in_addr *addr);
/*
 *	obtain nic state
 */
extern char *obtain_nic_state(char *nic_name);
/*
 * bind NIC
 */
extern int bind_NIC(int socket_fd, char *nic_name);
/*
 * set socket reuseaddr
 */
extern int set_socket_reuseaddr(int socket_fd, int optval);
/*
 * dump tcp info
 */
extern int dump_tcp_info(struct tcp_info *info);
/*
 * get tcp sock info
 */
extern int gettcpsockinfo(int sock, struct tcp_info *info);
/*
 * set tcp sock keepalive
 */
extern int set_tcp_keepalive(int sock, int keepalive, int keepidle, int keepinterval, int keepcount);
#endif