#include <uart_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static T_UartOpr g_tMotorUartOpr;

unsigned short GetModbusCRC16(unsigned char *data, unsigned short length) {
    unsigned short crc = 0xFFFF;
    unsigned short i;

    while (length--) {
        crc ^= *data++;
        for (i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

void motorinit()
{
    g_tMotorUartOpr = *GetUartOprByName("UartToMotor");
}

void readAngle(unsigned char ucId)
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

    g_tMotorUartOpr.write(strMsg, 13);
    g_tMotorUartOpr.read(strMsg, 13);

    printf("angle = %u angle = %u\n",((strMsg[7]) | (strMsg[8] << 8) | (strMsg[9] << 16) | (strMsg[10] << 24)), ((strMsg[7]) | (strMsg[8] << 8) | (strMsg[9] << 16) | (strMsg[10] << 24)) % 36000 );
}

void setAngle(unsigned char ucId, float fGoalAngle, unsigned char ucDir, unsigned short usSpeed)
{
    unsigned short usGoalAngle = fGoalAngle * 100;

    unsigned char strMsg[13] = {0};

    unsigned short usCrc16 = 0;

    /* header */
    strMsg[0]  = 0x3e;

    /* motor id */
    strMsg[1]  = ucId;

    /* data len */
    strMsg[2]  = 0x08;

    /* func code */
    strMsg[3]  = 0xA6;

    /* direct 0 clockwise 1 anticlockwise*/
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

    g_tMotorUartOpr.write(strMsg, 13);
}

int main(int argc, char **argv)
{
    AllUartRegister();
    AllUartInit();
    motorinit();
    
    unsigned char  ucId    = 0;  
    float          fAngle  = 0;
    unsigned char  ucDir   = 0;  
    unsigned short usSpeed = 0;

    if (argc == 2)
    {
        ucId    = strtoul(argv[1], NULL, 10);  
        readAngle(ucId);
    }
    else if (argc == 5)
    {
        ucId    = strtoul(argv[1], NULL, 10);  
        fAngle  = strtof(argv[2], NULL);
        ucDir   = strtoul(argv[3], NULL, 10);  
        usSpeed = strtoul(argv[4], NULL, 10);
        printf(" dir = %u angle = %f id = %u speed = %u\n",ucId, fAngle, ucDir, usSpeed);
        setAngle(ucId, fAngle, ucDir, usSpeed);
    }
}