/**
 * ��ʾ poll �������÷���poll_server.cpp
 * zhangyl 2019.03.16
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <errno.h>

//��Чfd���
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
	
	//������socket����Ϊ��������
	int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
	int newSocketFlag = oldSocketFlag | O_NONBLOCK;
	if (fcntl(listenfd, F_SETFL,  newSocketFlag) == -1)
	{
		close(listenfd);
		std::cout << "set listenfd to nonblock error." << std::endl;
		return -1;
	}
	
	//���õ�ַ�Ͷ˿ں�
	int on = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

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
	
	std::vector<pollfd> fds;
	pollfd listen_fd_info;
	listen_fd_info.fd = listenfd;
	listen_fd_info.events = POLLIN;
	listen_fd_info.revents = 0;
	fds.push_back(listen_fd_info);
	
	//�Ƿ������Ч��fd��־
	bool exist_invalid_fd;
	int n;
	while (true)
	{
		exist_invalid_fd = false;
		n = poll(&fds[0], fds.size(), 1000);
		if (n < 0)
		{
			//���ź��ж�
			if (errno == EINTR)
				continue;
			
			//�����˳�
			break;
		}
		else if (n == 0)
		{
			//��ʱ������
			continue;
		}
		
		for (size_t i = 0; i < fds.size(); ++i)
		{
			// �¼��ɶ�
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == listenfd)
				{
					//����socket������������
					struct sockaddr_in clientaddr;
					socklen_t clientaddrlen = sizeof(clientaddr);
					//���ܿͻ�������, �����뵽fds������
					int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
					if (clientfd != -1)
					{
						//���ͻ���socket����Ϊ��������
						int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
						int newSocketFlag = oldSocketFlag | O_NONBLOCK;
						if (fcntl(clientfd, F_SETFL,  newSocketFlag) == -1)
						{
							close(clientfd);
							std::cout << "set clientfd to nonblock error." << std::endl;						
						} 
						else
						{
							struct pollfd client_fd_info;
							client_fd_info.fd = clientfd;
							client_fd_info.events = POLLIN;
							client_fd_info.revents = 0;
							fds.push_back(client_fd_info);
							std::cout << "new client accepted, clientfd: " << clientfd << std::endl;
						}				
					}
				}
				else 
				{
					//��ͨclientfd,��ȡ����
					char buf[64] = { 0 };
					int m = recv(fds[i].fd, buf, 64, 0);
					if (m <= 0)
					{
						if (errno != EINTR && errno != EWOULDBLOCK)
						{
							//�����Զ˹ر������ӣ��رն�Ӧ��clientfd����������Ч��־λ
							for (std::vector<pollfd>::iterator iter = fds.begin(); iter != fds.end(); ++ iter)
							{
								if (iter->fd == fds[i].fd)
								{
									std::cout << "client disconnected, clientfd: " << fds[i].fd << std::endl;
									close(fds[i].fd);
									iter->fd = INVALID_FD;
									exist_invalid_fd = true;
									break;
								}
							}
						}			
					}
					else
					{
						std::cout << "recv from client: " << buf << ", clientfd: " << fds[i].fd << std::endl;
					}
				}
			}
			else if (fds[i].revents & POLLERR)
			{
				//TODO: ���Ҳ�����
			}
			
		}// end  outer-for-loop
		
		if (exist_invalid_fd)
		{
			//ͳһ������Ч��fd
			for (std::vector<pollfd>::iterator iter = fds.begin(); iter != fds.end(); )
			{
				if (iter->fd == INVALID_FD)
					iter = fds.erase(iter);
				else
					++iter;
			}
		}	
	}// end  while-loop
	  
	  
	//�ر�����socket
	for (std::vector<pollfd>::iterator iter = fds.begin(); iter != fds.end(); ++ iter)
		close(iter->fd);			

    return 0;
}