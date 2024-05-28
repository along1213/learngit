#include <motor_manager.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static PT_MotorOpr g_ptMotorOprHead;

int RegisterMotorOpr(PT_MotorOpr ptMotorOpr)
{
	PT_MotorOpr temp = g_ptMotorOprHead;

	if (temp == NULL)
	{
		g_ptMotorOprHead = ptMotorOpr;
		return 0;
	}

	while (temp->ptNext)
	{
		temp = temp->ptNext;
	}
	temp->ptNext = ptMotorOpr;
	ptMotorOpr->ptNext = NULL;

	return 0;
}

int AllMotorRegister(void)
{
	return RMDMotorOprRegister();
}

int AllMotorInit(void)
{
	int iRet = 0;
	PT_MotorOpr temp = g_ptMotorOprHead;
	
	while (temp)
	{
		iRet = temp->Init();
		if (iRet < 0)
		{
			printf("%s init error!!!\n", temp->pcName);
			return -1;
		}

		printf("%s init success!!!\n", temp->pcName);
		
		temp = temp->ptNext;
	};

	return 0;	
}

PT_MotorOpr GetMotorOprByName(char *name)
{
	int iRet = 0;
	PT_MotorOpr temp = g_ptMotorOprHead;

	while (temp)
	{
		if (0 == strcmp(name, temp->pcName))
		{
			return temp;
		}

		temp = temp->ptNext;
	};

	return NULL;	
}