#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h> //struct icmp
#include <netinet/in.h> //sockaddr_in
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>
 
//校验和计算
unsigned short calc_cksum(char *buff, int len)
{
    int blen = len;
    unsigned short *mid = (unsigned short*)buff;
    unsigned short te = 0;
    unsigned int sum = 0;
 
    while(blen > 1) {
       sum += *mid++;
       blen -= 2; 
    }
    //数据长度为奇数比如65 上面的while是按16计算的 最后就会剩下一字节不能计算 	
    if(blen == 1) {  
       //将多出的一字节放入short类型的高位 低8位置0 加入到sum中
       te = *(unsigned char*)mid;
       te  = (te << 8) & 0xff;
       sum += te;                
    }
    sum = (sum >> 16) + (sum&0xffff); 
    sum += sum >>16;  
    return (unsigned short)(~sum);
}
 
static void icmp_packet(char *buff, int len, int id, int seq)
{
    struct timeval *tval = NULL;
    struct icmp *icmp = (struct icmp*)buff;
   
    icmp->icmp_type = 8; //ECHO REQUEST
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;  //first set zero
    icmp->icmp_id = id & 0xffff;
    icmp->icmp_seq = seq;
	
    tval = (struct timeval *)icmp->icmp_data;    
    gettimeofday(tval, NULL);//获得传输时间作为数据
 
    //计算校验和
    icmp->icmp_cksum = calc_cksum(buff, len);   
    return;    
}
 
int parse_packet(char *buff, int len)
{
    struct timeval *val;
    struct timeval nv;
    struct icmp *icmp;
    struct iphdr *iphead = (struct iphdr *)buff;
    struct in_addr addr; 
    addr.s_addr = iphead->saddr;
 
    printf("comefrom ip=%s  ",inet_ntoa(addr));
    //跳过ip头
    icmp = (struct icmp *)(buff+sizeof(struct iphdr));
    
    //看传输回的包校验和是否正确
    if(calc_cksum((char *)icmp,len-sizeof(sizeof(struct iphdr))) > 1) {
       printf("receiver error\n");
       return -1;
    } 
    gettimeofday(&nv,NULL);
    val = (struct timeval *)icmp->icmp_data;
 
    printf("type=%d seq=%d id=%d pid=%d usec=%d \n",icmp->icmp_type, icmp->icmp_seq, icmp->icmp_id, (getpid()&0xffff), nv.tv_usec - val->tv_usec);
	
	return 0;
}
 
int main(int argc,char *argv[])
{
    int skfd;
    struct sockaddr_in addr={0};    
    struct sockaddr_in saddr={0};
    char buff[64]={0};  
    char recvbuff[512]={0};  
    int ret;
    int addrlen = 0;
	int count = 5;
    int snd_cnt = 0; 
	int rcv_cnt = 0;
    int i = 1;
     
	//------------------------------------------------------------------
	
	struct hostent *host;
    //host = gethostbyname("www.baidu.com");
	host = gethostbyname("157.255.71.212"); 
    if (host == NULL){
        switch (h_errno){
            case HOST_NOT_FOUND:
                error("Host not found.");
            break;
            case NO_ADDRESS:
                error("The requested name is valid but does not have an IP address.");
            break;
            case NO_RECOVERY:
                error("A nonrecoverable name server error occurred.");
            break;
            case TRY_AGAIN:
                error("A temporary error occurred on an authoritative name server.  Try again later.");
            break;
            default:
                error("You can't see it!");
            break;
        }
    }
    char *byte_addr;
    byte_addr = host->h_addr_list[0];
	char ip_buf[64] = {0};
    sprintf(ip_buf, "%d.%d.%d.%d", byte_addr[0]&0xff, byte_addr[1]&0xff, byte_addr[2]&0xff, byte_addr[3]&0xff);
	printf("ip: %s\n", ip_buf);
	
	//------------------------------------------------------------------
	 
    skfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(skfd < 0)
    {
        printf("socket error\n");
        return -1;
    }
         
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_buf);
 
	//每一秒发送一次 共发送count次    
    while(count > 0) {
        //序列号seq 从1 开始传输  buff的大小为64
        memset(buff, 0, sizeof(buff));
        icmp_packet(buff, 64, getpid(), i);
        i++;
        count--;
 
        //将数据发送出去
        ret = sendto(skfd, buff, 64, 0, (struct sockaddr *)&addr, sizeof(addr)); 
        if(ret <= 0) {
            printf("send error\n"); 
            goto out;  
        }  
        else {
			snd_cnt++;
		}
 
        //接收echo replay
        memset(recvbuff, 0, sizeof(recvbuff));
        memset(&saddr, 0, sizeof(saddr));
        addrlen = sizeof(saddr);
        ret = recvfrom(skfd, recvbuff, sizeof(recvbuff), 0, (struct sockaddr *)&saddr, &addrlen);   
        if(ret <= 0) {
            printf("recv error\n");
            goto out;
        }
        if(parse_packet(recvbuff, ret) == 0) {
			rcv_cnt++;
		}
        sleep(1);
     }
	 
	 if(rcv_cnt == snd_cnt) {
		 printf("ping target success!\n");
	 }
	 else {
		 printf("ping target fail!\n");
	 }
out:
    close(skfd);
	
    return 0;
}

