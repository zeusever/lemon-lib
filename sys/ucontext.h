#ifndef LEMON_SYS_UCONTEXT_H
#define LEMON_SYS_UCONTEXT_H

#include <lemon/sys/abi.h>

#include <signal.h>

#undef USE_UCONTEXT
#define USE_UCONTEXT 0

#define mcontext libthread_mcontext
#define mcontext_t libthread_mcontext_t
#define ucontext libthread_ucontext
#define ucontext_t libthread_ucontext_t

#if defined(__i386__)
#define	setcontext(u)	setmcontext(&(u)->uc_mcontext)
#define	getcontext(u)	getmcontext(&(u)->uc_mcontext)
typedef struct mcontext mcontext_t;
typedef struct ucontext ucontext_t;

EXTERN_C	int		swapcontext(ucontext_t*, const ucontext_t*);
EXTERN_C	void	makecontext(ucontext_t*, void(*)(), int, ...);
EXTERN_C	int		getmcontext(mcontext_t*);
EXTERN_C	void	setmcontext(const mcontext_t*);


struct mcontext {
	/*
	 * The first 20 fields must match the definition of
	 * sigcontext. So that we can support sigcontext
	 * and ucontext_t at the same time.
	 */
	int	mc_onstack;		/* XXX - sigcontext compat. */
	int	mc_gs;
	int	mc_fs;
	int	mc_es;
	int	mc_ds;
	int	mc_edi;
	int	mc_esi;
	int	mc_ebp;
	int	mc_isp;
	int	mc_ebx;
	int	mc_edx;
	int	mc_ecx;
	int	mc_eax;
	int	mc_trapno;
	int	mc_err;
	int	mc_eip;
	int	mc_cs;
	int	mc_eflags;
	int	mc_esp;			/* machine state */
	int	mc_ss;
    
	int	mc_fpregs[28];		/* env87 + fpacc87 + u_long */
	int	__spare__[17];
};

struct ucontext {
	/*
	 * Keep the order of the first two fields. Also,
	 * keep them the first two fields in the structure.
	 * This way we can have a union with struct
	 * sigcontext and ucontext_t. This allows us to
	 * support them both at the same time.
	 * note: the union is not defined, though.
	 */
	sigset_t	uc_sigmask;
	mcontext_t	uc_mcontext;
    
	struct ucontext *uc_link;
	stack_t		uc_stack;
	int		__spare__[8];
};

#elif defined(__x86_64__)

#define	setcontext(u)	setmcontext(&(u)->uc_mcontext)
#define	getcontext(u)	getmcontext(&(u)->uc_mcontext)

typedef struct mcontext mcontext_t;
typedef struct ucontext ucontext_t;

EXTERN_C	int		swapcontext(ucontext_t*, const ucontext_t*);
EXTERN_C	void	makecontext(ucontext_t*, void(*)(), int, ...);
EXTERN_C	int		getmcontext(mcontext_t*);
EXTERN_C	void	setmcontext(const mcontext_t*);

struct mcontext {
	/*
	 * The first 20 fields must match the definition of
	 * sigcontext. So that we can support sigcontext
	 * and ucontext_t at the same time.
	 */
	long	mc_onstack;		/* XXX - sigcontext compat. */
	long	mc_rdi;			/* machine state (struct trapframe) */
	long	mc_rsi;
	long	mc_rdx;
	long	mc_rcx;
	long	mc_r8;
	long	mc_r9;
	long	mc_rax;
	long	mc_rbx;
	long	mc_rbp;
	long	mc_r10;
	long	mc_r11;
	long	mc_r12;
	long	mc_r13;
	long	mc_r14;
	long	mc_r15;
	long	mc_trapno;
	long	mc_addr;
	long	mc_flags;
	long	mc_err;
	long	mc_rip;
	long	mc_cs;
	long	mc_rflags;
	long	mc_rsp;
	long	mc_ss;
    
	long	mc_len;			/* sizeof(mcontext_t) */
#define	_MC_FPFMT_NODEV		0x10000	/* device not present or configured */
#define	_MC_FPFMT_XMM		0x10002
	long	mc_fpformat;
#define	_MC_FPOWNED_NONE	0x20000	/* FP state not used */
#define	_MC_FPOWNED_FPU		0x20001	/* FP state came from FPU */
#define	_MC_FPOWNED_PCB		0x20002	/* FP state came from PCB */
	long	mc_ownedfp;
	/*
	 * See <machine/fpu.h> for the internals of mc_fpstate[].
	 */
	long	mc_fpstate[64];
	long	mc_spare[8];
};

struct ucontext {
	/*
	 * Keep the order of the first two fields. Also,
	 * keep them the first two fields in the structure.
	 * This way we can have a union with struct
	 * sigcontext and ucontext_t. This allows us to
	 * support them both at the same time.
	 * note: the union is not defined, though.
	 */
	sigset_t	uc_sigmask;
	mcontext_t	uc_mcontext;
    
	struct ucontext *uc_link;
	stack_t		uc_stack;
	int		__spare__[8];
};

#elif defined(__APPLE__)

#endif //

#endif //