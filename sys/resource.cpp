#include <lemon/sys/uuid.h>
#include <lemon/sys/datetime.h>
#include <lemon/sys/resource.h>
#include <lemon/sys/errorcode.h>
#include <lemon/sys/text.h>
typedef union {
	LemonResourceErrorInfo			ErrorInfo;

	LemonResourceText				Text;

	LemonResourceTraceCatalog		TraceCatalog;

	LemonResourceTraceEvent			TraceEvent;

} LemonResourceItem;

LEMON_IMPLEMENT_HANDLE(LemonResourceIterator)
{
	LemonResourceIterator				Prev;

	LemonResourceIterator				Next;

	size_t								Type;

	LemonResourceItem					Item;

	void(*Release)(LemonResource,LemonResourceIterator self);
};

LEMON_IMPLEMENT_HANDLE(LemonResource)
{
	LemonUuid						Uuid;

	LemonVersion					Version;

	LemonResourceIterator			Items;
};

LEMON_SYS_API 
	LemonResource 
	LemonCreateResource(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonResource,resource);

	resource->Uuid = LemonUuidGenerate(errorCode);

	if(LEMON_FAILED(*errorCode))
	{
		LEMON_FREE_HANDLE(resource);

		return LEMON_HANDLE_NULL_VALUE;
	}

	return resource;
}

LEMON_SYS_API
	void LemonReleaseResource(
	__lemon_free LemonResource resource)
{
	LemonResourceIterator iter = resource->Items;

	while (iter)
	{
		LemonResourceIterator current = iter;

		iter = iter->Next;

		current->Release(resource,current);
	}

	LEMON_FREE_HANDLE(resource);
}


LEMON_SYS_API
	const LemonUuid * 
	LemonResourceUuid(
	__lemon_in LemonResource resource)
{
	return &resource->Uuid;
}

LEMON_SYS_PRIVATE static lemon_uint16_t __atoi(const lemon_char_t * from,const lemon_char_t * to)
{
	const static lemon_char_t numbers[] = LEMON_TEXT("0123456789");

	const lemon_char_t * iter = from;

	lemon_uint16_t result = 0;

	while(iter != to)
	{
		lemon_uint16_t val;

		for(val = 0; val < sizeof(numbers)/sizeof(numbers[0]); ++ val)
		{
			if(*iter == numbers[val]) break;
		}

		if(val  == 10)
		{
			return (lemon_uint16_t)-1;
		}

		result = result * 10 + val;

		++ iter;
	}

	return result;
}

