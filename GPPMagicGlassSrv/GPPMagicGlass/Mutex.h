#pragma once
#include "handle.h"

struct  Mutex : public Handle
{
    Mutex()
    {
        Create(L"");
    }
    Mutex(
        __in_opt LPCWSTR lpName,
        __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL,
        __in     BOOL bInitialOwner = FALSE)
    {
        Create(lpName, lpMutexAttributes, bInitialOwner);
    }
    Mutex(__in_opt LPCSTR lpName,
        __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL,
        __in     BOOL bInitialOwner = FALSE)
    {
        Create(lpName, lpMutexAttributes, bInitialOwner);
    }

	Mutex(HANDLE Value) : Handle(Value)
	{
	}


	HANDLE operator=(HANDLE Value) 
	{ 
		Handle* handle = this;	
		*handle = Value; 
		return *handle; 
	}

    VOID Create(
        __in_opt LPCSTR lpName = NULL,
        __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL,
        __in     BOOL bInitialOwner = FALSE
    )
    {
        m_handle.reset(CreateMutexA(lpMutexAttributes, bInitialOwner, lpName));
    }

	VOID Create(
        __in_opt LPCWSTR lpName = NULL,
		__in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL,
		__in     BOOL bInitialOwner = FALSE
	)
	{
        m_handle.reset(CreateMutexW(lpMutexAttributes, bInitialOwner, lpName));
	}

	VOID Open(
		__in LPCTSTR lpName,
		__in DWORD dwDesiredAccess = MUTEX_ALL_ACCESS,
		__in BOOL bInheritHandle = FALSE
		)
	{
		m_handle.reset(OpenMutex(dwDesiredAccess, bInheritHandle, lpName));
	}

	BOOL Lock(INT WaitMilliseconds = INFINITE)
	{
		if (m_handle.get() == 0)
			return FALSE;

		return WAIT_OBJECT_0 == WaitForSingleObject(m_handle.get(), WaitMilliseconds);
	}

	static BOOL Lock(HANDLE Handle, UINT WaitMilliseconds = INFINITE)
	{
		if (Handle == 0)
			return FALSE;

		return WAIT_OBJECT_0 == WaitForSingleObject(Handle, WaitMilliseconds);
	}

	BOOL Unlock()
	{
		if (m_handle.get() == 0)
			return FALSE;

		return ReleaseMutex(m_handle.get());
	}

	BOOL Unlock(HANDLE Handle)
	{
		if (Handle == 0)
			return FALSE;

		return ReleaseMutex(Handle);
	}
};

struct  MutexLocker
{
	Mutex *mutex;

	MutexLocker(Mutex &Value)
	{
		mutex = &Value;
		mutex->Lock(INFINITE);
	}

	~MutexLocker()
	{
		mutex->Unlock();
	}
};