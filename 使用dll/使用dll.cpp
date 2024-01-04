#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <ctype.h>

int main(void)
{
	HMODULE hDll = LoadLibraryW(L"演示dll.dll");
	if (hDll == NULL)
	{
		printf("[%lld]dll load fail,errcode:%d\n", GetTickCount64(), GetLastError());
		(void)getchar();
		return -1;
	}

	printf("[%lld]dll loaded\n", GetTickCount64());

	typedef unsigned int(*WriteData)(const unsigned char *data, unsigned int size);
	typedef int(*ReadData)(unsigned char *data, unsigned int size);
	typedef void(*ClearData)(void);
	typedef unsigned long long (*ReferenceCount)(void);
	typedef unsigned int (*MaxDataSize)(void);
	typedef unsigned int (*ZeroData)(unsigned int size);
	typedef unsigned int (*FillData)(int val, unsigned int size);
	typedef unsigned int(*SizeData)(void);

	WriteData pWriteData = (WriteData)GetProcAddress(hDll, "WriteData");
	ReadData pReadData = (ReadData )GetProcAddress(hDll, "ReadData");
	ClearData pClearData = (ClearData)GetProcAddress(hDll, "ClearData");
	ReferenceCount pReferenceCount = (ReferenceCount)GetProcAddress(hDll, "ReferenceCount");
	MaxDataSize pMaxDataSize = (MaxDataSize)GetProcAddress(hDll, "MaxDataSize");
	ZeroData pZeroData = (ZeroData)GetProcAddress(hDll, "ZeroData");
	FillData pFillData = (FillData)GetProcAddress(hDll, "FillData");
	SizeData pSizeData = (SizeData)GetProcAddress(hDll, "SizeData");

	printf("[%lld]proc loaded\n", GetTickCount64());

#define LINE_SIZE 1025
	unsigned int ret;
	unsigned char sLine[LINE_SIZE];
	int c;//使用int接返回值防止EOF截断错误

	while ((c = tolower(getchar())) != EOF)
	{
		switch (c)
		{
		case 'r'://read
			{
				unsigned int length = 0;
				(void)scanf("%d", &length);
				if (length > LINE_SIZE)
				{
					length = LINE_SIZE;
				}
				ret = pReadData(sLine, length);
				printf("[%lld]read size:%d,data:%.*s\n", GetTickCount64(), ret, ret, sLine);//截断输出，不以0字符结尾
			}
			break;
		case 'w'://write
			{
				std::streamsize length = std::cin.get((char *)sLine, LINE_SIZE, '\n').gcount();
				ret = pWriteData(sLine, (unsigned int)length);//写入数据，不写入0字符
				printf("[%lld]write size:%d\n", GetTickCount64(), ret);
			}
			break;
		case 's'://size
			{
				while (getchar() != '\n') continue;
				ret = pSizeData();
				printf("[%lld]data size:%d\n", GetTickCount64(), ret);
			}
			break;
		case 'c'://clear
			{
				while (getchar() != '\n') continue;
				pClearData();
				printf("[%lld]data clear\n", GetTickCount64());
			}
			break;
		case 'm'://max size
			{
				while (getchar() != '\n') continue;
				ret = pMaxDataSize();
				printf("[%lld]max data size:%d\n", GetTickCount64(), ret);
			}
			break;
		case 'f'://ref count
			{
				while (getchar() != '\n') continue;
				unsigned long long ret = pReferenceCount();//作用域覆盖重命名
				printf("[%lld]reference count:%lld\n", GetTickCount64(), ret);
			}
			break;
		case 'q'://quit
			{
				FreeLibrary(hDll);
				printf("[%lld]quit\n", GetTickCount64());
				return 0;
			}
			break;
		default://其它指令处理
			{
				while (c != '\n')//防止当前c就是\n的情况
				{
					c = getchar();//清理整行
				}
				printf("[%lld]unknow command\n", GetTickCount64());
			}
			break;
		}
	}


	//ret = pWriteData((const unsigned char *)"Hello World", sizeof("Hello World"));
	//printf("write size:%d\n", ret);
	//
	//ret = pSizeData();
	//printf("data size:%d\n", ret);
	//
	//unsigned char cRead[32];
	//ret = pReadData(cRead, sizeof(cRead));
	//printf("read size:%d,read data:%.32s\n", ret, cRead);
	//
	//pClearData();
	//printf("clear\n");
	//
	//ret = pSizeData();
	//printf("data size:%d\n", ret);

	FreeLibrary(hDll);
	return 0;
}