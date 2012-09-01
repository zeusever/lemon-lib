#include <lemon/trace/assembly.h>
#include <lemon/trace/object.hpp>
#include <lemon/trace/local/service.hpp>

namespace lemon{namespace trace{

	Consumer::Consumer(IController * controller,LemonTraceProc proc, void * userdata)
		:_controller(controller),_proc(proc),_userdata(userdata)
	{
		_controller->Register(this);
	}

	Consumer::~Consumer()
	{
		_controller->UnRegister(this);
	}

	void Consumer::OnMessage(const Message * message)
	{
		_proc((LemonTraceMessage)(message),_userdata);
	}

	IService * CreateTraceService(const char * url)
	{
		if(NULL == url) return new Service();

		std::string URL = string_tolower<char>(url);

		std::string::size_type position = URL.find("TCP://");

		std::string::size_type start = 6;

		if(URL.npos == position) 
		{
			position = URL.find("UDP://");

			if(URL.npos == position) 
			{
				//TODO: add the pipe protocol

				scope_error_info errorCode;

				LEMON_USER_ERROR(errorCode,LEMON_TRACE_INVALID_URL);
			}

			start = 6;
		}

		position = URL.find(":",start);

		if(URL.npos ==position)
		{
			scope_error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_TRACE_INVALID_URL);
		}

		std::string nodename = URL.substr(start,position - start);

		std::string servername = URL.substr(position + 1);

		net::resolver_iterator iter(nodename,servername);

		if(net::resolver_iterator() == iter)
		{
			scope_error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_TRACE_INVALID_URL);
		}

		return NULL;
	}
}}