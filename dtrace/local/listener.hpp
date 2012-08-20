/**
* 
* @file     listener
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/20
*/
#ifndef LEMON_DTRACE_LOCAL_LISTENER_HPP
#define LEMON_DTRACE_LOCAL_LISTENER_HPP

#include <lemonxx/io/endpoint.hpp>
#include <lemon/dtrace/local/service.hpp>


namespace lemon{namespace dtrace{

	class Listener : public IService
	{
	public:

		Listener(const lemon::io::ip::endpoint & ep)
		{
			_ep = ep;
		}
		
		IProvider * CreateProvider(const LemonUuid * provider);

		void CloseProvider(IProvider * provider);

		IController * CreateController();

		void CloseController(IController * controller);

	private:

		lemon::io::ip::endpoint		_ep;
	};

}}


#endif //LEMON_DTRACE_LOCAL_LISTENER_HPP