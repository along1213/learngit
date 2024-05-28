#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <net_manager.h>

int ServerInit();
int ServerRecv(int iSrcFd, char *strBuf, int len);
int ServerSend(int iSrcFd, char *strBuf, int len);

static T_NetOpr g_tEthOpr = {
    .strName = "eth0",
    .strIp   = "192.168.5.9",
    .sPort   = 1213,
    .Init    = ServerInit,
    .Recv    = ServerRecv,
    .Send    = ServerSend,
};

int ServerInit()
{
    int iRet = 0;
    int iAddrLen = sizeof(struct sockaddr);

    g_tEthOpr.tAddr_in.sin_family = AF_INET;
    g_tEthOpr.tAddr_in.sin_port   = htons(g_tEthOpr.sPort);
    inet_aton(g_tEthOpr.strIp, &g_tEthOpr.tAddr_in.sin_addr);
    memset(&g_tEthOpr.tAddr_in.sin_zero, 0, 8);

    g_tEthOpr.iFd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_tEthOpr.iFd < 0)
    {
        printf("socket err!!!\n");
        return -1;
    }

    const int on=1;
    setsockopt(g_tEthOpr.iFd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    iRet = bind(g_tEthOpr.iFd, (struct sockaddr*)&g_tEthOpr.tAddr_in, iAddrLen);
    if (iRet < 0)
    {
        printf("bind err\n");
    }
    iRet = listen(g_tEthOpr.iFd, 1);
    if (iRet < 0)
    {
        printf("listen err\n");
    }
    printf("----------------------wait for requests---------------------\n");
}

int ServerRecv(int iSrcFd, char *strBuf, int len)
{
    return recv(iSrcFd, strBuf, len, 0);
}

int ServerSend(int iDesFd, char *strBuf, int len)
{
    return send(iDesFd, strBuf, len, 0);
}

int Eth0NetOprRegister()
{
    return RegisterNetOpr(&g_tEthOpr);
}

// int main(int argc, char** argv)
// {
//     int iAddrLen = sizeof(struct sockaddr);
//     int iFdCLient = 0;
//     struct sockaddr_in tClientAddr;

//     ServerInit();

//     while (1)
//     {
//         memset(&tClientAddr, 0, sizeof(tClientAddr));
//         iFdCLient = accept(g_tEthOpr.iFd, (struct sockaddr*)&tClientAddr, &iAddrLen);
//         if (iFdCLient < 0)
//         {
//             printf("accpet err!!!\n");
//             return -1;
//         }

//         printf("connected with %s\n", inet_ntoa(tClientAddr.sin_addr));

//         while (1)
//         {
//             char strRecvBuf[100];
            
//             int n = g_tEthOpr.Recv(iFdCLient, strRecvBuf, 100);
//             if (n < 1)
//             {
//                 break;
//             }

//             strRecvBuf[n] = '\0';
//             printf("%s\n", strRecvBuf);
//         }

//         close(iFdCLient);
//     }
    
//     close(g_tEthOpr.iFd);
// }




