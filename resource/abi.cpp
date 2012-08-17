#include <lemon/resource/abi.h>

LEMON_RESOURCE_API 
	const lemon_char_t * 
	LemonI18nText(
	__lemon_in const LemonUuid* /*id*/,
	__lemon_in const lemon_char_t * msg)
{
	return msg;
}