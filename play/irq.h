/**
* 
* @file     IRQ(...)
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/23
*/
#ifndef LEMON_PLAY_IRQ_H
#define LEMON_PLAY_IRQ_H

#include <lemon/play/abi.h>	
#define LEMON_PLAY_IRQ_MAX_LEVEL										0x01

#define LEMON_PLAY_IRQ_PASSIVE_LEVEL									0x00


//////////////////////////////////////////////////////////////////////////

#define LEMON_PLAY_IRQ_CREATE											0x00

#define LEMON_PLAY_IRQ_JOIN												0x01

#define LEMON_PLAY_IRQ_WAKEUPE											0x02

//////////////////////////////////////////////////////////////////////////

typedef union LemonPlayIRQData{
	struct{
		LemonActorProc	Proc;
		void			*UserData;
	} TaskData;
} LemonPlayIRQData;

LEMON_IMPLEMENT_HANDLE(LemonPlayIRQ){

	size_t				IRQType;

	LemonActor			Source;/*the IRQ source actor*/

	LemonActor			Object;/*the IRQ target actor*/

	LemonPlayIRQData	Data;

	LemonPlayIRQ		NextIRQ;
};

typedef struct LemonPlayIRQArray{

	volatile size_t					IRQs;

	volatile LemonPlayIRQ			Array;

	volatile LemonPlayIRQ			Tail;

}LemonPlayIRQArray;	

#endif  //LEMON_PLAY_IRQ_H
