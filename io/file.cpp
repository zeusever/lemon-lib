#include <string>
#include <lemon/io/file.h>

#ifdef WIN32

LEMON_IO_API 
	size_t		
	LemonGetCurrentDirectory(
	__lemon_in char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD length = GetCurrentDirectoryA((DWORD)bufferSize,buffer);

	if(0 == length){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return length;
}

LEMON_IO_API void LemonSetCurrentDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::SetCurrentDirectoryA(directory))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

LEMON_IO_API void LemonRemoveDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::RemoveDirectoryA(directory))  
	{  
		LEMON_WIN32_ERROR(*errorCode,GetLastError());  
	} 

}

LEMON_IO_API void LemonCreateDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::CreateDirectoryA(directory,NULL))  
	{  
		LEMON_WIN32_ERROR(*errorCode,GetLastError());  
	} 
}

LEMON_IO_API lemon_bool LemonIsDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD attributes = GetFileAttributesA(directory);

	return ((FILE_ATTRIBUTE_DIRECTORY & attributes) == FILE_ATTRIBUTE_DIRECTORY)?lemon_true:lemon_false;
}

LEMON_IO_API lemon_bool LemonCheckFileExist(const char * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD attributes = ::GetFileAttributesA(file);

	return (INVALID_FILE_ATTRIBUTES != attributes)?lemon_true:lemon_false;
}

LEMON_IO_API void LemonDeleteFile(const char * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::DeleteFileA(file))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError()); 
	}
}

LEMON_IO_API void LemonMoveFile(const char * source,const char * target,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::MoveFileA(source,target))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError()); 
	}
}

LEMON_IMPLEMENT_HANDLE(LemonDirectoryEnumerator)
{
	HANDLE	Handle;

	WIN32_FIND_DATAA Data;

	std::string path;
};



LEMON_IO_API LemonDirectoryEnumerator LemonDirectoryChildren
	(
	const char * directory,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	std::string searchString = directory;

	searchString += "\\*";

	LEMON_ALLOC_HANDLE(LemonDirectoryEnumerator,enumerator);

	enumerator->path = searchString;

	enumerator->Handle = INVALID_HANDLE_VALUE;

	return enumerator;

}

LEMON_IO_API const char* LemonDirectoryEnumeratorNext
	(
	LemonDirectoryEnumerator enumerator,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(enumerator->Handle == INVALID_HANDLE_VALUE){
		
		enumerator->Handle = ::FindFirstFileA(enumerator->path.c_str(),&enumerator->Data);

		if(INVALID_HANDLE_VALUE == enumerator->Handle){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			return NULL;
		}

	}else{

		if(!FindNextFileA(enumerator->Handle,&enumerator->Data)){

			DWORD lastErrorCode = GetLastError();

			if(ERROR_NO_MORE_FILES != lastErrorCode)
			{
				LEMON_WIN32_ERROR(*errorCode,GetLastError());
			}

			return NULL;
		}
	}

	return enumerator->Data.cFileName;

}

LEMON_IO_API void LemonReleaseDirectoryEnumerator(LemonDirectoryEnumerator enumerator)
{
	::FindClose(enumerator->Handle);

	delete enumerator;
}


#else
#endif //