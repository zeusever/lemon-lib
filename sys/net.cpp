#include <lemon/sys/net.h>
#include <lemon/sys/text.h>
#include <lemon/sys/assembly.h>


#ifndef LEMON_HAS_NTOP

LEMON_SYS_PRIVATE const char *inet_ntop_v4(const void *src, char *dst, socklen_t size)
{
	lemon_byte_t * bytes = (lemon_byte_t * )src;

	char buffer[sizeof("255.255.255.255")];

	memset(buffer,0,sizeof(buffer));

	int length = lemon_csprintf(buffer,sizeof(buffer),"%d.%d.%d.%d",bytes[0],bytes[1],bytes[2],bytes[3]) + 1;

	if(length > size){
		errno = ENOSPC;

		return NULL;
	}

	memcpy(dst,buffer,length);

	return dst;
}
#ifdef LEMON_SUPPORT_IPV6
LEMON_SYS_PRIVATE const char *inet_ntop_v6(const void *src, char*dst, socklen_t size)
{
	char buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];

	memset(buffer,0,sizeof(buffer));

	struct {int from;int length;} cur,better;

	lemon_byte_t *bytes = (lemon_byte_t*)src;

	int words[8];

	cur.from = -1;

	cur.length = -1;

	better = cur;

	for(size_t i = 0; i < 16; i += 2){

		words[i / 2] = (bytes[i] << 8) + bytes[i + 1];
	}

	//compute compress length;

	for(int i = 0; i < 8; ++ i){

		if(0 == words[i]){

			if(-1 == cur.from){
				cur.from = i;

				cur.length = 1;
			}
			else{

				++ cur.length;
			}

			continue;
		}

		if(-1 != cur.length){

			if(cur.length > better.length){
				better = cur;
			}

			cur.from = cur.length = -1;
		}
	}

	int length = 0;

	if(words[5] == 0xffff && better.from == 0 && better.length == 5){

		length = lemon_csprintf(buffer,sizeof(buffer),"::ffff:");

		if(!inet_ntop_v4(&bytes[12],&buffer[length],sizeof(buffer) - length)) return NULL;

		length = (int)strlen(buffer) + 1;

	}else if(better.from == 0 && better.length == 6){

		length = lemon_csprintf(buffer,sizeof(buffer),"::");

		if(!inet_ntop_v4(&bytes[12],&buffer[length],sizeof(buffer) - length)) return NULL;

		length = (int)strlen(buffer) + 1;
	}else{

		for(int i = 0 ; i < 8 ; ++ i){

			if(better.from == i){

				if(i == 0) length += lemon_csprintf(&buffer[length],sizeof(buffer) - length,"::");

				else length += lemon_csprintf(&buffer[length],sizeof(buffer) - length,":");

				continue;
			}
			else if(i - better.length < better.from && better.from < i) continue;

			if(i != 0) length += lemon_csprintf(&buffer[length],sizeof(buffer) - length,":");

			length += lemon_csprintf(&buffer[length],sizeof(buffer) - length,"%x",words[i]);
		}

		++ length;
	}

	if(length > size){
		errno = ENOSPC;

		return NULL;
	}

	memcpy(dst,buffer,length);

	return dst;
}
#endif //LEMON_SUPPORT_IPV6

LEMON_SYS_PRIVATE const char *inet_ntop(int af, const void *src, char*dst, socklen_t size)
{
	switch(af)
	{
	case AF_INET:
		{
			return inet_ntop_v4(src,dst,size);
		}
#ifdef LEMON_SUPPORT_IPV6
	case AF_INET6:
		{
			return inet_ntop_v6(src,dst,size);
		}
#endif //LEMON_SUPPORT_IPV6
	default:
		{
			errno = EAFNOSUPPORT;

			return NULL;
		}
	}
}

#endif 

#ifndef LEMON_HAS_PTON

