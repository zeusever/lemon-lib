#include <cassert>
#include <lemon/sys/coroutine.h>
#include <lemon/sys/assembly.h>

#if defined(LEMON_USE_BUILDIN_UCONTEXT) || defined (LEMON_HAS_UCONTEXT_H)
#   ifdef LEMON_USE_BUILDIN_UCONTEXT
#       include <lemon/sys/ucontext.h>
#   else
extern "C"{
#       include <ucontext.h>
}
#   endif //LEMON_USE_BUILDIN_UCONTEXT
LEMON_IMPLEMENT_HANDLE(LemonCoroutine){

	LemonCoroutine Parent;

	ucontext_t Context;

	LemonCoroutineStatus Status;

	LemonCoroutineProc Proc;

	void * Data;
};

void FiberProc(void * data){

	LemonCoroutine coro = (LemonCoroutine)data;

	coro->Proc(coro,coro->Data);

	coro->Status = LemonCoroutineStatuts_Stopped;

	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonCoroutineYield(coro,&errorCode);
}

LEMON_SYS_API LemonCoroutine LemonThreadToCoroutine(__lemon_inout LemonErrorInfo *errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonCoroutine coro = new LEMON_HANDLE_STRUCT_NAME(LemonCoroutine)();

	coro->Parent = 0;

	coro->Context.uc_link = NULL;

	coro->Context.uc_stack.ss_size = 0;

	coro->Status = LemonCoroutineStatuts_Running;

	return coro;
}



LEMON_SYS_API LemonCoroutine LemonCreateCoroutine(
	__lemon_in LemonCoroutine parent,
	__lemon_in LemonCoroutineProc proc,
	__lemon_in void* data,
	__lemon_in size_t stackSize,
	__lemon_inout LemonErrorInfo *errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		LemonCoroutine coro = new LEMON_HANDLE_STRUCT_NAME(LemonCoroutine)();

		if(0 != getcontext(&coro->Context)){

			LEMON_POSIX_ERROR(*errorCode,errno);

			delete coro;

			return LEMON_HANDLE_NULL_VALUE;
		}

		coro->Context.uc_link = &parent->Context;

		coro->Context.uc_stack.ss_sp = new lemon_byte_t[stackSize];

		coro->Context.uc_stack.ss_size = stackSize;

		coro->Data = data;

		coro->Parent = parent;

		coro->Proc = proc;

		coro->Status = LemonCoroutineStatuts_ReadyForRunning;
#ifdef _AIX    
		makecontext(&coro->Context,(void (*)(...))FiberProc,1,coro);
#else
		makecontext(&coro->Context,(void (*)(void))FiberProc,1,coro);
#endif //!_AIX

		return coro;
}

LEMON_SYS_API void LemonReleaseCoroutine(__lemon_in LemonCoroutine coro){

	if(0 != coro->Context.uc_stack.ss_size){
		delete [] (lemon_byte_t*)coro->Context.uc_stack.ss_sp;
	}

	delete coro;
}

LEMON_SYS_API void LemonCoroutineResume(__lemon_in LemonCoroutine coro,__lemon_inout LemonErrorInfo *errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	if(LemonCoroutineStatuts_ReadyForRunning != coro->Status && LemonCoroutineStatuts_Sleeping != coro->Status){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_COROUTINE_EXIT);

		return;
	}

	coro->Status = LemonCoroutineStatuts_Running;

	coro->Parent->Status = LemonCoroutineStatuts_Sleeping;

	if(-1 == swapcontext(&coro->Parent->Context, &coro->Context)){

		LEMON_POSIX_ERROR(*errorCode, errno);

		return;
	}

	coro->Parent->Status = LemonCoroutineStatuts_Running;

	if(coro->Status == LemonCoroutineStatuts_Running){

		coro->Status = LemonCoroutineStatuts_Sleeping;
	}
}

LEMON_SYS_API void LemonCoroutineYield(__lemon_in LemonCoroutine coro,__lemon_inout LemonErrorInfo *errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	if(-1 == swapcontext(&coro->Context,&coro->Parent->Context)){

		LEMON_POSIX_ERROR(*errorCode, errno);

		return;
	}
}

