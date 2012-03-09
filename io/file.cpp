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

	LemonDirectoryEnumerator enumerator = new LEMON_HANDLE_STRUCT_NAME(LemonDirectoryEnumerator)();

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
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

LEMON_IO_API
	size_t
	LemonGetCurrentDirectory(
	__lemon_in char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	memset(buffer,0,bufferSize);

	const char * result = getcwd(buffer,bufferSize);

	if(NULL == result){

		LEMON_POSIX_ERROR(*errorCode,errno);

		return 0;
	}

	return strlen(result);
}

LEMON_IO_API void LemonSetCurrentDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != chdir(directory)){

		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

LEMON_IO_API void LemonRemoveDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(rmdir(directory) != 0){

		LEMON_POSIX_ERROR(*errorCode,errno);
	}

}

LEMON_IO_API void LemonCreateDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != mkdir(directory,0777)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

LEMON_IO_API lemon_bool LemonIsDirectory(const char * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	struct stat st;

	if(0 != lstat(directory,&st)){

		if(ENOENT != errno){

			LEMON_POSIX_ERROR(*errorCode,errno);
		}

		return lemon_false;
	}

	return S_ISDIR(st.st_mode)?lemon_true:lemon_false;
}

LEMON_IO_API lemon_bool LemonCheckFileExist(const char * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != access(file,0)){

		if(ENOENT != errno) LEMON_POSIX_ERROR(*errorCode,errno);

		return lemon_false;
	}

	return lemon_true;
}

LEMON_IO_API void LemonDeleteFile(const char * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != unlink(file)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

LEMON_IO_API void LemonMoveFile(const char * source,const char * target,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != rename(source,target)){

		LEMON_POSIX_ERROR(*errorCode,errno);

	}
}

LEMON_IMPLEMENT_HANDLE(LemonDirectoryEnumerator)
{
	DIR *Dir;

	dirent * Entry;

	std::string path;
};

LEMON_IO_API LemonDirectoryEnumerator LemonDirectoryChildren
	(
	const char * directory,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonDirectoryEnumerator enumerator = new LEMON_HANDLE_STRUCT_NAME(LemonDirectoryEnumerator)();

	enumerator->path = directory;

	enumerator->Dir = NULL;

	return enumerator;

}

LEMON_IO_API const char* LemonDirectoryEnumeratorNext
	(
	LemonDirectoryEnumerator enumerator,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(enumerator->Dir == NULL){

		enumerator->Dir = opendir(enumerator->path.c_str());

		if(NULL == enumerator->Dir ){

			LEMON_POSIX_ERROR(*errorCode,errno);

			return NULL;
		}

	}

	enumerator->Entry = readdir(enumerator->Dir);

	if(NULL == enumerator->Entry) return NULL;

	return enumerator->Entry->d_name;
}

LEMON_IO_API void LemonReleaseDirectoryEnumerator(LemonDirectoryEnumerator enumerator)
{
	closedir(enumerator->Dir);

	delete enumerator;
}

#endif //
