#include <lemon/sys/ld.h>
#include <lemon/sys/errorcode.h>
#ifdef WIN32
LEMON_SYS_API 
	LemonDLL 
	LemonLoadLibrary(
	__lemon_in const lemon_char_t *path,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	HMODULE module = ::LoadLibraryW(path);

	if(NULL == module){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
		return LEMON_HANDLE_NULL_VALUE;
	}

	return (LemonDLL)module;
}

LEMON_SYS_API void LemonUnloadLibrary(
	__lemon_in LemonDLL library,
	__lemon_inout LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!FreeLibrary((HMODULE)library)){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

LEMON_SYS_API void * LemonGetProcAddress(
	__lemon_in LemonDLL library,
	__lemon_in const char * name,
	__lemon_inout LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	void* reuslt =  ::GetProcAddress((HMODULE)library,name);

	if(NULL == reuslt){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return reuslt;
}

#elif defined(LEMON_HAS_DLFCN_H)

#include <dlfcn.h>
#include <lemon/sys/text.h>


LEMON_SYS_API 
	LemonDLL 
	LemonLoadLibrary(
	__lemon_in const char *path,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	void * handle = dlopen(path, RTLD_NOW|RTLD_LOCAL);

	if(!handle)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_LD_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	return (LemonDLL)handle;
}

LEMON_SYS_API void LemonUnloadLibrary(
	__lemon_in LemonDLL library,
	__lemon_inout LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != dlclose((void*)library))
	{
	  printf("%s",dlerror());
	}
}

LEMON_SYS_API void * LemonGetProcAddress(
	__lemon_in LemonDLL library,
	__lemon_in const char * name,
	__lemon_inout LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	void * result = dlsym((void*)library,name);

	if(NULL == result){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_LD_GET_METHOD_ERROR);
	}

	return result;
}
#else
#	error "not support system"
#endif 
