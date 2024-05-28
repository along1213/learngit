#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP   "192.168.5.2"
#define SERVER_PORT 1213

typedef struct {
    char strName[20];
    char strIp[16];
    unsigned short int sPort;
    int (*Init)(char *pcIp, unsigned short int sPort);
    
}


static struct sockaddr_in g_tServerAddr;
static int g_iFdServer = 0;

int ServerInit(char* pcIp, unsigned short int sPort)
{
    int iRet = 0;
    int iAddrLen = sizeof(struct sockaddr);

    g_tServerAddr.sin_family = AF_INET;
    g_tServerAddr.sin_port   = htons(SERVER_PORT);
    inet_aton(SERVER_IP, &g_tServerAddr.sin_addr);
    memset(&g_tServerAddr.sin_zero, 0, 8);

    g_iFdServer = socket(AF_INET, SOCK_STREAM, 0);
    if (g_iFdServer < 0)
    {
        printf("socket err!!!\n");
        return -1;
    }

    const int on=1;
    setsockopt(g_iFdServer,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    iRet = bind(g_iFdServer, (struct sockaddr*)&g_tServerAddr, iAddrLen);
    if (iRet < 0)
    {
        printf("bind err\n");
    }
    iRet = listen(g_iFdServer, 1);
    if (iRet < 0)
    {
        printf("listen err\n");
    }
    printf("----------------------wait for requests---------------------\n");
}

int main(int argc, char** argv)
{
    int iAddrLen = sizeof(struct sockaddr);
    int iFdCLient = 0;
    struct sockaddr_in tClientAddr;

    ServerInit();

    while (1)
    {
        memset(&tClientAddr, 0, sizeof(tClientAddr));
        iFdCLient = accept(g_iFdServer, (struct sockaddr*)&tClientAddr, &iAddrLen);
        if (iFdCLient < 0)
        {
            printf("accpet err!!!\n");
            return -1;
        }

        printf("connected with %s\n", inet_ntoa(tClientAddr.sin_addr));

        while (1)
        {
            char strRecvBuf[100];

            int n = recv(iFdCLient, strRecvBuf, 100, 0);
            if (n < 1)
            {
                break;
            }

            strRecvBuf[99] = '\0';
            printf("%s\n", strRecvBuf);
        }

        close(iFdCLient);
    }
    
    close(g_iFdServer);
}




