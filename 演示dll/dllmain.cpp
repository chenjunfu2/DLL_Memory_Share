// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <string.h>
#include <Windows.h>

#define DATA_SIZE (1024*1024*4)//4MB

#pragma data_seg("Shared")//命名共享数据段
CRITICAL_SECTION g_cs = {};//同步用关键段
unsigned long long g_refcount = {};//dll加载引用计数
/************************************/
unsigned char g_data[DATA_SIZE] = {};
unsigned int g_size = {};
/************************************/
#pragma data_seg()

#pragma comment(linker,"/SECTION:Shared,RWS")//可读可写可共享

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)//保证单线程调用
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (g_refcount++ == 0)//第一次加载则创建
		{
			InitializeCriticalSection(&g_cs);//初始化关键段
		}
		break;
	case DLL_PROCESS_DETACH:
		if (--g_refcount == 0)//最后一次卸载则删除
		{
			DeleteCriticalSection(&g_cs);//删除关键段
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) unsigned int WriteData(const unsigned char *data, unsigned int size)
{
	if (size > DATA_SIZE - g_size)//比剩余空间大
	{
		size = DATA_SIZE - g_size;
	}

	EnterCriticalSection(&g_cs);
	memcpy(&g_data[g_size], &data[0], size);
	g_size += size;
	LeaveCriticalSection(&g_cs);

	return size;
}

extern "C" __declspec(dllexport) unsigned int ReadData(unsigned char *data, unsigned int size)
{
	if (size > g_size)
	{
		size = g_size;
	}

	EnterCriticalSection(&g_cs);
	memcpy(&data[0], &g_data[0], size);
	g_size -= size;
	memmove(&g_data[0], &g_data[size], g_size);
	LeaveCriticalSection(&g_cs);

	return size;
}

extern "C" __declspec(dllexport) void ClearData(void)
{
	EnterCriticalSection(&g_cs);
	g_size = 0;
	LeaveCriticalSection(&g_cs);
}

extern "C" __declspec(dllexport) unsigned int FillData(int val, unsigned int size)
{
	if (size > DATA_SIZE)
	{
		size = DATA_SIZE;
	}

	EnterCriticalSection(&g_cs);
	memset(g_data, val, size);
	g_size = size;
	LeaveCriticalSection(&g_cs);

	return size;
}

extern "C" __declspec(dllexport) unsigned int ZeroData(unsigned int size)
{
	return FillData(0, size);
}

extern "C" __declspec(dllexport) unsigned int SizeData(void)
{
	EnterCriticalSection(&g_cs);
	unsigned int ret = g_size;
	LeaveCriticalSection(&g_cs);
	
	return ret;
}

extern "C" __declspec(dllexport) unsigned int MaxDataSize(void)
{
	return DATA_SIZE;
}

extern "C" __declspec(dllexport) unsigned long long ReferenceCount(void)
{
	EnterCriticalSection(&g_cs);
	unsigned long long ret = g_refcount;//可能多线程调用
	LeaveCriticalSection(&g_cs);

	return ret;
}
