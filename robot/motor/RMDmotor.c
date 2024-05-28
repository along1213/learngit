#include <uart_manager.h>
#include <motor_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t g_mtxSetOrGetAngle = PTHREAD_MUTEX_INITIALIZER;

void  SetAngle(unsigned char ucId, float fGoalAngle, unsigned char ucDir, unsigned short usSpeed);
float GetAngle(unsigned char ucId);
int  MotorInit(void);

static T_UartOpr g_tMotorUartOpr;

static T_MotorOpr g_MotorOpr = {
    .ucMotorNum = 1,
    .pcName    = "RMDMotor",
    .Init       = MotorInit,
    .SetAngle   = SetAngle,
    .GetAngle   = GetAngle
};

unsigned short GetModbusCRC16(unsigned char *data, unsigned short length) 
{
    unsigned short crc = 0xFFFF;
    unsigned short i;

    while (length--) 
    {
        crc ^= *data++;
        for (i = 0; i < 8; i++) 
        {
            if (crc & 0x0001) 
            {
                crc >>= 1;
                crc ^= 0xA001;
            } 
            else 
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}

float GetAngle(unsigned char ucId)
{
    unsigned short usGoalAngle;

    unsigned char strMsg[13] = {0};

    unsigned short usCrc16 = 0;

    /* header */
    strMsg[0]  = 0x3e;

    /* motor id */
    strMsg[1]  = ucId;

    /* data len */
    strMsg[2]  = 0x08;

    /* func code */
    strMsg[3]  = 0x92;

    /* not used */
    strMsg[4]  = 0;
    strMsg[5]  = 0;
    strMsg[6]  = 0;
    strMsg[7]  = 0;
    strMsg[8]  = 0;
    strMsg[9]  = 0;
    strMsg[10] = 0;

    usCrc16 = GetModbusCRC16(strMsg, 11);

    /* modbus crc 16 */
    strMsg[11] = usCrc16;
    strMsg[12] = usCrc16 >> 8;


    pthread_mutex_lock(&g_mtxSetOrGetAngle);

    g_tMotorUartOpr.write(strMsg, 13);
    usleep(5000);
    g_tMotorUartOpr.read(strMsg, 13);

    pthread_mutex_unlock(&g_mtxSetOrGetAngle);

    float fAngle = (float)(((strMsg[7]) | (strMsg[8] << 8) | (strMsg[9] << 16) | (strMsg[10] << 24)) % 36000) / 100;

    return  fAngle;
}

void SetAngle(unsigned char ucId, float fGoalAngle, unsigned char ucDir, unsigned short usSpeed)
{
    if (fGoalAngle > 350 || fGoalAngle < 10)
    {
        printf("set angle err!!!\n");
        return;
    }
    unsigned short usGoalAngle = (unsigned short)(fGoalAngle * 100);

    unsigned char strMsg[13] = {0};

    unsigned short usCrc16 = 0;

    /* header */
    strMsg[0]  = 0x3e;

    /* motor id */
    strMsg[1]  = ucId;

    /* data len */
    strMsg[2]  = 0x08;

    /* func code */
    strMsg[3]  = 0xA4;

    /* direct 1 clockwise 0 anticlockwise*/
    strMsg[4]  = ucDir;

    /* speed °/s */
    strMsg[5]  = usSpeed;
    strMsg[6]  = usSpeed >> 8;

    /* goal angle °/s */
    strMsg[7]  = usGoalAngle;
    strMsg[8]  = usGoalAngle >> 8;

    /* not used */
    strMsg[9]  = 0;
    strMsg[10] = 0;

    usCrc16 = GetModbusCRC16(strMsg, 11);

    /* modbus crc 16 */
    strMsg[11] = usCrc16;
    strMsg[12] = usCrc16 >> 8;

    pthread_mutex_lock(&g_mtxSetOrGetAngle);

    g_tMotorUartOpr.write(strMsg, 13);

    pthread_mutex_unlock(&g_mtxSetOrGetAngle);
}

int MotorInit()
{
    g_tMotorUartOpr = *GetUartOprByName("UartToMotor");
} 

int RMDMotorOprRegister()
{
   return RegisterMotorOpr(&g_MotorOpr);
}