#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/un.h>

#include "net_utils.h"
#include "getaddrinfo_utils.h"

int main(int argc, char *argv[])
{
	int ret;
	int socket_fd;
	char ip_addr[24];
	char mac_addr[24];
	struct sockaddr_in sin;
	char nic_name[] = "eth0";

	SIOCGIFNAME_TEST();
	obtain_all_NICs();
	obtain_mac_addr(mac_addr, nic_name);
	printf("NIC[%s] mac_addr(%s)\n", nic_name, mac_addr);
	obtain_ip_addr(ip_addr, nic_name);
	printf("NIC[%s] ip_addr(%s)\n", nic_name, ip_addr);

	struct route_info_s route_info;
	obtain_gateway_route(&route_info);
	printf("iface_name = %s\n", route_info.iface_name);
	printf("gateway_route = %s\n", route_info.gate_way_str);

	struct in_addr addr;
	obtain_gateway_proc(&addr);
	char gateway[128];
	inet_ntop(AF_INET, &addr, gateway, sizeof(gateway));
	printf("gateway_proc = %s\n", gateway);

	char *nic_state;
	nic_state = obtain_nic_state(nic_name);
	printf("NIC[%s]:%s\n", nic_name, nic_state);

	GETADDRINFO_TEST();

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket\n");
		return -1;
	}

	bind_NIC(socket_fd, argv[1]);
	fcntl(socket_fd, F_SETFD, fcntl(socket_fd, F_GETFD, 0) | FD_CLOEXEC);
	set_socket_reuseaddr(socket_fd, 1);

	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(1235);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(socket_fd, (const struct sockaddr*)(&sin), sizeof(struct sockaddr_in));
	if (ret < 0) {
		perror("bind\n");
	}

	struct tcp_info info;
	gettcpsockinfo(socket_fd, &info);
	dump_tcp_info(&info);

	ret = listen(socket_fd, 5);
	if (ret < 0) {
		perror("listen\n");
	}

	while (1) {
		int max_fd = -1;
		fd_set rfds;
		struct timeval tv;
		FD_ZERO(&rfds);

		FD_SET(socket_fd, &rfds);

		max_fd = socket_fd + 1;
		tv.tv_sec = 0;
		tv.tv_usec = 500 * 1000;

		ret = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		if (ret <= 0) {
			continue;
		}

		if (FD_ISSET(socket_fd, &rfds)) {
			struct sockaddr_in cli_sin;
			socklen_t slen = sizeof(struct sockaddr_in);

			int client = accept(socket_fd, (struct sockaddr*)&cli_sin, &slen);
			if (client < 0) {
				printf("accept error\n");
				continue;
			}

			set_tcp_keepalive(socket_fd, 1, 2, 5, 3);
			printf("new client: %s\n", inet_ntoa(cli_sin.sin_addr));
			close(client);
		}
	}

	return 0;
}
