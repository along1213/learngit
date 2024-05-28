#include <stdio.h>
#include <stdlib.h>
#include <motor_manager.h>
#include <arm_control.h>
#include <unistd.h>
#include <pthread.h>
#include <net_manager.h>
/*对全局6轴角度的读写操作一定要互斥*/
pthread_mutex_t mtxReadWriteAngleArray = PTHREAD_MUTEX_INITIALIZER;

static float g_afAngelGet[6] = {-1};

/**
 * 关节角度点动
*/
int JointJogByAngle(float *pfAngleSet, int iNumOfMotor)
{
    PT_MotorOpr ptMotorOpr = GetMotorOprByName("RMDMotor");

    float afAngle[6];
    GetSixAngle(afAngle);

    for (int i = 0; i < iNumOfMotor; i++)
    {
        if (pfAngleSet[i] - afAngle[i] > 0)  /* 根据当前角度判断旋转方向 */
        {
            ptMotorOpr->SetAngle(i+1, pfAngleSet[i], CLOCKWISE, 100);
        }
        else if (pfAngleSet[i] - afAngle[i] < 0)
        {
            ptMotorOpr->SetAngle(i+1, pfAngleSet[i], ANTICLOCKWISE, 100);
        }

        usleep(5000);
    }
}

/**
 * 线程使用的读取关节角度
 * 只有角度读取线程一个线程使用，所以不用加互斥
*/
void GetJointAngle(float* pfJointANgle, int iNumOfMotor)
{
    PT_MotorOpr ptMotorOpr = GetMotorOprByName("RMDMotor");

    for (int i = 0; i < iNumOfMotor; i++)
    {
        pfJointANgle[i] = ptMotorOpr->GetAngle(i + 1);
    }
}

/**
 * 关节角度线线程
*/
void* Thread_GetJointAngle(void* argv)
{
    while (1)
    {
        pthread_mutex_lock(&mtxReadWriteAngleArray);

        GetJointAngle(g_afAngelGet, 6);

        pthread_mutex_unlock(&mtxReadWriteAngleArray);

        // printf("get angle : ");
        // for (int i = 0; i < 6; i++)
        //     printf("%.2f ", g_afAngelGet[i]);
        // printf("\n");

        usleep(10000);
    }
}

/**
 * 其他函数调用此函数，获取线程读出的角度
*/
void GetSixAngle(float afAngle[6])
{
    pthread_mutex_lock(&mtxReadWriteAngleArray);

    for (int i = 0; i < 6; i++)
    {
        afAngle[i] = g_afAngelGet[i];
    }

    pthread_mutex_unlock(&mtxReadWriteAngleArray);
}

/**
 * 等到关节角度到达目标角度
*/
void WaitJointAngleArrive(float *pfGoalAngel, int iNumOfMotor)
{
    float afAngle[6];

    GetSixAngle(afAngle);

    for (int i = 0; i < iNumOfMotor; i++)
    {
        while (abs(afAngle[i] - pfGoalAngel[i]) > 0.1)
        {
            usleep(10000);
        }
    }
}

int JointThreadInit()
{
    int iRet = 0;
    pthread_t tIdGetJointAngle = -1;

    iRet = pthread_create(&tIdGetJointAngle, NULL, Thread_GetJointAngle, (void *)NULL);
    if (iRet != 0)
    {
        printf("create get joint angle thread err!!!\n");
        return -1;
    } 

    printf("tId = %u\n", tIdGetJointAngle);
}