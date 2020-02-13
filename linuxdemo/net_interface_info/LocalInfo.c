#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>

int getLocalInfo(char *interfaceName, char *ip, char* subnetMask, char* mac, char* broadAddr)
{
    int fd;
    int interfaceNum = 0;
    struct ifreq buf[16];
    struct ifconf ifc;
    struct ifreq ifrcopy;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        close(fd);
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;
    if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc)) {
        interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
        //printf("interface num = %d\n", interfaceNum);
        while (interfaceNum-- > 0) {
            //printf("\ndevice name: %s\n", buf[interfaceNum].ifr_name);
			if(strcmp(buf[interfaceNum].ifr_name, interfaceName) == 0) {
				//ignore the interface that not up or not runing  
				ifrcopy = buf[interfaceNum];
				if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy)) {
					//printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
					close(fd);
					return -1;
				}

				//get the mac of this interface  
				if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum]))) {
					sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],

						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
					//printf("device mac: %s\n", mac);
				}
				else {
					//printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
					close(fd);
					return -1;
				}

				//get the IP of this interface  
				if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum])) {
					sprintf(ip, "%s", (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));
					//printf("device ip: %s\n", ip);
				}
				else {
					//printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
					close(fd);
					return -1;
				}

				//get the broad address of this interface  
				if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interfaceNum])) {
					sprintf(broadAddr, "%s", (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_broadaddr))->sin_addr));
					//printf("device broadAddr: %s\n", broadAddr);
				}
				else {
					//printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
					close(fd);
					return -1;
				}

				//get the subnet mask of this interface  
				if (!ioctl(fd, SIOCGIFNETMASK, &buf[interfaceNum])) {
					sprintf(subnetMask, "%s", (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_netmask))->sin_addr));
					//printf("device subnetMask: %s\n", subnetMask);
				}
				else {
					//printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
					close(fd);
					return -1;

				}
			}
        }
    }
    else {
        //printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

// 获取网关IP
int getLocalGateWay(char* gateway)  
{  
    FILE *fp;  
    char buf[1024] = {0};  
    char cmd[128]; 
	char *ip;	
    char *tmp;  
  
    strcpy(cmd, "ip route | grep default");  
    fp = popen(cmd, "r");  
	
    if(NULL == fp) {  
        perror("popen error");  
        return -1;  
    } 
	
    while(fgets(buf, sizeof(buf), fp) != NULL) {  
		tmp = strstr(buf, "default"); 
		if(tmp != NULL) {
			break; 
		}   
    }  
	
	// 删除前面不是IP部分的字符
	while(*tmp) {
		if(isdigit(*tmp) || *tmp == '.') {
			ip = tmp;
			break;
		}
		++tmp;
	}
		
	// 删除后面不是IP部分的字符
	while(*tmp) {
		if(isdigit(*tmp) || *tmp == '.') {
			++tmp;
			continue;
		}
		*tmp = '\0';
		++tmp;
	}

	strncpy(gateway, ip, strlen(ip));	
    pclose(fp);  
      
    return 0;  
}

// 获取网关DNS
// @param dns1 保存第1个dns配置
// @param dns2 保存第2个dns配置
// @return 0-成功, 非0-失败
int get_IPV4_DNS(char *dns)
{
	int fd = -1;
	int size = 0;
	char strBuf[1024] = {0};
	char *pStr = NULL;
	char *pFound = NULL;
	char *pTmp = NULL;
	char *pFoundIP = NULL;
	int pos = 0;
	int count = 0;
	
	fd = open("/etc/resolv.conf", O_RDONLY);
	if(-1 == fd) {
		return -1;
	}

	size = read(fd, strBuf, sizeof(strBuf));
	if(size < 0) {
		close(fd);
		return -1;
	}
	strBuf[size] = '\0';
	close(fd);

	pStr = strBuf;
	while((pFound = strstr(pStr, "nameserver")) && pFound != NULL) {
		count = 0;	
		pTmp = pFound;
		pStr = pFound;
		while(*pTmp != '\n') {
			pTmp++;
			count++;
		}
		pStr += count + 1;
	
		// 删除前面不是IP部分的字符
		while(*pFound) {
			if(isdigit(*pFound) || *pFound == '.') {
				pFoundIP = pFound;
				break;
			}
			++pFound;
		}
			
		// 删除后面不是IP部分的字符
		while(*pFound) {
			if(isdigit(*pFound) || *pFound == '.') {
				++pFound;
				continue;
			}
			*pFound = '\0';
			++pFound;
		}

		memcpy(dns, pFoundIP, count);
		dns[count] = '\0';
	}

	return 0;
}

int main(void)
{
	char mac[32] = {0};
    char ip[32] = {0};
    char broadAddr[32] = {0};
    char subnetMask[32] = {0};
    getLocalInfo("eth0", ip, subnetMask, mac, broadAddr);
	printf("ip: %s, subnetMask: %s, mac: %s, broadAddr: %s\n", ip, subnetMask, mac, broadAddr);
	
	char gateWayBuf[512] = {0};
	getLocalGateWay(gateWayBuf);
	printf("gateway: %s\n", gateWayBuf);
	
	char dnsBuf[256] = {0};
	get_IPV4_DNS(dnsBuf);
	printf("dns: %s\n", dnsBuf);
	printf("\n");
	
    return 0;
}
