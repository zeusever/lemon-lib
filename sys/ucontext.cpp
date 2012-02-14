#include <lemon/sys/configure.h>
#ifdef LEMON_USE_BUILDIN_UCONTEXT
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/utsname.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <lemon/sys/ucontext.h>

#if defined(__i386__)
EXTERN_C void
makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	int *sp;
    
    va_list va;
    
    int data;
    va_start(va, argc);
    data=(int)va_arg(va,void*);
    va_end(va);
	sp = (int*)ucp->uc_stack.ss_sp+ucp->uc_stack.ss_size/sizeof(int);
	sp -= 1;
	sp = (int*)((uintptr_t)sp - (uintptr_t)sp%16);	/* 16-align for OS X */
    memmove(sp, &data,sizeof(int));
	*--sp = 0;		/* return address */
	ucp->uc_mcontext.mc_eip = (long)func;
	ucp->uc_mcontext.mc_esp = (int)sp;
}
#elif defined (__x86_64__)

void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	long *sp;
	va_list va;
    
	memset(&ucp->uc_mcontext, 0, sizeof ucp->uc_mcontext);

	va_start(va, argc);
	ucp->uc_mcontext.mc_rdi = (long)va_arg(va, void*);
	//ucp->uc_mcontext.mc_rsi = va_arg(va, int);
	va_end(va);
	sp = (long*)ucp->uc_stack.ss_sp+ucp->uc_stack.ss_size/sizeof(long);
	sp -= argc;
	sp = (long*)((uintptr_t)sp - (uintptr_t)sp%16);	/* 16-align for OS X */
	*--sp = 0;	/* return address */
	ucp->uc_mcontext.mc_rip = (long)func;
	ucp->uc_mcontext.mc_rsp = (long)sp;
}

#else

#endif 


int
swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
	if(getcontext(oucp) == 0)
		setcontext(ucp);
	return 0;
}

#endif //LEMON_USE_BUILDIN_UCONTEXT