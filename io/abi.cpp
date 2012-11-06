#include <lemon/io/irp.h>

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{
	LemonIOService service = io->IOService;

	__lemon_io_file handle = io->Handle;

	io->Close(io);

	LemonIRPCloseFile_TS(service->IRPs,handle);
}

LEMON_IO_API
	void 
	LemonCancelIO(
	__lemon_in LemonIO io)
{
	LemonIRPFileCancel_TS(io->IOService->IRPs,io->Handle);
}

LEMON_IO_API 
	lemon_bool 
	LemonIOServiceStatus(
	__lemon_in LemonIOService service)
{
	return service->Status;
}