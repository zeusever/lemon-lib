#include <lemon/io/resolver.h>

LEMON_IMPLEMENT_HANDLE(LemonResovler){
	addrinfo * handle;

	addrinfo * current;
};

LEMON_IO_API 
	LemonResovler 
	LemonCreateResolver(
	__lemon_in const char *nodeName,
	__lemon_in const char *serverName,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonResovler,resolver);

	int ec = ::getaddrinfo(nodeName,serverName,NULL,&resolver->handle);

	if(0 != ec){
#ifdef WIN32	     
		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
#else
		LEMON_POSIX_ERROR(*errorCode,ec);
#endif //UNIX
		
		LEMON_FREE_HANDLE(resolver);

		return LEMON_HANDLE_NULL_VALUE;
	}

	return resolver;
}

LEMON_IO_API
	void LemonReleaseResolver(__lemon_free LemonResovler resolver)
{
	freeaddrinfo(resolver->handle);

	LEMON_FREE_HANDLE(resolver);
}

LEMON_IO_API
	const addrinfo * LemonResovlerNext(__lemon_in LemonResovler resolver,__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 == resolver->current){

		resolver->current = resolver->handle;

		
	}else{

		resolver->current = resolver->current->ai_next;
	}

	return resolver->current;
}
