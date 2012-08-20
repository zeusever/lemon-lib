#include <lemon/sys/assembly.h>
#include <lemonxx/io/resolver.hpp>
#include <lemon/dtrace/object.hpp>
#include <lemon/dtrace/local/service.hpp>
#include <lemon/dtrace/local/listener.hpp>

namespace lemon{namespace dtrace{

	IService * Create(const char * url)
	{
		std::string val = string_toupper(url);

		if(val == "LOCAL")
		{
			return new Service();
		}
		else if(val.find("LISTEN"))
		{
			val = val.substr(val.find_first_not_of("://"));

			size_t pos = val.find(":");

			return new Listener(io::resolver_iterator(val.substr(0,pos),val.substr(pos + 1))->ai_addr);
		}
		
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}
}}