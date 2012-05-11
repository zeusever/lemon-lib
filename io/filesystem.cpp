#include <string>
#include <lemon/io/filesystem.h>

#ifdef WIN32

LEMON_IO_API 
	size_t		
	LemonGetCurrentDirectory(
	__lemon_in lemon_char_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD length = GetCurrentDirectoryW((DWORD)bufferSize,buffer);

	if(0 == length){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return length;
}

LEMON_IO_API void LemonSetCurrentDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::SetCurrentDirectoryW(directory))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

LEMON_IO_API void LemonRemoveDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::RemoveDirectoryW(directory))  
	{  
		LEMON_WIN32_ERROR(*errorCode,GetLastError());  
	} 

}

LEMON_IO_API void LemonCreateDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::CreateDirectoryW(directory,NULL))  
	{  
		LEMON_WIN32_ERROR(*errorCode,GetLastError());  
	} 
}

LEMON_IO_API lemon_bool LemonIsDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD attributes = GetFileAttributesW(directory);

	return ((FILE_ATTRIBUTE_DIRECTORY & attributes) == FILE_ATTRIBUTE_DIRECTORY)?lemon_true:lemon_false;
}

LEMON_IO_API lemon_bool LemonCheckFileExist(const lemon_char_t * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD attributes = ::GetFileAttributesW(file);

	return (INVALID_FILE_ATTRIBUTES != attributes)?lemon_true:lemon_false;
}

LEMON_IO_API void LemonDeleteFile(const lemon_char_t * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::DeleteFileW(file))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError()); 
	}
}

LEMON_IO_API 
	void						
	LemonCopyFile(
	const lemon_char_t * source,
	const lemon_char_t * target,
	LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::CopyFileW(source,target,TRUE))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

LEMON_IO_API void LemonMoveFile(const lemon_char_t * source,const lemon_char_t * target,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::MoveFileW(source,target))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError()); 
	}
}

LEMON_IMPLEMENT_HANDLE(LemonDirectoryEnumerator)
{
	HANDLE	Handle;

	WIN32_FIND_DATAW Data;

	std::wstring path;
};



LEMON_IO_API LemonDirectoryEnumerator LemonDirectoryChildren
	(
	const lemon_char_t * directory,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	std::wstring searchString = directory;

	searchString += LEMON_TEXT("\\*");

	LemonDirectoryEnumerator enumerator = new LEMON_HANDLE_STRUCT_NAME(LemonDirectoryEnumerator)();

	enumerator->path = searchString;

	enumerator->Handle = INVALID_HANDLE_VALUE;

	return enumerator;

}

LEMON_IO_API const lemon_char_t* LemonDirectoryEnumeratorNext
	(
	LemonDirectoryEnumerator enumerator,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(enumerator->Handle == INVALID_HANDLE_VALUE){
		
		enumerator->Handle = ::FindFirstFileW(enumerator->path.c_str(),&enumerator->Data);

		if(INVALID_HANDLE_VALUE == enumerator->Handle){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			return NULL;
		}

	}else{

		if(!FindNextFileW(enumerator->Handle,&enumerator->Data)){

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
	__lemon_in lemon_char_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	memset(buffer,0,bufferSize);

	const lemon_char_t * result = getcwd(buffer,bufferSize);

	if(NULL == result){

		LEMON_POSIX_ERROR(*errorCode,errno);

		return 0;
	}

	return strlen(result);
}

LEMON_IO_API void LemonSetCurrentDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != chdir(directory)){

		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

LEMON_IO_API void LemonRemoveDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(rmdir(directory) != 0){

		LEMON_POSIX_ERROR(*errorCode,errno);
	}

}

LEMON_IO_API void LemonCreateDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != mkdir(directory,0777)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

LEMON_IO_API lemon_bool LemonIsDirectory(const lemon_char_t * directory,LemonErrorInfo * errorCode)
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

LEMON_IO_API lemon_bool LemonCheckFileExist(const lemon_char_t * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != access(file,0)){

		if(ENOENT != errno) LEMON_POSIX_ERROR(*errorCode,errno);

		return lemon_false;
	}

	return lemon_true;
}

LEMON_IO_API void LemonDeleteFile(const lemon_char_t * file,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(0 != unlink(file)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
}

LEMON_IO_API void LemonMoveFile(const lemon_char_t * source,const lemon_char_t * target,LemonErrorInfo * errorCode)
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
	const lemon_char_t * directory,
	LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonDirectoryEnumerator enumerator = new LEMON_HANDLE_STRUCT_NAME(LemonDirectoryEnumerator)();

	enumerator->path = directory;

	enumerator->Dir = NULL;

	return enumerator;

}

LEMON_IO_API const lemon_char_t* LemonDirectoryEnumeratorNext
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
