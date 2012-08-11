#include <lemon/sys/uuid.h>
#include <lemon/sys/datetime.h>
#include <lemon/sys/resource.h>
#include <lemon/sys/assembly.h>
#include <lemon/sys/text.h>

typedef struct LemonResourceTextBinary{

	lemon_uint32_t				Type;
	
	lemon_uint32_t				Global;

	lemon_uint32_t				Locale;

}LemonResourceTextBinary;

typedef struct LemonResourceErrorInfoBinary{

	lemon_uint32_t				Type;

	lemon_uint32_t				Code;
	
	lemon_uint32_t				Name;

	lemon_uint32_t				Description;

}LemonResourceErrorInfoBinary;

typedef struct LemonResourceTraceCatalogBinary{

	lemon_uint32_t				Type;
	
	lemon_uint32_t				Value;

	lemon_uint32_t				Name;

	lemon_uint32_t				Description;

}LemonResourceTraceCatalogBinary;

typedef struct LemonResourceTraceEventBinary{

	lemon_uint32_t				Type;

	lemon_uint32_t				Sequence;

	lemon_uint32_t				Text;					

}LemonResourceTraceEventBinary;

typedef union {
	LemonResourceErrorInfo			ErrorInfo;

	LemonResourceText				Text;

	LemonResourceTraceCatalog		TraceCatalog;

	LemonResourceTraceEvent			TraceEvent;

} LemonResourceItem;


typedef union {
	LemonResourceErrorInfoBinary			ErrorInfo;

	LemonResourceTextBinary					Text;

	LemonResourceTraceCatalogBinary			TraceCatalog;

	LemonResourceTraceEventBinary			TraceEvent;

} LemonResourceBinaryItem;

LEMON_IMPLEMENT_HANDLE(LemonResourceIterator)
{
	LemonResourceIterator				Prev;

	LemonResourceIterator				Next;

	size_t								Type;

	LemonResourceItem					Item;

	LemonResourceBinaryItem				BinaryItem;

	void(*Release)(LemonResource,LemonResourceIterator self);
};

