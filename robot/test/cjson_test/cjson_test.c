#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

int main(int argc, char** argv)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddObjectToObject(root, "JAngle");
    cJSON *JAngle = cJSON_GetObjectItem(root, "JAngle");

    cJSON_AddStringToObject(root, "ip", "192.168.5.9");
    cJSON_AddNumberToObject(JAngle, "angle1", 0);
    cJSON_AddNumberToObject(JAngle, "angle2", 0);
    cJSON_AddNumberToObject(JAngle, "angle3", 0);
    cJSON_AddNumberToObject(JAngle, "angle4", 0);
    cJSON_AddNumberToObject(JAngle, "angle5", 0);
    cJSON_AddNumberToObject(JAngle, "angle6", 0);

    char *str;
    str = cJSON_Print(root);

    cJSON *parsedJSON = cJSON_Parse(str);

    char *ip = cJSON_GetObjectItem(parsedJSON, "ip")->valuestring;
    
    double dAngle[6];
    dAngle[0] = cJSON_GetObjectItem(cJSON_GetObjectItem(root, "JAngle"), "angle1")->valuedouble ;
    // dAngle[1] = cJSON_GetObjectItem(parsedJSON, "angle2")->valuedouble;
    // dAngle[2] = cJSON_GetObjectItem(parsedJSON, "angle3")->valuedouble;
    // dAngle[3] = cJSON_GetObjectItem(parsedJSON, "angle4")->valuedouble;
    // dAngle[4] = cJSON_GetObjectItem(parsedJSON, "angle5")->valuedouble;
    // dAngle[5] = cJSON_GetObjectItem(parsedJSON, "angle6")->valuedouble;

    printf("%s\n", str);
    printf("%s\n", ip);

    for (int i = 0; i < 6; i++)
    {
        printf("angle%d = %f\n", i, dAngle[i]);
    }
}
