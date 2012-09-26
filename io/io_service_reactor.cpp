#include <lemon/io/io_service_reactor.hpp>

#ifndef LEMON_IO_IOCP

namespace lemon{namespace io{namespace core{

	io_data::io_data(size_t type, int fd):_type(type),_fd(fd) {}

	io_data::io_data(size_t type, int fd,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
		:_type(type),_fd(fd),_userdata(userdata),_callback(callback),_buffer(buffer),_bufferSize(bufferSize)
	{

	}

	void io_data::reset(void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
	{
		_userdata = userdata; _callback = callback; _buffer = buffer; _bufferSize = bufferSize;
	}

	void io_data::call()
	{
		switch(_type)
		{
		case LEMON_REACTOR_READ:
			{
				ssize_t length = read(_fd, _buffer, _bufferSize);

				if(length == -1)
				{
					LEMON_POSIX_ERROR(_errorCode,errno);
				}
				else
				{
					_numberOfBytesTransferred = length;
				}

				break;
			}
		case LEMON_REACTOR_WRITE:
			{
				ssize_t length = write(_fd, _buffer, _bufferSize);

				if(length == -1)
				{
					LEMON_POSIX_ERROR(_errorCode,errno);
				}
				else
				{
					_numberOfBytesTransferred = length;
				}

				break;
			}
		case LEMON_REACTOR_SENDTO:
			{
				ssize_t length = read(_fd, _buffer, _bufferSize);

				if(length == -1)
				{
					LEMON_POSIX_ERROR(_errorCode,errno);
				}
				else
				{
					_numberOfBytesTransferred = length;
				}

				break;
			}
		case LEMON_REACTOR_RECVFROM:
			{
				ssize_t length = write(_fd, _buffer, _bufferSize);

				if(length == -1)
				{
					LEMON_POSIX_ERROR(_errorCode,errno);
				}
				else
				{
					_numberOfBytesTransferred = length;
				}

				break;
			}
		}

		_callback(_userdata,_numberOfBytesTransferred,&_errorCode);
	}

	//////////////////////////////////////////////////////////////////////////

	accept_io_data::accept_io_data
		(
		int fd,
		LemonAcceptCallback callback, 
		void * userdata, 
		sockaddr *address,
		socklen_t *addressSize
		)
		:io_data(LEMON_REACTOR_ACCEPT,fd)
		,_address(address)
		,_addresslen(addressSize)
	{
		reset(this,&accept_io_data::callback,NULL,0);
	}

	void accept_io_data::callback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode)
	{
		accept_io_data * self = (accept_io_data*)userData;

		//TODO: call accept function
	}

	//////////////////////////////////////////////////////////////////////////

	sendto_io_data::sendto_io_data
			(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			const sockaddr *address,
			socklen_t addressSize
			)
		:io_data(LEMON_REACTOR_SENDTO,fd,userdata,callback,buffer,bufferSize)
		,_address(address)
		,_addresslen(addressSize)
	{

	}

	///////////////////////////////////////////////////////////////////////////

	recvfrom_io_data::recvfrom_io_data
			(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			sockaddr *address,
			socklen_t *addressSize
			)
		:io_data(LEMON_REACTOR_RECVFROM,fd,userdata,callback,buffer,bufferSize)
		,_address(address)
		,_addresslen(addressSize)
	{

	}


	//////////////////////////////////////////////////////////////////////////
	io_service_reactor::io_service_reactor()
		:_completeQueue(LEMON_IO_REACTOR_SESSIONS),_exit(false)
	{

	}

	io_service_reactor::~io_service_reactor()
	{

	}

	void io_service_reactor::attach()
	{
		mutex_t::scope_lock lock(_mutex);

		while(!_exit)
		{
			while(!_exit && _completeQueue.empty()) _condition.wait(lock);

			if(!_exit)
			{
				io_data* iodata = (io_data*)_completeQueue.pop_front();

				iodata->call();

				switch(iodata->type())
				{
				case LEMON_REACTOR_ACCEPT:
					{
						free_accept_io_data(iodata);

						break;
					}
				case LEMON_REACTOR_SENDTO:
					{
						free_sendto_io_data(iodata);

						break;
					}
				case LEMON_REACTOR_RECVFROM:
					{
						free_recvfrom_io_data(iodata);

						break;
					}
				default:
					free_io_data(iodata);

					break;
				}
			}
		}
	}

	void io_service_reactor::detach()
	{
		mutex_t::scope_lock lock(_mutex);

		_exit = true;

		_condition.notifyall();
	}

	void io_service_reactor::post_one(LemonIOCallback callback,void * userdata, void * buffer, size_t bufferSize,LemonErrorInfo *errorCode)
	{
		mutex_t::scope_lock lock(_mutex);

		io_data * iodata = alloc_io_data(LEMON_REACTOR_POST_ONE,-1,userdata,callback,buffer,bufferSize);

		post_one(iodata,errorCode);
	}

	void io_service_reactor::post_one(io_data * iodata,LemonErrorInfo *errorCode)
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		bool used = false;

		io_data* replace = (io_data*)_completeQueue.push_back(used);

		if(used)
		{
			iodata->call();

			if(iodata->type() == LEMON_REACTOR_ACCEPT)
			{
				free_accept_io_data(iodata);
			}
			else
			{
				free_io_data(iodata);
			}
		}
	}

	io_data * io_service_reactor::alloc_io_data(size_t type,int fd,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
	{
		mutex_t::scope_lock lock(_iodataAllocatorMutex);

		return new(_iodataAllocator.alloc()) io_data(type,fd,userdata,callback,buffer,bufferSize);
	}

	void io_service_reactor::free_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_iodataAllocatorMutex);

		_iodataAllocator.free(iodata);
	}

	accept_io_data * io_service_reactor::alloc_io_data
		(
		int fd,
		LemonAcceptCallback callback, 
		void * userdata, 
		sockaddr *address,
		socklen_t *addressSize
		)
	{
		mutex_t::scope_lock lock(_acceptIODataAllocatorMutex);

		return new(_acceptIODataAllocator.alloc()) accept_io_data(fd,callback,userdata,address,addressSize);
	}

	void io_service_reactor::free_accept_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_acceptIODataAllocatorMutex);

		_acceptIODataAllocator.free(iodata);
	}

	sendto_io_data * io_service_reactor::alloc_sendto_io_data(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			const sockaddr *address,
			socklen_t addressSize)
	{
		mutex_t::scope_lock lock(_sendtoIODataAllocatorMutex);

		return new(_sendtoIODataAllocator.alloc()) sendto_io_data(fd,userdata,callback,buffer, bufferSize,address,addressSize);
	}

	void io_service_reactor::free_sendto_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_sendtoIODataAllocatorMutex);

		_sendtoIODataAllocator.free(iodata);
	}

	recvfrom_io_data * io_service_reactor::alloc_recvfrom_io_data(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			sockaddr *address,
			socklen_t *addressSize)
	{
		mutex_t::scope_lock lock(_recvfromIODataAllocatorMutex);

		return new(_recvfromIODataAllocator.alloc()) sendto_io_data(fd,userdata,callback,buffer, bufferSize,address,addressSize);
	}

	void io_service_reactor::free_recvfrom_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_recvfromIODataAllocatorMutex);

		_recvfromIODataAllocator.free(iodata);
	}
}}}

#endif //