LEMON_IMPLEMENT_HANDLE(LemonResource)
{
	lemon_uint32_t					LocaleId;

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

	resource->LocaleId = 1033;

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


//////////////////////////////////////////////////////////////////////////

typedef struct LemonResourceUTF8String{
	
	struct LemonResourceUTF8String		*Next;

	lemon_uint32_t						Offset;

	size_t								Length;

	lemon_utf8_t						String[1];

}LemonResourceUTF8String;


typedef struct LemonResourceStringTable{

	LemonResourceUTF8String	*Header;

	LemonResourceUTF8String	*Tail;

}LemonResourceStringTable;

LEMON_SYS_PRIVATE
	void __LemonResourceReleaseUTF8String(__lemon_free LemonResourceUTF8String * string)
{
	free(string);
}

LEMON_SYS_PRIVATE
	lemon_uint32_t 
	__LemonResourceStringTableAdd(
	__lemon_in LemonResourceStringTable * table,
	__lemon_in const lemon_char_t *source,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sourceSize = lemon_strlen(source) + 1;

	size_t length = sourceSize * 6;

	LemonResourceUTF8String * string = (LemonResourceUTF8String *)malloc(length + sizeof(LemonResourceUTF8String));

	memset(string,0,length + sizeof(LemonResourceUTF8String));

	string->Length = LemonToUTF8(source,sourceSize,(lemon_byte_t*)string->String,length,errorCode);

	if(LEMON_FAILED(*errorCode)) return (lemon_uint32_t)-1;

	if(table->Header == NULL){

		string->Offset = 0;

		table->Header = string;

		table->Tail = string;

	}else{

		string->Offset = table->Tail->Offset + (lemon_uint32_t)table->Tail->Length;

		table->Tail->Next = string;

		table->Tail = string;
	}

	return htonl(string->Offset);
}

LEMON_SYS_API
	void LemonResourceWrite(
	__lemon_in LemonResource  resource,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonResourceStringTable stringTable = {NULL,NULL};

	LemonResourceIterator iter = resource->Items;

	lemon_uint32_t itemTableSize = 0;

	lemon_uint32_t	localeId = htonl(resource->LocaleId);

	LemonVersion version = 
	{
		htons(resource->Version.Major),

		htons(resource->Version.Minor),

		htons(resource->Version.Build),

		htons(resource->Version.Reversion)
	};

	while(iter)
	{
		switch(iter->Type)
		{
		case LEMON_RESOURCE_TTEXT:
			{
				itemTableSize += sizeof(LemonResourceTextBinary);

				iter->BinaryItem.Text.Type = htonl(LEMON_RESOURCE_TTEXT);

				iter->BinaryItem.Text.Global = __LemonResourceStringTableAdd(&stringTable,iter->Item.Text.Global,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;

				iter->BinaryItem.Text.Locale = __LemonResourceStringTableAdd(&stringTable,iter->Item.Text.Locale,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;

				break;
			}

		case LEMON_RESOURCE_TERRORINFO:
			{
				itemTableSize += sizeof(LemonResourceErrorInfoBinary);

				iter->BinaryItem.ErrorInfo.Type = htonl(LEMON_RESOURCE_TERRORINFO);

				iter->BinaryItem.ErrorInfo.Name = __LemonResourceStringTableAdd(&stringTable,iter->Item.ErrorInfo.Name,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				iter->BinaryItem.ErrorInfo.Description = __LemonResourceStringTableAdd(&stringTable,iter->Item.ErrorInfo.Description,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				iter->BinaryItem.ErrorInfo.Code = htonl(iter->Item.ErrorInfo.Code);

				break;
			}

		case LEMON_RESOURCE_TTRACECATALOG:
			{
				itemTableSize += sizeof(LemonResourceTraceCatalogBinary);

				iter->BinaryItem.TraceCatalog.Type = htonl(LEMON_RESOURCE_TTRACECATALOG);

				iter->BinaryItem.TraceCatalog.Name = __LemonResourceStringTableAdd(&stringTable,iter->Item.TraceCatalog.Name,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				iter->BinaryItem.TraceCatalog.Description = __LemonResourceStringTableAdd(&stringTable,iter->Item.TraceCatalog.Description,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				iter->BinaryItem.TraceCatalog.Value = htonl(iter->Item.TraceCatalog.Value);

				break;
			}

		case LEMON_RESOURCE_TTRACEEVENT:
			{
				itemTableSize += sizeof(LemonResourceTraceEventBinary);

				iter->BinaryItem.TraceEvent.Type = htonl(LEMON_RESOURCE_TTRACEEVENT);

				iter->BinaryItem.TraceEvent.Text = __LemonResourceStringTableAdd(&stringTable,iter->Item.TraceEvent.Text,errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				iter->BinaryItem.TraceEvent.Sequence = htonl(iter->Item.TraceEvent.Sequence);

				break;
			}
		}

		iter = iter->Next;
	}

	
	// write locale id 
	writer.Write(writer.UserData,(const lemon_byte_t*)&localeId,sizeof(localeId),errorCode);
	
	if(LEMON_FAILED(*errorCode)) goto Finally;;

	// write version 

	writer.Write(writer.UserData,(const lemon_byte_t*)&version,sizeof(version),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;;
	//write uuid
	writer.Write(writer.UserData,(const lemon_byte_t*)&resource->Uuid,sizeof(LemonUuid),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;;

	lemon_uint32_t stringTableSize = 0;

	if(stringTable.Tail)
	{
		stringTableSize = stringTable.Tail->Offset + (lemon_uint32_t)stringTable.Tail->Length;
	}

	stringTableSize = htonl(stringTableSize);

	//write string table length
	writer.Write(writer.UserData,(const lemon_byte_t*)&stringTableSize,sizeof(stringTableSize),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;;
	//write string table strings
	LemonResourceUTF8String *string = stringTable.Header;

	while(string)
	{
		writer.Write(writer.UserData,(const lemon_byte_t*)&string->String,string->Length,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Finally;;

		string = string->Next;
	}

	itemTableSize = htonl(itemTableSize);

	//write item table length
	writer.Write(writer.UserData,(const lemon_byte_t*)&itemTableSize,sizeof(itemTableSize),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;;

	iter = resource->Items;

	while(iter)
	{
		switch(iter->Type)
		{
		case LEMON_RESOURCE_TTEXT:
			{
				writer.Write(writer.UserData,(const lemon_byte_t*)&iter->BinaryItem.Text,sizeof(LemonResourceTextBinary),errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				break;
			}

		case LEMON_RESOURCE_TERRORINFO:
			{
				writer.Write(writer.UserData,(const lemon_byte_t*)&iter->BinaryItem.ErrorInfo,sizeof(LemonResourceErrorInfoBinary),errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				break;
			}

		case LEMON_RESOURCE_TTRACECATALOG:
			{
				writer.Write(writer.UserData,(const lemon_byte_t*)&iter->BinaryItem.TraceCatalog,sizeof(LemonResourceTraceCatalogBinary),errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				break;
			}

		case LEMON_RESOURCE_TTRACEEVENT:
			{
				writer.Write(writer.UserData,(const lemon_byte_t*)&iter->BinaryItem.TraceEvent,sizeof(LemonResourceTraceEventBinary),errorCode);

				if(LEMON_FAILED(*errorCode)) goto Finally;;

				break;
			}
		}

		iter = iter->Next;
	}

Finally:

	string = stringTable.Header;

	while(string)
	{
		LemonResourceUTF8String *current = string;

		string = string->Next;

		__LemonResourceReleaseUTF8String(current);
	}
}


//LEMON_SYS_API
//	LemonResource LemonResourceRead(
//	__lemon_in LemonIoReader reader,
//	__lemon_inout LemonErrorInfo *errorCode)
//{
//
//}
//} 


LEMON_SYS_API 
	const lemon_char_t * 
	LemonI18nText(
	__lemon_in const LemonUuid*,
	__lemon_in const lemon_char_t * msg)
{
	return msg;
}
