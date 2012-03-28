/**
* 
* @file     io
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/16
*/
#ifndef LEMON_IO_ABI_H
#define LEMON_IO_ABI_H

#include <lemon/sys/abi.h>
#include <lemon/io/configure.h>

LEMON_DECLARE_HANDLE(LemonIo);

LEMON_DECLARE_HANDLE(LemonIoData);

LEMON_DECLARE_HANDLE(LemonSocket);

LEMON_DECLARE_HANDLE(LemonIoDevice);

typedef void(*LemonAsyncIoCallback)(
	__lemon_in void		*userData,
	__lemon_in size_t	numberOfBytesTransferred,
	__lemon_in const LemonErrorInfo * errorCode);


LEMON_IO_API 
	size_t
	LemonIoWrite(
	__lemon_in LemonIo io,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t writeSize,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_IO_API 
	void
	LemonIoAsyncWrite(
	__lemon_in LemonIo io,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t writeSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_IO_API 
	size_t
	LemonIoRead(
	__lemon_in LemonIo io,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t readSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	void
	LemonIoAsyncRead(
	__lemon_in LemonIo io,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t readSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo * errorCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_IO_API LemonIoDevice LemonCreateIoDevice( __lemon_inout LemonErrorInfo *errorCode)
///
/// @brief	Lemon create i/o device. 
///
/// @author	Yuki
/// @date	2012/2/16
///
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_IO_API 
	LemonIoDevice 
	LemonCreateIoDevice(
	__lemon_inout LemonErrorInfo *errorCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_IO_API void LemonReleaseIoDevice( __lemon_free LemonIoDevice device)
///
/// @brief	Lemon release i/o device. 
///
/// @author	Yuki
/// @date	2012/2/16
///
/// @param	device	The device. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_IO_API 
	void 
	LemonReleaseIoDevice(
	__lemon_free LemonIoDevice device);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_IO_API void LemonJoinAsWorkThread( __lemon_in LemonIoDevice device,
/// 	__lemon_inout LemonErrorInfo *errorCode)
///
/// @brief	When call this function,the current calling thread will join as iodevice working thread
///
/// @author	Yuki
/// @date	2012/2/16
///
/// @param	device				The device. 
/// @param [in,out]	errorCode	If non-null, the error code. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void LemonJoinAsIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_IO_API void LemonStopAllIoDeviceWorkThread( __lemon_in LemonIoDevice device,
/// 	__lemon_inout LemonErrorInfo *errorCode)
///
/// @brief	Lemon stop all i/o device work thread. 
///
/// @author	Yuki
/// @date	2012/2/16
///
/// @param	device				The device. 
/// @param [in,out]	errorCode	If non-null, the error code. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void LemonStopAllIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void LemonPostIoDeviceCompeleteEvent(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_in size_t numberOfBytesTransferred,
	__lemon_inout LemonErrorInfo *errorCode);


#endif //LEMON_IO_IO_H