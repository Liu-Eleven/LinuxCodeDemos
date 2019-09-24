/**
 * select����ʾ����server��, select_server.cpp
 * zhangyl 2018.12.24
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include <errno.h>

//�Զ��������Чfd��ֵ
#define INVALID_FD -1

int main(int argc, char* argv[])
{
    //����һ������socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }

    //��ʼ����������ַ
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);
    if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
		close(listenfd);
        return -1;
    }

	//��������
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
		close(listenfd);
        return -1;
    }
	
	//�洢�ͻ���socket������
	std::vector<int> clientfds;
	int maxfd = listenfd;
	
	while (true) 
	{	
		fd_set readset;
		FD_ZERO(&readset);
		
		//������socket���뵽�����Ŀɶ��¼���ȥ
		FD_SET(listenfd, &readset);
		
		//���ͻ���fd���뵽�����Ŀɶ��¼���ȥ
		int clientfdslength = clientfds.size();
		for (int i = 0; i < clientfdslength; ++i)
		{
			if (clientfds[i] != INVALID_FD)
			{
				FD_SET(clientfds[i], &readset);
			}
		}
		
		timeval tm;
		tm.tv_sec = 1;
		tm.tv_usec = 0;
		//����ֻ���ɶ��¼���������д���쳣�¼�
		int ret = select(maxfd + 1, &readset, NULL, NULL, &tm);
		if (ret == -1)
		{
			//�����˳�����
			if (errno != EINTR)
				break;
		}
		else if (ret == 0)
		{
			//select ������ʱ���´μ���
			continue;
		} else {
			//��⵽ĳ��socket���¼�
			if (FD_ISSET(listenfd, &readset))
			{
				//����socket�Ŀɶ��¼�����������µ����ӵ���
				struct sockaddr_in clientaddr;
				socklen_t clientaddrlen = sizeof(clientaddr);
				//4. ���ܿͻ�������
				int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
				if (clientfd == -1)					
				{         	
					//�������ӳ����˳�����
					break;
				}
				
				//ֻ�������ӣ�������recv��ȡ�κ�����
				std:: cout << "accept a client connection, fd: " << clientfd << std::endl;
				clientfds.push_back(clientfd);
				//��¼һ�����µ����fdֵ���Ա���Ϊ��һ��ѭ����select�ĵ�һ������
				if (clientfd > maxfd)
					maxfd = clientfd;
			} 
			else 
			{
				//����Զ˷��������ݳ��Ȳ�����63���ַ�
				char recvbuf[64];
				int clientfdslength = clientfds.size();
				for (int i = 0; i < clientfdslength; ++i)
				{
					if (clientfds[i] != -1 && FD_ISSET(clientfds[i], &readset))
					{				
						memset(recvbuf, 0, sizeof(recvbuf));
						//������socket�����������
						int length = recv(clientfds[i], recvbuf, 64, 0);
						if (length <= 0 && errno != EINTR)
						{
							//��ȡ���ݳ�����
							std::cout << "recv data error, clientfd: " << clientfds[i] << std::endl;							
							close(clientfds[i]);
							//��ֱ��ɾ����Ԫ�أ�����λ�õ�Ԫ����λ-1
							clientfds[i] = INVALID_FD;
							continue;
						}
						
						std::cout << "clientfd: " << clientfds[i] << ", recv data: " << recvbuf << std::endl;					
					}
				}
				
			}
		}
	}
	
	//�ر����пͻ���socket
	int clientfdslength = clientfds.size();
    for (int i = 0; i < clientfdslength; ++i)
	{
		if (clientfds[i] != INVALID_FD)
		{
			close(clientfds[i]);
		}
	}
	
	//�ر�����socket
	close(listenfd);

    return 0;
}