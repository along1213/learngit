#include <net_manager.h>
#include <pthread.h>
#include <unistd.h>
#include <arm_control.h>
#include <cJSON.h>
#include <string.h>
#include <stdlib.h>

cJSON *g_tJsonRoot = NULL;

void JsonAngleLoad(cJSON *jsonRoot)
{
    cJSON* JAngle = cJSON_GetObjectItem(jsonRoot, "JAngle");
    float afAngle[6];
    GetSixAngle(afAngle);

    for (int i = 0; i < 6; i++)
    {
        char str[7] = "angle";
        sprintf(str, "angle%d", i);
        str[6] = '\0';

        cJSON_ReplaceItemInArray(JAngle, i, cJSON_CreateNumber(afAngle[i]));
    }
}

/**
 * 初始化json结构体
 * root
 * {
 *      "JAngle"    {
 *             angle
 *      }
 * }
*/
void JsonInit()
{
    g_tJsonRoot = cJSON_CreateObject();

    /* ip地址*/
    cJSON_AddStringToObject(g_tJsonRoot, "ip", "192.168.5.9");

    /* 6轴角度数组 */
    cJSON* JAngle = cJSON_CreateArray();
    cJSON_AddItemToObject(g_tJsonRoot, "JAngle", JAngle);
    for (int i = 0; i < 6; i++)
    {
        char str[7];
        sprintf(str, "angle%d", i);
        str[6] = '\0';

        cJSON_AddItemToArray(JAngle,  cJSON_CreateNumber(i));
    }

    JsonAngleLoad(g_tJsonRoot);

    char *str = cJSON_Print(g_tJsonRoot);

    printf("%s", str);
}

char* JsonInfoPrePare()
{
    JsonAngleLoad(g_tJsonRoot);
    return cJSON_Print(g_tJsonRoot); 
}

void* Thread_NetToIPC(void *p)
{
    PT_NetOpr ptNetOpr = GetNetOprByName("eth0");
    // char aSendBuf[128] = "aaa";

    // /* socket初始化并开始监听 */
    //ptNetOpr->Init();

    int iAddrLen = sizeof(struct sockaddr);
    int iFdCLient = 0;
    struct sockaddr_in tClientAddr;

    while (1)
    {
        memset(&tClientAddr, 0, sizeof(tClientAddr));
        iFdCLient = accept(ptNetOpr->iFd, (struct sockaddr*)&tClientAddr, &iAddrLen);
        if (iFdCLient < 0)
        {
            printf("accpet err!!!\n");
            return -1;
        }

        printf("connected with %s\n", inet_ntoa(tClientAddr.sin_addr));

        while (1)
        {
            // char strRecvBuf[100];
            
            // int n = ptNetOpr->Recv(iFdCLient, strRecvBuf, 100);
            // if (n < 1)
            // {
            //     break;
            // }

            // strRecvBuf[n] = '\0';
            // printf("%s\n", strRecvBuf);
            // usleep(5000);

            char *pcSendBuf = JsonInfoPrePare();

            int iRet = send(iFdCLient, pcSendBuf, strlen(pcSendBuf), 0);

            free(pcSendBuf);
            
            usleep(200000);
        }

        close(iFdCLient);
    }
    close(ptNetOpr->iFd);
}

int NetThreadInit()
{
    int iRet = 0;
    pthread_t tIdNetToIPC = -1;

    iRet = pthread_create(&tIdNetToIPC, NULL, Thread_NetToIPC, (void *)NULL);
    if (iRet != 0)
    {
        printf("create net to IPC thread err!!!\n");
        return -1;
    } 

    printf("tId = %u\n", tIdNetToIPC);
}
