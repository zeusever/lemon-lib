#include <lemon/io/io_service_reactor.hpp>

#ifndef LEMON_IO_IOCP

namespace lemon{namespace io{namespace core{

	io_data::io_data(size_t type,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
		:_type(type),_userdata(userdata),_callback(callback),_buffer(buffer),_bufferSize(bufferSize)
	{

	}

	void io_data::reset(void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
	{
		_userdata = userdata; _callback = callback; _buffer = buffer; _bufferSize = bufferSize;
	}

	void io_data::call()
	{
		_callback(_userdata,_numberOfBytesTransferred,&_errorCode);
	}

	//////////////////////////////////////////////////////////////////////////

	accept_io_data::accept_io_data
		(
		socket * listen, 
		socket *peer, 
		LemonAcceptCallback callback, 
		void * userdata, 
		sockaddr *address,
		socklen_t *addressSize
		)
		:io_data(LEMON_REACTOR_ACCEPT)
	{
		reset(this,&accept_io_data::callback,NULL,0);
	}

	void accept_io_data::callback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode)
	{
		accept_io_data * self = (accept_io_data*)userData;

		if(LEMON_SUCCESS(*errorCode))
		{
			self->_callback(userData,reinterpret_cast<LemonIO>(self->_peer),numberOfBytesTransferred,errorCode);
		}
		else
		{
			//TODO: release the socket
			self->_callback(userData,NULL,numberOfBytesTransferred,errorCode);
		}
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

		io_data * iodata = alloc_io_data(LEMON_REACTOR_POST_ONE,userdata,callback,buffer,bufferSize);

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

	io_data * io_service_reactor::alloc_io_data(size_t type,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
	{
		mutex_t::scope_lock lock(_iodataAllocatorMutex);

		return new(_iodataAllocator.alloc()) io_data(type,userdata,callback,buffer,bufferSize);
	}

	void io_service_reactor::free_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_iodataAllocatorMutex);

		_iodataAllocator.free(iodata);
	}

	accept_io_data * io_service_reactor::alloc_io_data
		(
		socket * listen, 
		socket *peer, 
		LemonAcceptCallback callback, 
		void * userdata, 
		sockaddr *address,
		socklen_t *addressSize
		)
	{
		mutex_t::scope_lock lock(_acceptIODataAllocatorMutex);

		return new(_acceptIODataAllocator.alloc()) accept_io_data(listen,peer,callback,userdata,address,addressSize);
	}

	void io_service_reactor::free_accept_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_acceptIODataAllocatorMutex);

		_acceptIODataAllocator.free(iodata);
	}
}}}

#endif //