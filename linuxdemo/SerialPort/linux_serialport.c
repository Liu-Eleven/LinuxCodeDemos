#include <termios.h> /* tcgetattr, tcsetattr */
#include <stdio.h>   /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>  /* read, write, close */
#include <fcntl.h>   /* open */
#include <string.h>  /* bzero */


#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

/* 
 * Decription for TIMEOUT_SEC(buflen,baud);
 * baud bits per second, buflen bytes to send.
 * buflen*20 (20 means sending an octect-bit data by use of the maxim bits 20)
 * eg. 9600bps baudrate, buflen=1024B, then TIMEOUT_SEC = 1024*20/9600+1 = 3 
 * don't change the two lines below unless you do know what you are doing.
 */
#define TIMEOUT_SEC(buflen, baud) (buflen * 20 / baud + 2)
#define TIMEOUT_USEC 0

static struct termios termios_old, termios_new;

int SetPortAttr(int fd, int baudrate, int databit, const char *stopbit, char parity);

int OpenComPort(int ComPort, int baudrate, int databit, const char *stopbit, char parity)
{
        char *pComPort;
        int fd; //File descriptor for the port
        int retval;

        switch (ComPort)
        {
        case 0:
                pComPort = "/dev/ttyS0";
                break;
        case 1:
                pComPort = "/dev/ttyS1";
                break;
        case 2:
                pComPort = "/dev/ttyS2";
                break;
        case 3:
                pComPort = "/dev/ttyS3";
                break;
        case 4:
                pComPort = "/dev/ttyS4";
                break;
        case 5:
                pComPort = "/dev/ttyS5";
                break;
        case 6:
                pComPort = "/dev/ttyS6";
                break;
        case 7:
                pComPort = "/dev/ttyS7";
                break;
        default:
                pComPort = "/dev/ttyS0";
                break;
        }

        fd = open(pComPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (-1 == fd)
                return (-1);

        tcgetattr(fd, &termios_old); /* save old termios value */
        /* 0 on success, -1 on failure */
        retval = SetPortAttr(fd, baudrate, databit, stopbit, parity);
        if (-1 == retval)
                return -1;

        return (fd);
}

void CloseComPort(int fd)
{
        /* flush output data before close and restore old attribute */
        tcsetattr(fd, TCSADRAIN, &termios_old);
        close(fd);
}

int _ReadComPort(int fd, void *data, int datalength)
{
        fd_set fs_read;
        struct timeval tv_timeout;
        int retval = 0;

        FD_ZERO(&fs_read);
        FD_SET(fd, &fs_read);
        tv_timeout.tv_sec = TIMEOUT_SEC(datalength, GetBaudrate());
        tv_timeout.tv_usec = TIMEOUT_USEC;

        retval = select(fd + 1, &fs_read, NULL, NULL, &tv_timeout);
        if (retval)
                return (read(fd, data, datalength));
        else
                return (-1);
}

int ReadComPort(int fd, void *buf, int nbytes)
{
        int nleft;
        int nread;

        nleft = nbytes;

        while (nleft > 0)
        {
                if ((nread = _ReadComPort(fd, buf, nleft)) < 0)
                {
                        if (nleft == nbytes)
                                return (-1); /* error, return -1 */
                        else
                                break; /* error, return amount read so far */
                }
                else if (nread == 0)
                {
                        break; /* EOF */
                }
                nleft -= nread;
                buf += nread;
        }
        return (nbytes - nleft); /* return >= 0 */
}

int WriteComPort(int fd, char *data, int datalength)
{
        fd_set fs_write;
        struct timeval tv_timeout;
        int retval, len, total_len;

        FD_ZERO(&fs_write);
        FD_SET(fd, &fs_write);
        tv_timeout.tv_sec = TIMEOUT_SEC(datalength, GetBaudrate());
        tv_timeout.tv_usec = TIMEOUT_USEC;

        for (total_len = 0, len = 0; total_len < datalength;)
        {
                retval = select(fd + 1, NULL, &fs_write, NULL, &tv_timeout);
                if (retval)
                {
                        len = write(fd, &data[total_len], datalength - total_len);
                        if (len > 0)
                                total_len += len;
                }
                else
                {
                        tcflush(fd, TCOFLUSH); /* flush all output data */
                        break;
                }
        }

        return (total_len);
}

/* get serial port baudrate */
int GetBaudrate()
{
        return (_BAUDRATE(cfgetospeed(&termios_new)));
}

/* set serial port baudrate by use of file descriptor fd */
void SetBaudrate(int baudrate)
{
        termios_new.c_cflag = BAUDRATE(baudrate); /* set baudrate */
}

void SetDataBit(int databit)
{
        termios_new.c_cflag &= ~CSIZE;
        switch (databit)
        {
        case 8:
                termios_new.c_cflag |= CS8;
                break;
        case 7:
                termios_new.c_cflag |= CS7;
                break;
        case 6:
                termios_new.c_cflag |= CS6;
                break;
        case 5:
                termios_new.c_cflag |= CS5;
                break;
        default:
                termios_new.c_cflag |= CS8;
                break;
        }
}

void SetStopBit(const char *stopbit)
{
        if (0 == strcmp(stopbit, "1"))
        {
                termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
        }
        else if (0 == strcmp(stopbit, "1.5"))
        {
                termios_new.c_cflag &= ~CSTOPB; /* 1.5 stop bits */
        }
        else if (0 == strcmp(stopbit, "2"))
        {
                termios_new.c_cflag |= CSTOPB; /* 2 stop bits */
        }
        else
        {
                termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
        }
}

void SetParityCheck(char parity)
{
        switch (parity)
        {
        case 'N': /* no parity check */
                termios_new.c_cflag &= ~PARENB;
                break;
        case 'E': /* even */
                termios_new.c_cflag |= PARENB;
                termios_new.c_cflag &= ~PARODD;
                break;
        case 'O': /* odd */
                termios_new.c_cflag |= PARENB;
                termios_new.c_cflag |= ~PARODD;
                break;
        default: /* no parity check */
                termios_new.c_cflag &= ~PARENB;
                break;
        }
}

int SetPortAttr(int fd, int baudrate, int databit, const char *stopbit, char parity)
{
        bzero(&termios_new, sizeof(termios_new));

        cfmakeraw(&termios_new);
        SetBaudrate(baudrate);
        termios_new.c_cflag |= CLOCAL | CREAD; /* | CRTSCTS */
        SetDataBit(databit);
        SetParityCheck(parity);
        SetStopBit(stopbit);
        termios_new.c_oflag = 0;
        //termios_new.c_lflag |= 0;
        termios_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input，非经典接收模式，不必以回车键为接收字符串时的结束符*/
        termios_new.c_oflag &= ~OPOST;
        termios_new.c_cc[VTIME] = 1; /* unit: 1/10 second. */
        termios_new.c_cc[VMIN] = 1;  /* minimal characters for reading */
        tcflush(fd, TCIFLUSH);

        return (tcsetattr(fd, TCSANOW, &termios_new));
}

int BAUDRATE(int baudrate)
{
        switch (baudrate)
        {
        case 0:
                return (B0);
        case 50:
                return (B50);
        case 75:
                return (B75);
        case 110:
                return (B110);
        case 134:
                return (B134);
        case 150:
                return (B150);
        case 200:
                return (B200);
        case 300:
                return (B300);
        case 600:
                return (B600);
        case 1200:
                return (B1200);
        case 2400:
                return (B2400);
        case 9600:
                return (B9600);
        case 19200:
                return (B19200);
        case 38400:
                return (B38400);
        case 57600:
                return (B57600);
        case 115200:
                return (B115200);
        default:
                return (B9600);
        }
}

int _BAUDRATE(int baudrate)
{
        /* reverse baudrate */
        switch (baudrate)
        {
        case B0:
                return (0);
        case B50:
                return (50);
        case B75:
                return (75);
        case B110:
                return (110);
        case B134:
                return (134);
        case B150:
                return (150);
        case B200:
                return (200);
        case B300:
                return (300);
        case B600:
                return (600);
        case B1200:
                return (1200);
        case B2400:
                return (2400);
        case B9600:
                return (9600);
        case B19200:
                return (19200);
        case B38400:
                return (38400);
        case B57600:
                return (57600);
        case B115200:
                return (115200);
        default:
                return (9600);
        }
}



//16进制显示
void hexdump(char *notice, void *buffer, int len)
{
        int i = 0;
        unsigned char *ptr = (unsigned char *)buffer;
        printf("[%s][lentgh=%d]  ", notice, len);
        for (i = 0; i < len; i++)
        {
                printf("0x%02X ", *(ptr + i));
        }
        printf("\n");
}


int main(void)
{
        //char sendcmd[] = {0x68, 0x17, 0x00, 0x43, 0x05, 0x14, 0x01, 0x50, 0x22, 0x01, 0x18, 0x10, 0xAD, 0x75, 0x05, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x74, 0xDE, 0x16};
        char sendcmd[] = {0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89};
        int fd;
        int num = 1000;
        int nread, nwrite, i;
        unsigned char buff[1024];

        struct timeval t;  //delay function
        t.tv_sec = 1;      //second
        t.tv_usec = 10000; //microsecond

        if ((fd = OpenComPort(3, 9600, 7, "1", 'E')) < 0)
        {
                perror("open_port error");
                return;
        }

        printf("fd=%d\n", fd);
        write(fd, sendcmd, sizeof(sendcmd));
        while (1)
        {
                nanosleep(&t, NULL); //delay
                nread = read(fd, buff, 1024);

                if (nread > 0)
                {
                        hexdump("serial read", buff, nread); 
                }
        }
        // close(fd);
        return;
}


