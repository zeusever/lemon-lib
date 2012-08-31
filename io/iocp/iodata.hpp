/**
* 
* @file     iodata
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/31
*/
#ifndef LEMON_IO_IOCP_IODATA_HPP
#define LEMON_IO_IOCP_IODATA_HPP
#include <lemon/io/io_service.hpp>

#ifdef LEMON_IO_IOCP

namespace lemon{namespace io{

	template<class IOService>
	struct IODataT
	{
		typedef IODataT<IOService>	Self;

		typedef void (IOService::*IODataRelease)( Self * self );

		OVERLAPPED					Overlapped;

		void						*UserData;

		LemonIOCallback				Callback;

		WSABUF						Buffer;

		IODataRelease				Release;

		IODataT(){}

		IODataT(void * userData, LemonIOCallback callback, WSABUF buffer, IODataRelease release)
		{
			UserData = userData;

			Callback = callback;

			Buffer = buffer;

			Release = release;
		}
	};

	template<class IOService>
	inline void Release(IOService * service,IODataT<IOService> * iodata)
	{
		(service->*(iodata->Release))(iodata);
	}
}}

#endif // LEMON_IO_IOCP

#endif //LEMON_IO_IOCP_IODATA_HPP