LEMON_SYS_API
	const LemonVersion* LemonResetResourceVersion(
	__lemon_in LemonResource  resource,
	__lemon_in const lemon_char_t * versionstring,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t counter = 0;

	LemonDateTime datetime;

	LemonTime now = LemonNow(errorCode);

	if(LEMON_FAILED(*errorCode)) return NULL;

	LemonLocalDateTime(&datetime,now,errorCode);

	if(LEMON_FAILED(*errorCode)) return NULL;

	struct {const lemon_char_t *begin;const lemon_char_t * end;} nodes[4];

	const lemon_char_t * iter = versionstring;

	nodes[0].begin = versionstring;

	while(LEMON_TEXT('\0') != * iter)
	{
		if(LEMON_TEXT('.') == *iter)
		{
			nodes[counter].end = iter;

			++ counter;

			++ iter;

			if(LEMON_TEXT('\0') != * iter)
			{
				nodes[counter].begin = iter;
			}

			continue;
		}

		++ iter;
	}

	if(nodes[counter].begin != iter)
	{
		nodes[counter].end = iter;

		++ counter;
	}

	switch (counter)
	{
	case 3:
		{
			resource->Version.Major = __atoi(nodes[0].begin,nodes[0].end);

			if(resource->Version.Major == (lemon_uint16_t)-1) goto Error;

			resource->Version.Minor = __atoi(nodes[1].begin,nodes[1].end);

			if(resource->Version.Minor == (lemon_uint16_t)-1) goto Error;

			if(nodes[2].end - nodes[2].begin != 1 || *nodes[2].begin != LEMON_TEXT('*')) goto Error;
			
			resource->Version.Build = (lemon_uint16_t)(datetime.DayOfYear);

			resource->Version.Reversion = (lemon_uint16_t)((datetime.Hour * 3600 + datetime.Minute * 60 + datetime.Second)/2);

			break;
		}
	case 4:
		{
			resource->Version.Major = __atoi(nodes[0].begin,nodes[0].end);

			if(resource->Version.Major == (lemon_uint16_t)-1) goto Error;

			resource->Version.Minor = __atoi(nodes[1].begin,nodes[1].end);

			if(resource->Version.Minor == (lemon_uint16_t)-1) goto Error;

			resource->Version.Build = __atoi(nodes[2].begin,nodes[2].end);

			if(resource->Version.Build == (lemon_uint16_t)-1) goto Error;

			if(nodes[3].end - nodes[3].begin == 1 && *nodes[3].begin == LEMON_TEXT('*'))
			{
				resource->Version.Reversion = (lemon_uint16_t)((datetime.Hour * 3600 + datetime.Minute * 60 + datetime.Second)/2);
			}
			else
			{
				resource->Version.Reversion = __atoi(nodes[3].begin,nodes[3].end);

				if(resource->Version.Reversion == (lemon_uint16_t)-1) goto Error;	
			}

			break;
		}
	default:
		{
			goto Error;
		}
		
	}

	return &resource->Version;

Error:

	LEMON_USER_ERROR(*errorCode,LEMON_SYS_INVALID_VERSION_STRING);

	return NULL;
}

LEMON_SYS_API
	const LemonVersion* 
	LemonResourceVersion(
	__lemon_in LemonResource  resource)
{
	return &resource->Version;
}


//////////////////////////////////////////////////////////////////////////

static void __add_iterator(
	__lemon_in LemonResource  resource,
	__lemon_in LemonResourceIterator iter)
{
	if(resource->Items)
	{
		resource->Items->Prev = iter;
	}

	iter->Next = resource->Items;

	resource->Items = iter;
}

static void __errorinfo_Release(LemonResource,LemonResourceIterator iter)
{
	LemonResourceErrorInfo * object = &iter->Item.ErrorInfo;

	lemon_free_string((lemon_char_t*)object->Name);

	lemon_free_string((lemon_char_t*)object->Description);

	LEMON_FREE_HANDLE(iter);
}

LEMON_SYS_API
	void 
	LemonAddResourceErrorInfo(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceErrorInfo * info,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonResourceIterator,iter);

	LemonResourceErrorInfo * object = &iter->Item.ErrorInfo;

	object->Name = lemon_copy_string(info->Name);

	object->Description = lemon_copy_string(info->Description);

	object->Code = info->Code;

	iter->Release = &__errorinfo_Release;

	iter->Type = LEMON_RESOURCE_TERRORINFO;

	__add_iterator(resource,iter);
}

static void __tracecatalog_Release(LemonResource,LemonResourceIterator iter)
{
	LemonResourceTraceCatalog * object = &iter->Item.TraceCatalog;

	lemon_free_string((lemon_char_t*)object->Name);

	lemon_free_string((lemon_char_t*)object->Description);

	LEMON_FREE_HANDLE(iter);
}

LEMON_SYS_API
	void 
	LemonAddResourceTraceCatalog(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceTraceCatalog * val,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonResourceIterator,iter);

	LemonResourceTraceCatalog * object = &iter->Item.TraceCatalog;

	object->Name = lemon_copy_string(val->Name);

	object->Description = lemon_copy_string(val->Description);

	object->Value = val->Value;

	iter->Release = &__tracecatalog_Release;

	iter->Type = LEMON_RESOURCE_TTRACECATALOG;

	__add_iterator(resource,iter);
}

static void __tracevent_Release(LemonResource,LemonResourceIterator iter)
{
	LemonResourceTraceEvent * object = &iter->Item.TraceEvent;

	lemon_free_string((lemon_char_t*)object->Text);

	LEMON_FREE_HANDLE(iter);
}

