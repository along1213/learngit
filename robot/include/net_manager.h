#ifndef _NET_MANAGER_H
#define _NET_MANAGER_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct NetOpr{
    int iFd;
    char strName[20];
    char strIp[16];
    unsigned short int sPort;
    int (*Init)();
    int (*Send)(int iSrcFd, char *strBuf, int len);
    int (*Recv)(int iSrcFd, char *strBuf, int len);
    struct sockaddr_in tAddr_in;
    struct NetOpr* ptNext;
}T_NetOpr, *PT_NetOpr;

int Eth0NetOprRegister();

int AllNetInit(void);
int RegisterNetOpr(PT_NetOpr ptNetOpr);
int AllNetRegister(void);
PT_NetOpr GetNetOprByName(char *name);

#endif