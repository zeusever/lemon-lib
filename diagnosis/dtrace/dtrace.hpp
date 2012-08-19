/**
* 
* @file     dtrace
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/11
*/
#ifndef LEMON_DIAGNOSIS_DTRACE_DTRACE_HPP
#define LEMON_DIAGNOSIS_DTRACE_DTRACE_HPP

#include <lemon/diagnosis/abi.h>
#include <lemon/diagnosis/assembly.h>

#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	struct IService;

	struct IProvider;

	struct IController;

	struct IConsumer;

	struct IMessage;

	struct IService : private lemon::nocopyable
	{
		virtual ~ IService() {}

		virtual IProvider *  CreateProvider(const LemonUuid * id) = 0 ;

		virtual IController * CreateController()  = 0 ;

		virtual void CloseProvider(IProvider * provider) = 0 ;

		virtual void CloseController(IController * controller)  = 0 ;

		virtual void CommitTrace(IMessage * message)  = 0 ;

		virtual void TraceFlagsChange(IController * sender,const LemonUuid * provider) = 0;
	};

	struct IProvider :  private lemon::nocopyable
	{
		void Release() { Service()->CloseProvider(this); }

		IService *  Service() { return _service; }

		virtual IMessage * Trace(LemonDTraceFlags flags) = 0;

		virtual void CommitTrace(IMessage * message) = 0;

		virtual void TraceFlagsChange(LemonDTraceFlags flags) = 0;

		virtual const LemonUuid* Id() const = 0;

	protected:

		IProvider(IService *  service):_service(service) {  }

		~ IProvider() {}

	private:

		IService			*_service;
	};

	struct IMessage : private lemon::nocopyable
	{
		template<typename ConstBuffer>
		size_t Write(ConstBuffer buffer)
		{
			return Write(buffer.Data,buffer.Length);
		}

		template<typename Buffer>
		size_t Read(Buffer buffer)
		{
			return Read(buffer.Data,buffer.Length);
		}

		virtual size_t Dump(lemon::byte_t * data,size_t length,error_info & errorCode) = 0;

		virtual void Load(const lemon::byte_t * data,size_t length,error_info & errorCode) = 0;

		virtual void Dump(LemonIoWriter writer) = 0;

		virtual size_t Write(const lemon::byte_t *  data , size_t length) = 0 ;

		virtual size_t Read(lemon::byte_t *  data , size_t length) = 0 ;

		virtual lemon::byte_t Peek() = 0 ;

		virtual const LemonUuid * Provider() = 0;

		virtual LemonDTraceFlags Flags()  = 0;

		virtual size_t Length() const = 0;

		virtual size_t Offset(size_t status,int offset) = 0;

		virtual const lemon::time_t &TimeStamp() = 0;
	protected:

		~IMessage() {}
	};

	struct IController : private lemon::nocopyable
	{
		void Release() { Service()->CloseController(this); }

		IService *  Service() { return _service; }

		virtual IConsumer * CreateConsumer(void *userData, LemonDTraceConsumerCallback callback)  = 0 ;

		virtual void CloseConsumer(IConsumer * provider) = 0 ;

		virtual void Trace(IMessage * message) = 0;

		virtual void OpenTrace(const LemonUuid * provider , LemonDTraceFlags flags) = 0;

		virtual void CloseTrace(const LemonUuid * provider , LemonDTraceFlags flags) = 0;

		virtual LemonDTraceFlags Flags(const LemonUuid * provider) const = 0;

	protected:

		IController(IService *  service):_service(service) {  }

		~IController(){}

	private:

		IService			*_service;
	};

	struct IConsumer : private lemon::nocopyable
	{
		void Release() { Controller()->CloseConsumer(this); }

		IController *  Controller() { return _controller; }

		virtual void CommitTrace(IMessage * message)  = 0 ;

	protected:

		IConsumer(IController *  service):_controller(service) {  }

	private:

		IController			*_controller;
	};
}}}


#endif // LEMON_DIAGNOSIS_DTRACE_DTRACE_HPP

