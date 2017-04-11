#ifndef SMOBJECT_H
#define SMOBJECT_H

#include <Windows.h>
#include <tchar.h>
#include <SMfcn.h>

class SMObject
{
	HANDLE CreateHandle;
	HANDLE AccessHandle;
	TCHAR *szName;
	int Size;
public:
	void *pData;
	int SMCreateError;
	int SMAccessError;
public:
	SMObject();
	SMObject(TCHAR* szname, int size);

	~SMObject();
	int SMCreate();
	int SMAccess();
};
#endif


