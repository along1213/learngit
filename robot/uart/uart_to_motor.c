#include <stdio.h>
#include <string.h>
#include <uart_manager.h>
#include <pthread.h>
#include <unistd.h>

int UartToMotorInit(void);
int UartToMotorRead(char* strData, int n);
int UartToMotorWrite(char* strData, int n);
void *read_thread(void *arg);
static T_UartOpr g_tUartToMotor = 
{
    .pcName = "UartToMotor",
    .pcDev  = "/dev/ttyS4",
    .init   = UartToMotorInit,
    .read   = UartToMotorRead,
    .write  = UartToMotorWrite,
    .mutex_read_write = PTHREAD_MUTEX_INITIALIZER
};

// void *read_thread(void *arg)
// {
//     int iReadLen = 0;

//     while (1)
//     {
//         bzero(g_tUartToMotor.strBuf, 100);
        
//         iReadLen = g_tUartToMotor.read(g_tUartToMotor.strBuf, 100);
//         if (iReadLen != 0)
//             printf("get %d: %s\n", iReadLen, g_tUartToMotor.strBuf);
//     }
// }

// void *write_thread(void *arg)
// {
//     while (1)
//     {
//         g_tUartToMotor.write("hello lafin1111", strlen("hello lafin111"));
//         sleep(1);
//     }  
// }

int UartToMotorInit(void)
{
    g_tUartToMotor.iFd = uart_init(g_tUartToMotor.pcDev, 115200, 8, 'N', 1);
    if (g_tUartToMotor.iFd < 0)
        return -1;

    // pthread_create(&g_tUartToMotor.tReadTid,  NULL, read_thread,  NULL);
    // pthread_create(&g_tUartToMotor.tWriteTid, NULL, write_thread, NULL);
    return 0;
}


int UartToMotorRead(char* strData, int n)
{
    int iReadLen = 0;
    
    pthread_mutex_lock(&g_tUartToMotor.mutex_read_write);

    iReadLen = read(g_tUartToMotor.iFd, strData, n);

    pthread_mutex_unlock(&g_tUartToMotor.mutex_read_write);

    return iReadLen;
}

int UartToMotorWrite(char* strData, int n)
{
    int iWriteLen = 0;

    pthread_mutex_lock(&g_tUartToMotor.mutex_read_write);

    iWriteLen = write(g_tUartToMotor.iFd, strData, n);

    pthread_mutex_unlock(&g_tUartToMotor.mutex_read_write);

    return iWriteLen;
}

int UartToMotorRegister(void)
{
    return RegisterUartOpr(&g_tUartToMotor);
}
