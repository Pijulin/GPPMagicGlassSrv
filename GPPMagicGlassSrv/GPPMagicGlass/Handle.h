#pragma once

#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <vector>  
#include <string>  
#include <iostream>
#include <memory>
#include <windows.h>

struct HandleDeleter
{
public:

	void operator()(HANDLE h) const 
	{ 
		if (!h || h == INVALID_HANDLE_VALUE) return; CloseHandle(h); 
	}
};

class Handle
{
public:
	std::unique_ptr<void, HandleDeleter> m_handle;

	inline operator HANDLE() const 
	{ 
		return m_handle.get(); 
	}

	Handle() 
	{ 
		m_handle.reset(NULL);
	};

	Handle(HANDLE Value) 
	{
		m_handle.reset(Value); 
	}

	HANDLE operator=(HANDLE Value) 
	{ 
		m_handle.reset(Value); 
		return m_handle.get(); 
	}

	DWORD Wait(UINT WaitMilliseconds = INFINITE)
	{
		if (m_handle.get() == 0)
			return WAIT_FAILED;

		return WaitForSingleObject(m_handle.get(), WaitMilliseconds);
	}

	inline HANDLE* GetAddressOf()
	{
#if (_MSC_VER <= 1600)
		return (HANDLE*)&m_handle._Myptr;
#else
        return (HANDLE*)m_handle.get();
#endif
	}

	inline HANDLE Get()
	{
		return (HANDLE)m_handle.get();
	}

	BOOL IsValidHandle()
	{
		HANDLE handle = (HANDLE)m_handle.get();
		return (handle != 0);
	}



	void Release()
	{
		m_handle.reset(0);
	}
};
#endif