#include <lemon/io/io.h>

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{
	io->Close(io->Service,io);
}
