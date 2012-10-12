#include <lemon/memory/hash.h>

LEMON_DECLARE_HANDLE(LemonHashMapItem);

LEMON_DECLARE_HANDLE(LemonHashMapSlot);


//////////////////////////////////////////////////////////////////////////

LEMON_IMPLEMENT_HANDLE(LemonHashMapItem){
	LemonHashMapItem					*Prev;

	LemonHashMapItem					*Next;

	lemon_byte_t						Data[1];
};

LEMON_IMPLEMENT_HANDLE(LemonHashMapSlot){

	LemonHashMapSlot				*Next;

	LemonHashMapItem				Items[1];
};

LEMON_IMPLEMENT_HANDLE(LemonHashMap){

	size_t								Size;
	
	size_t								(*HashFunction)(void *);

	int									(*CompareFunction)(void *, void *);

	size_t							ItemsOfPerSlot;

	LemonHashMapSlot					Slots;
};