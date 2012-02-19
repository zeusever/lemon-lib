#include <errno.h>
#include <lemon/io/buildin/inet_ntop.h>
#include <lemon/sys/text.h>

#ifndef LEMON_HAS_INET_NTOP

LEMON_IO_PRIVATE const char *inet_ntop_v4(const void *src, char *dst, socklen_t size)
{
	lemon_byte_t * bytes = (lemon_byte_t * )src;

	char buffer[sizeof("255.255.255.255")];

	memset(buffer,0,sizeof(buffer));

	int length = lemon_sprintf(buffer,sizeof(buffer),"%d.%d.%d.%d",bytes[0],bytes[1],bytes[2],bytes[3]) + 1;

	if(length > size){
		errno = ENOSPC;

		return NULL;
	}

	memcpy(dst,buffer,length);

	return dst;
}
#ifdef LEMON_SUPPORT_IPV6
LEMON_IO_PRIVATE const char *inet_ntop_v6(const void *src, char*dst, socklen_t size)
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

		length = lemon_sprintf(buffer,sizeof(buffer),"::ffff:");

		if(!inet_ntop_v4(&bytes[12],&buffer[length],sizeof(buffer) - length)) return NULL;

		length = (int)strlen(buffer) + 1;

	}else if(better.from == 0 && better.length == 6){

		length = lemon_sprintf(buffer,sizeof(buffer),"::");

		if(!inet_ntop_v4(&bytes[12],&buffer[length],sizeof(buffer) - length)) return NULL;

		length = (int)strlen(buffer) + 1;
	}else{

		for(int i = 0 ; i < 8 ; ++ i){

			if(better.from == i){

				if(i == 0) length += lemon_sprintf(&buffer[length],sizeof(buffer) - length,"::");

				else length += lemon_sprintf(&buffer[length],sizeof(buffer) - length,":");

				continue;
			}
			else if(i - better.length < better.from && better.from < i) continue;

			if(i != 0) length += lemon_sprintf(&buffer[length],sizeof(buffer) - length,":");

			length += lemon_sprintf(&buffer[length],sizeof(buffer) - length,"%x",words[i]);
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

LEMON_IO_PRIVATE const char *inet_ntop(int af, const void *src, char*dst, socklen_t size)
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

#endif //LEMON_HAS_INET_NTOP