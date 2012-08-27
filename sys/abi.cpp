#include <lemon/sys/abi.h>    


LemonUuid LEMON_WIN32_ERROR_CATALOG = { 0x3eb0937e, 0xdbe9, 0x4946, { 0xa0, 0xb4, 0x44, 0xc4, 0x19, 0xe9, 0x46, 0xf7 } };

LemonUuid LEMON_COM_ERROR_CATALOG = { 0x3eb0937e, 0xdbe9, 0x4946, { 0xa0, 0xb4, 0x44, 0xc4, 0x19, 0xe9, 0x46, 0xf2 } };

LemonUuid LEMON_POSIX_ERROR_CATALOG = { 0x11a7987e, 0xa950, 0x434c, { 0xa4, 0xbb, 0x76, 0xdf, 0x4d, 0x42, 0xa2, 0xe1 } };

LemonUuid LEMON_UNITTEST_ERROR_CATALOG = { 0x11a7987e, 0xa950, 0x434c, { 0xa4, 0xbb, 0x76, 0xd1, 0x4d, 0x42, 0xa2, 0xe2 } };


LEMON_SYS_API 
	lemon_bool 
	LemonErrorCodeCompare(
	const LemonError * lhs,
	const LemonError * rhs)
{
	if(lhs->Catalog == lhs->Catalog)
	{
		if(lhs->Catalog == 0) return lemon_true;

		if(memcmp(lhs->Catalog,rhs->Catalog,sizeof(LemonUuid)) == 0 && lhs->Code == rhs->Code) return lemon_true;
	}

	return lemon_false;
}

#ifdef WIN32

LEMON_SYS_API lemon_pid_t LemonGetProcessID()
{
	return GetCurrentProcessId();
}

#else

#include <unistd.h>

LEMON_SYS_API lemon_pid_t LemonGetProcessID()
{
	return getpid();
}

#endif //WIN32