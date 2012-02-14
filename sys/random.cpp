#include <lemon/sys/random.h>
#include <lemon/sys/errorcode.h>

#ifdef LEMON_RANDOM_WINCRYPT
#include <Wincrypt.h>
#pragma comment(lib, "advapi32.lib")

LEMON_SYS_API LemonRandomDevice LemonCreateRandomDevice(LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	const char * providerName = MS_DEF_PROV_A;

	char buffer[128] = {0};

	DWORD type;

	DWORD length;

	for(DWORD i = 0; ; ++ i)
	{
		length = sizeof(buffer);

		if(!::CryptEnumProvidersA(i,NULL,0,&type,buffer,&length))
		{
			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			return NULL;
		}

		if(strcmp(buffer,providerName) == 0) break;
	}

	HCRYPTPROV handle;

	if(!CryptAcquireContextA(&handle, NULL, providerName, type, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) 
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return NULL;
	}

	return (LemonRandomDevice)handle;
}

LEMON_SYS_API void LemonReleaseRandomDevice(LemonRandomDevice device)
{
	::CryptReleaseContext((HCRYPTPROV)device,0);
}

LEMON_SYS_API size_t LemonRandomDeviceNext(LemonRandomDevice device,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t result;

	if(!CryptGenRandom((HCRYPTPROV)device, sizeof(result),static_cast<BYTE*>(static_cast<void*>(&result)))) 
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return 0;
	}

	return result;
}

#elif defined(LEMON_RANDOM_URANDOM) || defined(LEMON_RANDOM_PRNGD)
#include <fstream>
LEMON_IMPLEMENT_HANDLE(LemonRandomDevice){

	std::ifstream urandom;

	LEMON_HANDLE_STRUCT_NAME(LemonRandomDevice)()
#ifdef LEMON_RANDOM_PRNGD
		:urandom("/opt/openssl/prngd/prngd",std::ios::in|std::ios::binary)
#else
		:urandom("/dev/urandom",std::ios::in|std::ios::binary)
#endif
	{

	}

	~LEMON_HANDLE_STRUCT_NAME(LemonRandomDevice)()
	{
		urandom.close();
	}

};

LEMON_SYS_API LemonRandomDevice LemonCreateRandomDevice(LemonErrorInfo * errorCode){

	LemonRandomDevice device = new LEMON_HANDLE_STRUCT_NAME(LemonRandomDevice)();

	return device;

}

LEMON_SYS_API void LemonReleaseRandomDevice(LemonRandomDevice device){
	delete device;
}

LEMON_SYS_API size_t LemonRandomDeviceNext(LemonRandomDevice device,LemonErrorInfo* errorCode){

	size_t result;

	LEMON_RESET_ERRORINFO(*errorCode);

	if(!device->urandom.read((char*)&result,sizeof(result)))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return 0;
	}

	return result;
}
#else
# error "not implement"
#endif