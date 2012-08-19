/**
* 
* @file     message
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/11
*/
#ifndef LEMON_DIAGNOSIS_DTRACE_MESSAGE_HPP
#define LEMON_DIAGNOSIS_DTRACE_MESSAGE_HPP
#include <lemon/diagnosis/dtrace/dtrace.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	class LocalMessage : public IMessage
	{
	public:
		
		LocalMessage(size_t blockSize);

		~LocalMessage();

		void Reset(const LemonUuid * provider,LemonDTraceFlags flags);

	public:

		virtual size_t Write( const lemon::byte_t * data , size_t length );

		virtual size_t Read( lemon::byte_t * data , size_t length );

		virtual lemon::byte_t Peek();

		virtual const LemonUuid * Provider();

		virtual LemonDTraceFlags Flags();

		virtual size_t Length() const;

		virtual size_t Offset( size_t status,int offset );

		virtual size_t Dump(lemon::byte_t * data,size_t length,error_info & errorCode);

		virtual void Load(const lemon::byte_t * data,size_t length,error_info & errorCode);

		size_t Load(LemonIoReader reader);

		const lemon::time_t &TimeStamp();

		void Dump(LemonIoWriter writer);

	private:

		size_t						_blockSize;

		int							_offset;

		size_t						_length;

		lemon::byte_t				*_buffer;

		lemon::uuid_t				_provider;

		lemon::time_t				_timestamp;

		LemonDTraceFlags			_flags;
	};

}}}

#endif // LEMON_DIAGNOSIS_DTRACE_MESSAGE_HPP