LEMON_SYS_PRIVATE int inet_pton_v4(const char *src, void *dst)
{
	static const char digits[] = "0123456789";

	lemon_byte_t bytes[sizeof(in_addr)] = {0};

	lemon_byte_t * val = bytes;

	int index = 0;

	bool status = false;

	for(char iter = *src; iter != 0; iter = * (++src)){

		const char *found = strchr(digits,iter);

		if(index == 4) goto Error;

		if(found){
			int digit = (*val) * 10 + (int)(found - digits);

			if(digit > 255) goto Error;

			status = true;

			*val = (lemon_byte_t)digit;

		}else if(iter == '.' && status){

			*++val = 0;

			status = false;

			++ index;

		}else{

			goto Error;
		}
	}

	memcpy(dst,bytes,sizeof(bytes));

	return 1;

Error:
	errno = EAFNOSUPPORT;

	return 0;
}
#ifdef LEMON_SUPPORT_IPV6



LEMON_SYS_PRIVATE int inet_pton_v6(const char *src, void *dst)
{
	static const char xdigits_l[] = "0123456789abcdef",
		xdigits_u[] = "0123456789ABCDEF";
	u_char tmp[sizeof(in_addr6)], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	u_int val;

	memset((tp = tmp), '\0', sizeof(in_addr6));
	endp = tp + sizeof(in_addr6);
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) {
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!saw_xdigit) {
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			} else if (*src == '\0') {
				return (0);
			}
			if (tp + sizeof(short) > endp)
				return (0);
			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + sizeof(in_addr)) <= endp) &&
			inet_pton_v4(curtok, tp) > 0) {
				tp += sizeof(in_addr);
				saw_xdigit = 0;
				break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) {
		if (tp + sizeof(short) > endp)
			return (0);
		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		* Since some memmove()'s erroneously fail to handle
		* overlapping regions, we'll do the shift by hand.
		*/
		const int n = (int)(tp - colonp);
		int i;

		if (tp == endp)
			return (0);
		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	memcpy(dst, tmp, sizeof(in_addr6));
	return (1);

}
#endif //#ifdef LEMON_SUPPORT_IPV6
LEMON_SYS_PRIVATE int inet_pton(int af, const char *src, void *dst)
{
	switch(af)
	{
	case AF_INET:
		{
			return inet_pton_v4(src,dst);
		}
#ifdef LEMON_SUPPORT_IPV6
	case AF_INET6:
		{
			return inet_pton_v6(src,dst);
		}
#endif //LEMON_SUPPORT_IPV6
	default:
		{
			errno = EAFNOSUPPORT;

			return -1;
		}
	};
}

#endif //LEMON_HAS_PTON


LEMON_SYS_API size_t LemonAddressToString(
	__lemon_in int af,
	__lemon_in const void * source __lemon_buffer(sourceSize),
	__lemon_in size_t /*sourceSize*/,
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(AF_INET6 != af && AF_INET != af){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_NOT_SUPPORT_ADDRESS_FAMILY);

		return 0;
	}

	if(NULL == inet_ntop(af,source,buffer,(socklen_t)bufferSize)){
		LEMON_POSIX_ERROR(*errorCode,errno);

		return 0;
	}

	return strlen(buffer);
}

LEMON_SYS_API void LemonAddressFromString(
	__lemon_in int af,
	__lemon_in const char * source __lemon_buffer(sourceSize),
	__lemon_in  size_t /*sourceSize*/,
	__lemon_inout void * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(AF_INET6 != af && AF_INET != af){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_NOT_SUPPORT_ADDRESS_FAMILY);

		return;
	}

	if(AF_INET == af && bufferSize < sizeof(in_addr)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		return;
	}

	if(AF_INET6 == af && bufferSize < sizeof(in6_addr)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		return;
	}

	if(1 != inet_pton(af,source,buffer)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_INVALID_ADDRESS_STRING);
	}
}



LEMON_IMPLEMENT_HANDLE(LemonResovler){
	addrinfo * handle;

	addrinfo * current;
};

LEMON_SYS_API 
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

LEMON_SYS_API
	void LemonReleaseResolver(__lemon_free LemonResovler resolver)
{
	freeaddrinfo(resolver->handle);

	LEMON_FREE_HANDLE(resolver);
}

LEMON_SYS_API
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