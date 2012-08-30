/**
 * 
 * @file     message
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/25
 */
#ifndef LEMON_TRACE_MESSAGE_HPP
#define LEMON_TRACE_MESSAGE_HPP
#include <lemon/trace/abi.h>
#include <lemonxx/io/io.hpp>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace trace{

	class Message : private lemon::nocopyable
	{
	public:

		Message( );
		
		Message( const LemonUuid * uuid );

		~Message( );

		void Reset( lemon_trace_flag flag );

		void Reset( const LemonUuid * uuid , lemon_trace_flag flag );

		size_t Read( void * data, size_t length );

		size_t Write( const void * data, size_t length );

		size_t Seek( int offset , io::seek::Value position );

		void Description( LemonTraceDescription * description ) const;

		size_t Dump( io::writer &writer ) const;

		size_t Load( io::reader &reader );

	private:

		LemonTraceNodeName		_node;

		lemon_pid_t				_processid;

		lemon_tid_t				_threadId;

		lemon_trace_flag		_flag;

		lemon::uuid_t			_uuid;

		lemon_uint32_t			_length;

		size_t					_offset;

		byte_t					_buffer[LEMON_TRACE_MESSAGE_MAX_LENGTH];
	};

}}

#endif // LEMON_TRACE_MESSAGE_HPP
