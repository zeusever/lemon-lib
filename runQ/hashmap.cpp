#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE 
	size_t 
	LemonJobTableHashF(
	__lemon_in const void* key)
{
	return (size_t)*(lemon_job_id*)key;
}

LEMON_RUNQ_PRIVATE 
	int 
	LemonJobTableCompareF(
	__lemon_in const void* l, 
	__lemon_in const void* r)
{
	lemon_job_id lhs = *(lemon_job_id*)l;

	lemon_job_id rhs = *(lemon_job_id*)r;

	if(lhs > rhs) return 1;

	if(lhs == rhs) return 0;

	return -1;
}

LEMON_RUNQ_PRIVATE
	void
	LemonJobTableGc(
	__lemon_in void *userdata,
	__lemon_in const void * , 
	__lemon_in void * val)
{
	LemonRunQ Q = (LemonRunQ)userdata;

	__LemonCloseJob(Q,(LemonJob)val);
}