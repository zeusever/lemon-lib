#include <lemon/mq/url.h>
#include <lemon/sys/text.h>
#include <lemon/mq/errorcode.h>
#include <lemon/sys/errorcode.h>

typedef struct UrlType{
	
	const char *	Name;

	size_t			Value;

}UrlType;

UrlType __urlTypes[] = 
{
	{"tcp",LEMON_MQ_URL_TCP},{"udp",LEMON_MQ_URL_UDP},{"ipc",LEMON_MQ_URL_IPC},{"memory",LEMON_MQ_URL_MEMORY}
};

LEMON_MQ_PRIVATE
	void 
	LemonParseMQUrl(
	__lemon_in const char * url,
	__lemon_inout LemonMQUrl *target,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t length = strlen(url);

	const char * begin = url;

	const char * iter,*end = url + length;

	for(iter = url; iter != end; ++ iter){

		if(':' == *iter){
			break;
		}
	}

	if(iter == end){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL); return;
	}

	UrlType *urlIter, *urlEnd = __urlTypes + sizeof(__urlTypes)/sizeof(UrlType);

	for(urlIter = __urlTypes; urlIter != urlEnd; ++ urlIter){
		
		size_t lhs = strlen(urlIter->Name);

		size_t rhs = iter - begin;

		if(lhs != rhs) continue;

		if(memcmp(urlIter->Name,begin,lhs) != 0) continue;

		target->Type = urlIter->Value;

		break;
	}

	if(urlIter != urlEnd){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL); return;
	}

	begin = iter;

	iter += 2;

	if(iter >= end){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL); return;
	}

	if(memcmp(begin,"//",2) != 0){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL); return;
	}

	begin = iter;

	for(; iter != end; ++ iter){

		if(':' == *iter){
			break;
		}
	}

	if(iter == end){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL); return;
	}

	if(iter - begin > sizeof(target->NodeName)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return;
	}

	memcpy(target->NodeName,begin,iter - begin);

	++ iter;

	if(iter == end){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL); return;
	}

	if(end - iter  > sizeof(target->NodeName)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return;
	}

	memcpy(target->ServerName,iter,end - iter);
}


LEMON_MQ_PRIVATE
	size_t 
	LemonMQUrlToString(
	__lemon_in const LemonMQUrl *source,
	__lemon_inout char * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * errorCode)
{
	const UrlType * iter = __urlTypes;

	const UrlType * end = __urlTypes + sizeof(__urlTypes)/sizeof(UrlType);

	for(; iter != end; ++ iter){
		if(iter->Value == source->Type) break;
	}

	if(iter == end){
		LEMON_USER_ERROR(*errorCode,LEMON_MQ_INVALID_URL_TYPE); return (size_t)-1;
	}

	return lemon_sprintf(target,targetLength,"%s://%s:%s",iter->Name,source->NodeName,source->ServerName);
}