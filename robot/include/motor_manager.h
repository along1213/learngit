#ifndef _MOTOR_MANAGER_H
#define _MOTOR_MANAGER_H

#define CLOCKWISE      1
#define ANTICLOCKWISE  0

typedef struct MotorOpr
{
    unsigned char ucMotorNum;
    unsigned char* pcName;
    void (*SetAngle)(unsigned char uId, float fGoalAnlge, unsigned char ucDir, unsigned short usSpeed);
    float (*GetAngle)(unsigned char uId);
    int (*Init)(void);
    struct MotorOpr* ptNext;
}T_MotorOpr, * PT_MotorOpr;

int RegisterMotorOpr(PT_MotorOpr ptMotorOpr);
int RMDMotorOprRegister(void);
int AllMotorRegister(void);
int AllMotorInit(void);	
PT_MotorOpr GetMotorOprByName(char *name);
#endif