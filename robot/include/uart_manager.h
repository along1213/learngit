#ifndef _UART_MANAGER_H
#define _UART_MANAGER_H

#include <pthread.h>

typedef struct UartOpr {
    int iFd;
    pthread_t tReadTid;
    pthread_t tWriteTid;
    char *pcName;
    char *pcDev;  
    char strBuf[100];
    int (*init) (void);
    int (*read) (char* strData, int n);
    int (*write)(char* strData, int n);
    struct UartOpr* ptNext;
    pthread_mutex_t mutex_read_write;
}T_UartOpr, * PT_UartOpr;

int AllUartInit(void);
int AllUartRegister(void);
PT_UartOpr GetUartOprByName(char *name);
int RegisterUartOpr(PT_UartOpr ptUartOpr);
int uart_init(char *com, int nSpeed, int nBits, char nEvent, int nStop);
int UartToMotorRegister(void);

#endif