LEMON_SYS_API
	void 
	LemonAddResourceTraceEvent(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceTraceEvent * val,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonResourceIterator,iter);

	LemonResourceTraceEvent * object = &iter->Item.TraceEvent;

	object->Text = lemon_copy_string(val->Text);

	object->Sequence = val->Sequence;

	iter->Release = &__tracevent_Release;

	iter->Type = LEMON_RESOURCE_TTRACEEVENT;

	__add_iterator(resource,iter);
}

static void __text_Release(LemonResource,LemonResourceIterator iter)
{
	LemonResourceText* object = &iter->Item.Text;

	lemon_free_string((lemon_char_t*)object->Global);

	lemon_free_string((lemon_char_t*)object->Locale);

	LEMON_FREE_HANDLE(iter);
}

LEMON_SYS_API
	void 
	LemonAddResourceText(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceText * val,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonResourceIterator,iter);

	LemonResourceText * object = &iter->Item.Text;

	object->Global = lemon_copy_string(val->Global);

	object->Locale = lemon_copy_string(val->Locale);

	iter->Release = &__text_Release;

	iter->Type = LEMON_RESOURCE_TTEXT;

	__add_iterator(resource,iter);
}
//////////////////////////////////////////////////////////////////////////

LEMON_SYS_API
	const LemonResourceErrorInfo *
	LemonGetResourceErrorInfo(
	__lemon_in LemonResource  resource,
	__lemon_in lemon_uint32_t code,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonResourceIterator iter = resource->Items;

	while(iter)
	{
		if(iter->Type == LEMON_RESOURCE_TERRORINFO)
		{
			if(iter->Item.ErrorInfo.Code == code) return &iter->Item.ErrorInfo;
		}

		iter = iter->Next;
	}

	return NULL;
}


LEMON_SYS_API
	const LemonResourceTraceCatalog *
	LemonGetResourceTraceCatalog(
	__lemon_in LemonResource  resource,
	__lemon_in lemon_uint32_t val,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonResourceIterator iter = resource->Items;

	while(iter)
	{
		if(iter->Type == LEMON_RESOURCE_TTRACECATALOG)
		{
			if(iter->Item.TraceCatalog.Value == val) return &iter->Item.TraceCatalog;
		}

		iter = iter->Next;
	}

	return NULL;
}

LEMON_SYS_API
	const LemonResourceTraceEvent *
	LemonGetResourceTraceEvent(
	__lemon_in LemonResource  resource,
	__lemon_in lemon_uint32_t sequence,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonResourceIterator iter = resource->Items;

	while(iter)
	{
		if(iter->Type == LEMON_RESOURCE_TTRACEEVENT)
		{
			if(iter->Item.TraceEvent.Sequence == sequence) return &iter->Item.TraceEvent;
		}

		iter = iter->Next;
	}

	return NULL;
}

LEMON_SYS_API
	const LemonResourceText *
	LemonGetResourceText(
	__lemon_in LemonResource  resource,
	__lemon_in const lemon_char_t *global,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonResourceIterator iter = resource->Items;

	while(iter)
	{
		if(iter->Type == LEMON_RESOURCE_TTEXT)
		{
			if(0 == lemon_strcmp(global,iter->Item.Text.Global))
			{
				return &iter->Item.Text;
			}
		}

		iter = iter->Next;
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////


LEMON_SYS_API
	LemonResourceIterator 
	LemonGetResourceIterator(
	__lemon_in LemonResource  resource)
{
	return resource->Items;
}


LEMON_SYS_API
	void * 
	LemonResourceIteratorNext(
	__lemon_inout LemonResourceIterator * iter,
	__lemon_in size_t itemtypes)
{
	while(*iter)
	{
		LemonResourceIterator current = *iter;

		*iter = (*iter)->Next;

		if((current->Type & itemtypes) == current->Type)
		{
			return &current->Item;
		}
	}

	return NULL;
}