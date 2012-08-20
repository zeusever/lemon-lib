/**
* 
* @file     service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/20
*/
#ifndef LEMON_DTRACE_SERVICE_HPP
#define LEMON_DTRACE_SERVICE_HPP

#include <lemon/dtrace/object.hpp>

namespace lemon{namespace dtrace{

	class Service : public IService
	{
	public:

		IProvider * CreateProvider(const LemonUuid * provider);

		void CloseProvider(IProvider * provider);

		IController * CreateController();

		void CloseController(IController * controller);
	};

}}

#endif //LEMON_DTRACE_SERVICE_HPP