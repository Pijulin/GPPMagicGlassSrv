/********************************************************************
	Created:	2011/04/13
	Created:	13:4:2011   16:44
	Filename: 	AutoLocker.h
	File base:	AutoLocker
	File ext:	h
	Author:		 
	
	Purpose:	自动锁
*********************************************************************/

#ifndef __AUTO_LOCKER_H__
#define __AUTO_LOCKER_H__

#include <windows.h>
//#include "stdafx.h"

namespace _BaseModule
{
	class ILocker
	{
	public:
		virtual BOOL __stdcall Lock(void) = 0;
		virtual BOOL __stdcall Unlock(void) = 0;
	};

	class CCriticalSection : public ILocker
	{
	private:
		// 禁用拷贝构造函数及等号操作符
		CCriticalSection( const CCriticalSection & s ) {} 
		CCriticalSection & operator = ( const CCriticalSection & s ) { return * this; }
         
	public:
		CCriticalSection()
		{ 
			::InitializeCriticalSection(&m_sect); 
		}

		virtual ~CCriticalSection()
		{ 
			::DeleteCriticalSection(&m_sect); 
		}

		virtual BOOL __stdcall Lock()
		{ 
			::EnterCriticalSection(&m_sect); 
			return TRUE; 
		}
		
		virtual BOOL __stdcall Unlock()
		{ 
			::LeaveCriticalSection(&m_sect); 
			return TRUE; 
		}

	private:
		CRITICAL_SECTION m_sect;
	};

	template < typename T >
	class CAutoLockerT
	{
	private:
		// 禁用普通构造函数，拷贝构造函数及等号操作符
		CAutoLockerT() : m_pLocker(NULL) {}
		CAutoLockerT( const CAutoLockerT & s ) : m_pLocker(NULL){};
		CAutoLockerT & operator = ( const CAutoLockerT & s ) { return * this; }

	public:
		CAutoLockerT(T * pLocker) : m_pLocker(pLocker)
		{
			if(m_pLocker)
			{
				m_pLocker->Lock();
			}
		}

		virtual ~CAutoLockerT()
		{
			if(m_pLocker)
			{
				m_pLocker->Unlock();
				m_pLocker = NULL;
			}
		}

	private:
		T * m_pLocker;
	};

	class CAutoLocker
	{
	private:
		// 禁用普通构造函数，拷贝构造函数及等号操作符
		CAutoLocker() : m_pLocker(NULL) {}
		CAutoLocker( const CAutoLocker & s ) : m_pLocker(NULL){};
		CAutoLocker & operator = ( const CAutoLocker & s ) { return * this; }

	public:
		CAutoLocker(ILocker * pLocker) : m_pLocker(pLocker)
		{
			if(m_pLocker)
			{
				m_pLocker->Lock();
			}
		}

		virtual ~CAutoLocker()
		{
			if(m_pLocker)
			{
				m_pLocker->Unlock();
				m_pLocker = NULL;
			}
		}

	private:
		ILocker * m_pLocker;
	};

};

#define BASEMODULE_AUTO_LOCKER( l )	_BaseModule::CAutoLocker __auto_locker(&l);

#endif	// #ifndef __AUTO_LOCKER_H__

