#include <lemon/sys/assembly.h>
#include <lemon/sys/environment.h>

#ifdef WIN32

LEMON_SYS_API
	size_t LemonGetEnv(
	__lemon_in const lemon_char_t * varname,
	__lemon_inout lemon_char_t * buffer __lemon_buffer(length),
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t result = 0;

	LEMON_RESET_ERRORINFO(*errorCode);

	errno_t eno = _wgetenv_s(&result,buffer,length,varname);

	if(0 != eno) LEMON_POSIX_ERROR(*errorCode,eno);

	return result;
}

LEMON_SYS_API
	void LemonSetEnv(
	__lemon_in const lemon_char_t * varname,
	__lemon_in const lemon_char_t * value,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	errno_t eno = _wputenv_s(varname,value);

	if(0 != eno) LEMON_POSIX_ERROR(*errorCode,eno);
}


#else

#include <stdlib.h>

LEMON_SYS_API
	size_t LemonGetEnv(
	__lemon_in const lemon_char_t * varname,
	__lemon_inout lemon_char_t * buffer __lemon_buffer(length),
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_char_t *val = getenv(varname);

	if(NULL == val){
	
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_NOT_FOUND_ENV);

		return 0;
	}

	size_t result = strlen(val);

	if(result + 1> length){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return result;
	}

	memcpy(buffer,val,result + 1);

	return result;
}

LEMON_SYS_API
	void LemonSetEnv(
	__lemon_in const lemon_char_t * varname,
	__lemon_in const lemon_char_t * value,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(-1 == setenv(varname,value,1)){

		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

#endif //WIN32