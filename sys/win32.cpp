#ifdef WIN32
#include <vector>
#include <lemon/sys/thread.h>
#include <lemon/sys/configure.h>
#include <dbghelp.h>

#include <lemonxx/sys/filesystem.hpp>

#pragma comment(lib, "dbghelp.lib")

LemonMutex __globalMutex = LEMON_HANDLE_NULL_VALUE;

static void CreateGlobalMutex(LemonErrorInfo * info)
{
	LEMON_RESET_ERRORINFO(*info);

	__globalMutex = LemonCreateMutex(info);
}

static void ReleaseGlobalMutex(LemonErrorInfo * info)
{
	LEMON_RESET_ERRORINFO(*info);

	LemonCreateMutex(info);
}

struct LemonTlsDestructor
{
	LemonTls Tls;

	void (*Proc)(void*);
};

std::vector<LemonTlsDestructor> LemonTlsRegister;

void RegisterTls(LemonTls tls,void (*destructor)(void*),LemonErrorInfo * errorCode)
{
	LemonMutexLock(__globalMutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	LemonTlsDestructor d = {tls,destructor};

	LemonTlsRegister.push_back(d);

	LemonMutexUnLock(__globalMutex,errorCode);
}

void DeRegisterTls(LemonTls tls,LemonErrorInfo * errorCode)
{
	LemonMutexLock(__globalMutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	std::vector<LemonTlsDestructor>::iterator iter,end = LemonTlsRegister.end();

	for(iter = LemonTlsRegister.begin();iter != end; ++ iter)
	{
		if(tls == iter->Tls)
		{
			LemonTlsRegister.erase(iter);

			break;
		}
	}

	LemonMutexUnLock(__globalMutex,errorCode);
}


LONG WINAPI LemonWin32UnHandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	std::cout << "test" <<  std::endl;

	lemon_char_t buffer[MAX_PATH] = {0};

	GetModuleFileNameW(NULL,buffer,MAX_PATH);

	lemon::fs::path path(buffer);

	lemon::fs::path dumpFile = path.parent() / LEMON_TEXT("lemon.dmp");

	HANDLE lhDumpFile = CreateFileW
		(
		dumpFile.native().c_str(), 
		GENERIC_WRITE, 
		0,
		NULL,                                    
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL 
		,NULL
		); 

	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;

	int nDumpType = MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithFullMemory | 
		MiniDumpWithThreadInfo | MiniDumpWithHandleData | MiniDumpWithUnloadedModules |
		MiniDumpFilterModulePaths;

	 MiniDumpWriteDump
		 (
		 GetCurrentProcess(), 
		 GetCurrentProcessId(),
		 lhDumpFile, 
		 (MINIDUMP_TYPE)nDumpType, 
		 &loExceptionInfo, 
		 NULL, 
		 NULL
		 );

	 CloseHandle(lhDumpFile);

	 return EXCEPTION_EXECUTE_HANDLER;
}


BOOLEAN WINAPI DllMain( 
	IN HINSTANCE /*hDllHandle*/, 
	IN DWORD     nReason, 
	IN LPVOID    /*Reserved*/ )
{

	LEMON_DECLARE_ERRORINFO(errorinfo);

	switch (nReason)
	{
	case DLL_THREAD_DETACH:
		{
			LEMON_DECLARE_ERRORINFO(errorCode);

			LemonMutexLock(__globalMutex,&errorCode);

			if(LEMON_FAILED(errorCode)) return FALSE;

			std::vector<LemonTlsDestructor>::iterator iter,end = LemonTlsRegister.end();

			for(iter = LemonTlsRegister.begin();iter != end; ++ iter)
			{
				void * data = LemonTlsGet(iter->Tls,&errorCode);

				if(LEMON_FAILED(errorCode))
				{
					LemonMutexUnLock(__globalMutex,&errorCode);

					return FALSE;
				}

				if(data) iter->Proc(data);
			}

			LemonMutexUnLock(__globalMutex,&errorCode);

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			WSACleanup();

			ReleaseGlobalMutex(&errorinfo);

			break;
		}

	case DLL_PROCESS_ATTACH:
		{
			SetUnhandledExceptionFilter(LemonWin32UnHandledExceptionFilter); 

			CreateGlobalMutex(&errorinfo);

			if(LEMON_FAILED(errorinfo)) return FALSE;


			WSADATA wsaData;

			DWORD result;

			if(0 != (result = WSAStartup(MAKEWORD(2, 2), &wsaData))) return FALSE;

			break;
		}
	}

	return TRUE;
}

#endif //WIN32