LEMON_SYS_API LemonCoroutine LemonCoroutineParent(__lemon_in LemonCoroutine coro){
	return coro->Parent;
}

LEMON_SYS_API LemonCoroutineStatus LemonGetCoroutineStatus(__lemon_in LemonCoroutine coro){
	return coro->Status;
}

#elif defined(LEMON_USE_WIN32_FIBER)
LEMON_IMPLEMENT_HANDLE(LemonCoroutine)
{
	LemonCoroutine Parent;

	LemonCoroutineStatus Status;

	PVOID Fiber;

	LemonCoroutineProc Proc;

	void * Data;
};

LEMON_SYS_API LemonCoroutine LemonThreadToCoroutine(__lemon_inout LemonErrorInfo *errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	void * fiber = ConvertThreadToFiber(NULL);

	if(NULL == fiber){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	LemonCoroutine coro = new LEMON_HANDLE_STRUCT_NAME(LemonCoroutine)();

	coro->Data = NULL;

	coro->Parent = NULL;

	coro->Proc = NULL;

	coro->Fiber = fiber;

	coro->Status = LemonCoroutineStatuts_Running;

	return coro;
}

VOID CALLBACK FiberProc(__in  PVOID lpParameter){

	LemonCoroutine coro = (LemonCoroutine)lpParameter;

	coro->Proc(coro,coro->Data);

	coro->Status = LemonCoroutineStatuts_Stopped;

	LEMON_DECLARE_ERRORINFO(errorcode);

	LemonCoroutineYield(coro,&errorcode);

	//if(LEMON_FAILED(errorcode)) throw lemon::Exception(errorcode);
}


LEMON_SYS_API LemonCoroutine LemonCreateCoroutine(
	__lemon_in LemonCoroutine parent,
	__lemon_in LemonCoroutineProc proc,
	__lemon_in void* data,
	__lemon_in size_t stackSize,
	__lemon_inout LemonErrorInfo *errorCode){

		LemonCoroutine coro = new LEMON_HANDLE_STRUCT_NAME(LemonCoroutine)();

		void * fiber  = ::CreateFiberEx(stackSize,stackSize,0 ,FiberProc,coro);

		if(NULL == fiber){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			delete coro;

			return LEMON_HANDLE_NULL_VALUE;
		}

		coro->Data = data;

		coro->Parent = parent;

		coro->Proc = proc;

		coro->Fiber = fiber;

		coro->Status = LemonCoroutineStatuts_ReadyForRunning;

		return coro;
}

LEMON_SYS_API void LemonReleaseCoroutine(__lemon_in LemonCoroutine coro){

	if(coro->Parent){

		::DeleteFiber(coro->Fiber);

	}else{

		::ConvertFiberToThread();

	}

	delete coro;
}

LEMON_SYS_API void LemonCoroutineResume(__lemon_in LemonCoroutine coro,__lemon_inout LemonErrorInfo *errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	if(LemonCoroutineStatuts_ReadyForRunning != coro->Status && LemonCoroutineStatuts_Sleeping != coro->Status){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_COROUTINE_EXIT);

		return;
	}

	coro->Status = LemonCoroutineStatuts_Running;

	coro->Parent->Status = LemonCoroutineStatuts_Sleeping;

	::SwitchToFiber(coro->Fiber);

	coro->Parent->Status = LemonCoroutineStatuts_Running;

	if(coro->Status == LemonCoroutineStatuts_Running){
		coro->Status = LemonCoroutineStatuts_Sleeping;
	}
}

LEMON_SYS_API void LemonCoroutineYield(__lemon_in LemonCoroutine coro,__lemon_inout LemonErrorInfo *errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(coro->Parent);

	::SwitchToFiber(coro->Parent->Fiber);
}

LEMON_SYS_API LemonCoroutine LemonCoroutineParent(__lemon_in LemonCoroutine coro){
	return coro->Parent;
}

LEMON_SYS_API LemonCoroutineStatus LemonGetCoroutineStatus(__lemon_in LemonCoroutine coro){
	return coro->Status;
}
#else
# error "not implement"
#endif //
