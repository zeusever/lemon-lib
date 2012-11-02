#include <lemon/io/io.h>
#include <lemon/io/reactor_events.h>

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{
	LemonCancelIOEventsOfFile(io->Service->Q,io->Handle);

	io->Close(io);
}

LEMON_IO_API
	void 
	LemonCancelIO(
	__lemon_in LemonIO io)
{
	LemonCancelIOEventsOfFile(io->Service->Q,io->Handle);
}