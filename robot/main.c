#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <uart_manager.h>
#include <motor_manager.h>
#include <arm_control.h>
#include <net_manager.h>
int main(int argc, char **argv)
{      
    if (argc < 2)
    {
        printf("Usage: %s n to read n angles of joint\n", argv[0]);
        printf("       %s angle1 angle2 .... anglen to set n angles of joint", argv[0]);

        return -1;
    }

    int iMotorNum = atol(argv[1]);   
    AllNetRegister();
    AllMotorRegister();
    AllUartRegister();

    AllMotorInit();
    AllUartInit();
    AllNetInit();
    JointThreadInit();
    NetThreadInit();

    JsonInit();

    usleep(100000);

    if (argc == 2)
    {
        float pfAngleGet[6] = {-1};
        
        GetSixAngle(pfAngleGet);

        printf("the angle read is :\n");
        
        for (int i = 0; i < iMotorNum; i++)
        {
            printf("%4f ", pfAngleGet[i]);
        }
    }
    else if (argc > 1)
    {
        float pfAngleSet[6];

        for (int i = 0; i < argc - 1; i++)
        {
            pfAngleSet[i] = strtof(argv[i+1], NULL);
        }
        JointJogByAngle(pfAngleSet, argc - 1);
        WaitJointAngleArrive(pfAngleSet, argc - 1);
        printf("Each joint has reached the target angle!!!\n");
    }

    while (1);
}
