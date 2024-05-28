#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "uart_manager.h"

static PT_UartOpr g_ptUartOprHead = NULL;

/**
 *  uart_init(com,115200,8,'N',1) 
 */
int uart_init(char *com, int nSpeed, int nBits, char nEvent, int nStop)
{
	int fd;
	int iRet;
    struct termios newtio,oldtio;

	//fd = open(com, O_RDWR|O_NOCTTY|O_NDELAY);
	fd = open(com, O_RDWR|O_NOCTTY);
	if (fd < 0)
	{
		printf("open %s err!\n", com);
		return -1;
	}

    if(fcntl(fd, F_SETFL, 0)<0) /* 设置串口为阻塞状态*/
    {
        printf("fcntl failed!\n");
        return -1;
    }

	if ( tcgetattr( fd,&oldtio) != 0) { 
		perror("SetupSerial 1");
		return -1;
	}
	
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag |= CLOCAL | CREAD; 
	newtio.c_cflag &= ~CSIZE; 
	newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	newtio.c_oflag  &= ~OPOST;   /*Output*/

	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
	break;
	case 8:
		newtio.c_cflag |= CS8;
	break;
	}

	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
	break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
	break;
	case 'N': 
		newtio.c_cflag &= ~PARENB;
	break;
	}

	switch( nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
	break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
	break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
	break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
	break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
	break;
	}
	
	if( nStop == 1 )
		newtio.c_cflag &= ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |= CSTOPB;
	
	newtio.c_cc[VMIN]  = 0;  /* 读数据时的最小字节数: 没读到这些数据我就不返回! */
	newtio.c_cc[VTIME] = 0; /* 等待第1个数据的时间: 
	                         * 比如VMIN设为10表示至少读到10个数据才返回,
	                         * 但是没有数据总不能一直等吧? 可以设置VTIME(单位是10秒)
	                         * 假设VTIME=1，表示: 
	                         *    10秒内一个数据都没有的话就返回
	                         *    如果10秒内至少读到了1个字节，那就继续等待，完全读到VMIN个数据再返回
	                         */

	tcflush(fd,TCIFLUSH);
	
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}

	return fd;
}

int RegisterUartOpr(PT_UartOpr ptUartOpr)
{
	PT_UartOpr temp = g_ptUartOprHead;

	if (temp == NULL)
	{
		g_ptUartOprHead = ptUartOpr;
		return 0;
	}

	while (temp->ptNext)
	{
		temp = temp->ptNext;
	}
	temp->ptNext = ptUartOpr;
	ptUartOpr->ptNext = NULL;

	return 0;

}

int AllUartRegister(void)
{
	UartToMotorRegister();
}

int AllUartInit(void)
{
	int iRet = 0;
	PT_UartOpr temp = g_ptUartOprHead;
	
	while (temp)
	{
		iRet = temp->init();
		if (iRet < 0)
		{
			printf("%s init error!!!\n", temp->pcName);
			return -1;
		}

		printf("%s init success!!!\n", temp->pcName);
		
		temp = temp->ptNext;
	};

	return 0;	
}

PT_UartOpr GetUartOprByName(char *name)
{
	int iRet = 0;
	PT_UartOpr temp = g_ptUartOprHead;

	while (temp)
	{
		if (0 == strcmp(name, temp->pcName))
		{
			return temp;
		}

		temp = temp->ptNext;
	};

	return NULL;	
}
