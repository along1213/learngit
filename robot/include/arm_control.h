#ifndef _ARM_CONTROL_H
#define _ARM_CONTROL_H

int  JointJogByAngle(float angle[6], int iNumOfMotor);
void GetSixAngle(float afAngle[6]);
void WaitJointAngleArrive(float *pfGoalAngel, int iNumOfMotor);
int JointThreadInit();
int NetThreadInit();
void JsonInit();
#endif