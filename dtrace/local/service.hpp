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
#include <map>
#include <set>
#include <lemon/dtrace/object.hpp>

namespace lemon{namespace dtrace{

	class Service : public IService
	{
	public:
		
		typedef std::map<lemon::uuid_t,IProvider*>	Providers;

		typedef std::set<IController*>			Controllers;

		IProvider * CreateProvider(const LemonUuid * provider);

		void CloseProvider(IProvider * provider);

		IController * CreateController();

		void CloseController(IController * controller);

	public:

		void OnStatusChanged(const lemon::uuid_t & id)
		{
			lemon::mutex_t::scope_lock lock(_controllerMutex);

			OnStatusChangedNoLocked(id);
		}

	private:

		void OnStatusChangedNoLocked(const lemon::uuid_t & id);

	private:

		Providers						_providers;

		Controllers						_controllers;

		lemon::mutex_t					_providerMutex;

		lemon::mutex_t					_controllerMutex;
	};

}}

#endif //LEMON_DTRACE_SERVICE_HPP