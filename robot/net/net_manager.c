#include "net_manager.h"

static PT_NetOpr g_ptNetOpr = NULL;

int RegisterNetOpr(PT_NetOpr ptNetOpr)
{
	PT_NetOpr temp = g_ptNetOpr;

	if (temp == NULL)
	{
		g_ptNetOpr = ptNetOpr;
		return 0;
	}

	while (temp->ptNext)
	{
		temp = temp->ptNext;
	}
	temp->ptNext = ptNetOpr;
	ptNetOpr->ptNext = NULL;

	return 0;
}

int AllNetRegister(void)
{
    int iRet = Eth0NetOprRegister();

	return iRet;
}

int AllNetInit(void)
{
	int iRet = 0;
	PT_NetOpr temp = g_ptNetOpr;
	
	while (temp)
	{
		iRet = temp->Init();
		if (iRet < 0)
		{
			printf("%s init error!!!\n", temp->strName);
			return -1;
		}

		printf("%s init success!!!\n", temp->strName);
		
		temp = temp->ptNext;
	};

	return 0;	
}

PT_NetOpr GetNetOprByName(char *name)
{
	int iRet = 0;
	PT_NetOpr temp = g_ptNetOpr;

	while (temp)
	{
		if (0 == strcmp(name, temp->strName))
		{
			return temp;
		}

		temp = temp->ptNext;
	};

	return NULL;	
}
