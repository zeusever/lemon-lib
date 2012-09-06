/**
* 
* @file     iodata
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/31
*/
#ifndef LEMON_IO_EPOLL_IODATA_HPP
#define LEMON_IO_EPOLL_IODATA_HPP
#include <lemon/io/io_service.hpp>

namespace lemon{namespace io{

	template<typename IOService>
	struct IODataT
	{
		typedef IODataT<IOService>	Self;

		typedef void (IOService::*IODataRelease)( Self * self );

		IODataRelease				Release;

		void						*UserData;

		LemonIOCallback				Callback;

		void						*Buffer;

		size_t						BufferSize;

		IODataT(void * userData, LemonIOCallback callback, void * buffer, size_t bufferSize,IODataRelease release)
			:UserData(userData),
			Callback(callback),
			Buffer(buffer),
			BufferSize(bufferSize),
			Release(release)
		{

		}
	};

	template<class IOService>
	inline void Release(IOService * service,IODataT<IOService> * iodata)
	{
		(service->*(iodata->Release))(iodata);
	}

}}

#endif //LEMON_IO_EPOLL_IODATA_HPP