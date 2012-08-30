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

	IService * CreateTraceService(const char * /*url*/)
	{
		//TODO : parse the url ,create the real service
		return new Service();
	}
}}