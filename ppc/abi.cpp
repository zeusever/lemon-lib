#include <lemon/ppc/abi.h>
#include <lemon/sys/errorcode.h>

LEMON_PPC_API
	void LemonCXXPreprocess(
	__lemon_in const lemon_char_t * /*source*/,
	__lemon_in const lemon_char_t * /*target*/,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_USER_ERROR(*errorCode,LEMON_SYS_NOT_IMPLEMENT);
}