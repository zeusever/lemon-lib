#include <lemon/resource/assembly.h>
#include <lemon/resource/package.hpp>


namespace lemon{namespace resource{

#ifdef WIN32

	inline const_buffer __cbuf(lemon::char_t const * source)
	{
		if(source == NULL)
		{
			return lemon::const_buffer((const lemon::byte_t*)source,0);
		}

		return lemon::const_buffer((const lemon::byte_t*)source,(lemon_strlen(source) + 1) * 2);
	}

#endif //

	inline const_buffer __cbuf(char const * source)
	{
		if(source == NULL)
		{
			return lemon::const_buffer((const lemon::byte_t*)source,0);
		}

		return lemon::const_buffer((const lemon::byte_t*)source,strlen(source) + 1);
	}


	void ResourceTable::NewErrorMessage(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description)
	{
		if(_errorMessageTable.find(code) != _errorMessageTable.end())
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_RESOURCE_MULTI_RESOURCE_ITEM_ERROR);

			errorCode.check_throw();
		}

		ErrorMessage em = {code,0,0};

		em.Name = _stringTable.Push(__cbuf(name));

		em.Description = _stringTable.Push(__cbuf(description));

		_errorMessageTable[code] = em;
	}

	void ResourceTable::NewTraceCatalog(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description)
	{
		if(_traceCatalogTable.find(code) != _traceCatalogTable.end())
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_RESOURCE_MULTI_RESOURCE_ITEM_ERROR);

			errorCode.check_throw();
		}

		TraceCatalog em = {code,0,0};

		em.Name = _stringTable.Push(__cbuf(name));

		em.Description = _stringTable.Push(__cbuf(description));

		_traceCatalogTable[code] = em;
	}

	void ResourceTable::NewTraceMessage(lemon::uint32_t id,lemon::char_t const * message)
	{
		if(_traceMessageTable.find(id) != _traceMessageTable.end())
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_RESOURCE_MULTI_RESOURCE_ITEM_ERROR);

			errorCode.check_throw();
		}

		TraceMessage em = {id,0};

		em.Message = _stringTable.Push(__cbuf(message));

		_traceMessageTable[id] = em;
	}

	void ResourceTable::NewText(lemon::char_t const * key,lemon::char_t const * message)
	{
		if(_textTable.find(key) != _textTable.end())
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_RESOURCE_MULTI_RESOURCE_ITEM_ERROR);

			errorCode.check_throw();
		}

		I18nText text = {0};

		text.key = _stringTable.Push(__cbuf(key));

		text.Message = _stringTable.Push(__cbuf(message));

		_textTable[key] = text;
	}


	const ErrorMessage * ResourceTable::GetErrorMessage(lemon::uint32_t code) const
	{
		if(_errorMessageTable.find(code) == _errorMessageTable.end()) return NULL;
		
		return &_errorMessageTable[code];
	}

	const TraceCatalog * ResourceTable::GetTraceCatalog(lemon::uint32_t code) const
	{
		if(_traceCatalogTable.find(code) == _traceCatalogTable.end()) return NULL;

		return &_traceCatalogTable[code];
	}

	const TraceMessage * ResourceTable::GetTraceMessage(lemon::uint32_t code) const
	{
		if(_traceMessageTable.find(code) == _traceMessageTable.end()) return NULL;

		return &_traceMessageTable[code];
	}

	const lemon::char_t * ResourceTable::GetText(lemon::char_t const *  key) const
	{
		if(_textTable.find(key) == _textTable.end()) return NULL;

		return String(_textTable[key].Message);
	}

	const lemon::char_t * ResourceTable::String(lemon::uint32_t id) const 
	{
		lemon::tuple<const lemon::byte_t*,size_t> result = _stringTable.Get(id);

		return (const lemon::char_t*)get<0>(result);
	}


	//////////////////////////////////////////////////////////////////////////


	void Package::NewErrorMessage(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description,lemon::char_t const * locale)
	{
		_resourceTables[locale].NewErrorMessage(code,name,description);
	}

	void Package::NewTraceCatalog(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description,lemon::char_t const * locale)
	{
		_resourceTables[locale].NewTraceCatalog(code,name,description);
	}

	void Package::NewTraceMessage(lemon::uint32_t id,lemon::char_t const * message,lemon::char_t const * locale)
	{
		_resourceTables[locale].NewTraceMessage(id,message);
	}

	void Package::NewText(lemon::char_t const * key,lemon::char_t const * message,lemon::char_t const * locale)
	{
		_resourceTables[locale].NewText(key,message);
	}

	LemonResourceInfo  Package::GetErrorMessage(lemon::uint32_t code,lemon::char_t const * locale) const
	{
		LemonResourceInfo result = {NULL,NULL};

		LocaleResourceTables::const_iterator iter = _resourceTables.find(locale);

		if(iter == _resourceTables.end()) return result;

		const ErrorMessage * em = iter->second.GetErrorMessage(code);

		if(NULL == em) return result;

		result.Name = iter->second.String(em->Name);

		result.Description = iter->second.String(em->Description);

		return result;
	}

	LemonResourceInfo Package::GetTraceCatalog(lemon::uint32_t code,lemon::char_t const * locale) const
	{
		LemonResourceInfo result = {NULL,NULL};

		LocaleResourceTables::const_iterator iter = _resourceTables.find(locale);

		if(iter == _resourceTables.end()) return result;

		const TraceCatalog * em = iter->second.GetTraceCatalog(code);

		result.Name = iter->second.String(em->Name);

		result.Description = iter->second.String(em->Description);

		return result;
	}

	const lemon::char_t * Package::GetTraceMessage(lemon::uint32_t code,lemon::char_t const * locale) const
	{
		LocaleResourceTables::const_iterator iter = _resourceTables.find(locale);

		if(iter == _resourceTables.end()) return NULL;

		const TraceMessage * traceMessage = iter->second.GetTraceMessage(code);

		if(NULL == traceMessage) return NULL;
		
		return iter->second.String(traceMessage->Message);
	}

	const lemon::char_t * Package::GetText(lemon::char_t const *  key,lemon::char_t const * locale) const
	{
		LocaleResourceTables::const_iterator iter = _resourceTables.find(locale);

		if(iter == _resourceTables.end()) return NULL;

		return iter->second.GetText(key);
	}

	//////////////////////////////////////////////////////////////////////////

	static lemon_uint16_t __atoi(const lemon_char_t * from,const lemon_char_t * to)
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

	void Package::Version(const lemon::char_t * version)
	{
		lemon::String versionString;

		error_info errorCode;

		size_t counter = 0;

		lemon::localtime_t datetime(lemon::time_t::now());

		struct {const lemon_char_t *begin;const lemon_char_t * end;} nodes[4];

		const lemon_char_t * iter = version;

		nodes[0].begin = version;

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
				_version.Major = __atoi(nodes[0].begin,nodes[0].end);

				if(_version.Major == (lemon_uint16_t)-1) goto Error;

				_version.Minor = __atoi(nodes[1].begin,nodes[1].end);

				if(_version.Minor == (lemon_uint16_t)-1) goto Error;

				if(nodes[2].end - nodes[2].begin != 1 || *nodes[2].begin != LEMON_TEXT('*')) goto Error;

				_version.Build = (lemon_uint16_t)(datetime.DayOfYear);

				_version.Reversion = (lemon_uint16_t)((datetime.Hour * 3600 + datetime.Minute * 60 + datetime.Second)/2);

				break;
			}
		case 4:
			{
				_version.Major = __atoi(nodes[0].begin,nodes[0].end);

				if(_version.Major == (lemon_uint16_t)-1) goto Error;

				_version.Minor = __atoi(nodes[1].begin,nodes[1].end);

				if(_version.Minor == (lemon_uint16_t)-1) goto Error;

				_version.Build = __atoi(nodes[2].begin,nodes[2].end);

				if(_version.Build == (lemon_uint16_t)-1) goto Error;

				if(nodes[3].end - nodes[3].begin == 1 && *nodes[3].begin == LEMON_TEXT('*'))
				{
					_version.Reversion = (lemon_uint16_t)((datetime.Hour * 3600 + datetime.Minute * 60 + datetime.Second)/2);
				}
				else
				{
					_version.Reversion = __atoi(nodes[3].begin,nodes[3].end);

					if(_version.Reversion == (lemon_uint16_t)-1) goto Error;	
				}

				break;
			}
		default:
			{
				goto Error;
			}

		}

		return;

Error:

		LEMON_USER_ERROR(errorCode,LEMON_RESOURCE_INVALID_VERSION_STRING);

		errorCode.check_throw();
	}


	//////////////////////////////////////////////////////////////////////////


	void Package::Write(IWriter & writer) const
	{
		//write package id

		writer.Write((const lemon::byte_t*)&_uuid,sizeof(_uuid));

		lemon::uint16_t major = htons(_version.Major);

		lemon::uint16_t minor = htons(_version.Minor);

		lemon::uint16_t build = htons(_version.Build);

		lemon::uint16_t reversion = htons(_version.Reversion);

		writer.Write((const lemon::byte_t*)&major,sizeof(major));

		writer.Write((const lemon::byte_t*)&minor,sizeof(minor));

		writer.Write((const lemon::byte_t*)&build,sizeof(build));

		writer.Write((const lemon::byte_t*)&reversion,sizeof(reversion));

		lemon::uint8_t tables = (lemon::uint8_t)_resourceTables.size();

		writer.Write((const lemon::byte_t*)&tables,sizeof(tables));

		LocaleResourceTables::const_iterator iter ,end = _resourceTables.end();

		for(iter = _resourceTables.begin(); iter != end; ++ iter)
		{
			iter->second.Write(iter->first,writer);
		}
	}

	void ResourceTable::Write(const lemon::String & locale,IWriter & writer) const
	{
		std::string utf8 = lemon::to_utf8(locale);

		char localeString[5] = {0};

		memset(localeString,0,5);

		size_t localeStringLength = utf8.size();

		if(utf8.size() > 5) localeStringLength = 5;

		memcpy(localeString,utf8.c_str(),localeStringLength);

		writer.Write((const lemon::byte_t*)&localeString,5);

		//i18n -> l10n text table size

		lemon::uint16_t i18nTextTableSize = htons((lemon::uint16_t)_textTable.size());

		writer.Write((const lemon::byte_t*)&i18nTextTableSize,sizeof(i18nTextTableSize));

		//trace message table size

		lemon::uint16_t traceMessageTableSize = htons((lemon::uint16_t)_traceMessageTable.size());

		writer.Write((const lemon::byte_t*)&traceMessageTableSize,sizeof(traceMessageTableSize));

		//trace catalog table size

		lemon::uint16_t traceCatalogTableSize = htons((lemon::uint16_t)_traceCatalogTable.size());

		writer.Write((const lemon::byte_t*)&traceCatalogTableSize,sizeof(traceCatalogTableSize));

		//trace catalog table size

		lemon::uint16_t errorMessageTableSize = htons((lemon::uint16_t)_errorMessageTable.size());

		writer.Write((const lemon::byte_t*)&errorMessageTableSize,sizeof(errorMessageTableSize));

		// string table size

		lemon::uint16_t stringTableSize = htons((lemon::uint16_t)_stringTable.Items());

		writer.Write((const lemon::byte_t*)&stringTableSize,sizeof(stringTableSize));

		//i18n -> l10n text table 
		{
			TextTable::const_iterator iter,end = _textTable.end();

			for(iter = _textTable.begin() ; iter != end; ++ iter)
			{
				const I18nText & text = iter->second;

				lemon::uint32_t index = htonl(text.key);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));

				index = htonl(text.Message);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));
			}
		}

		//trace message table size

		{
			TraceMessageTable::const_iterator iter,end = _traceMessageTable.end();

			for(iter = _traceMessageTable.begin() ; iter != end; ++ iter)
			{
				const TraceMessage & text = iter->second;

				lemon::uint32_t index = htonl(text.Id);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));

				index = htonl(text.Message);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));
			}
		}
		//trace catalog table size

		{
			TraceCatalogTable::const_iterator iter,end = _traceCatalogTable.end();

			for(iter = _traceCatalogTable.begin() ; iter != end; ++ iter)
			{
				const TraceCatalog & text = iter->second;

				lemon::uint32_t index = htonl(text.Value);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));

				index = htonl(text.Name);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));

				index = htonl(text.Description);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));
			}
		}

		// error message

		{
			ErrorMessageTable::const_iterator iter,end = _errorMessageTable.end();

			for(iter = _errorMessageTable.begin() ; iter != end; ++ iter)
			{
				const ErrorMessage & text = iter->second;

				lemon::uint32_t index = htonl(text.Code);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));

				index = htonl(text.Name);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));

				index = htonl(text.Description);

				writer.Write((const lemon::byte_t*)&index,sizeof(index));
			}
		}

		// string table

		StringTable table;

		for(size_t i = 0 ; i < _stringTable.Items(); ++ i)
		{
			tuple<const lemon::byte_t*,size_t> result = _stringTable.Get((lemon::uint32_t)i);

			std::string utf8 = lemon::to_utf8((const char_t*)get<0>(result));

			table.Push(__cbuf(utf8.c_str()));
		}

		table.Write(writer);
	}

	void Package::Read(IReader & reader)
	{
		reader.Read((byte_t*)&_uuid,sizeof(_uuid));

		reader.Read((byte_t*)&_version.Major,sizeof(lemon::uint16_t));
		reader.Read((byte_t*)&_version.Minor,sizeof(lemon::uint16_t));
		reader.Read((byte_t*)&_version.Build,sizeof(lemon::uint16_t));
		reader.Read((byte_t*)&_version.Reversion,sizeof(lemon::uint16_t));

		_version.Major = ntohs(_version.Major);

		_version.Minor = ntohs(_version.Minor);

		_version.Build = ntohs(_version.Build);

		_version.Reversion = ntohs(_version.Reversion);

		lemon::uint8_t tables;

		reader.Read(&tables,sizeof(tables));

		for(size_t i = 0; i < tables; ++  i)
		{
			char locale[6] = {0};

			reader.Read((byte_t*)locale,5);

			ResourceTable & table =_resourceTables[lemon::from_utf8(locale)];

			table.Read(reader);
		}
	}

	void ResourceTable::Read(IReader & reader)
	{
		lemon::uint16_t i18nTextTableSize = 0;

		lemon::uint16_t traceMessageTableSize = 0;

		lemon::uint16_t traceCatalogTableSize = 0;

		lemon::uint16_t errorMessageTableSize = 0;

		lemon::uint16_t stringTableSize = 0;

		reader.Read((byte_t*)&i18nTextTableSize,sizeof(i18nTextTableSize));

		reader.Read((byte_t*)&traceMessageTableSize,sizeof(traceMessageTableSize));

		reader.Read((byte_t*)&traceCatalogTableSize,sizeof(traceCatalogTableSize));

		reader.Read((byte_t*)&errorMessageTableSize,sizeof(errorMessageTableSize));

		reader.Read((byte_t*)&stringTableSize,sizeof(stringTableSize));

		i18nTextTableSize = htons(i18nTextTableSize);

		traceMessageTableSize = htons(traceMessageTableSize);

		traceCatalogTableSize = htons(traceCatalogTableSize);

		errorMessageTableSize = htons(errorMessageTableSize);

		stringTableSize = htons(stringTableSize);

		std::vector<I18nText> texts;

		for(size_t i = 0; i < i18nTextTableSize; ++ i)
		{
			I18nText text;

			reader.Read((byte_t*)&text.key,sizeof(text.key));

			reader.Read((byte_t*)&text.Message,sizeof(text.Message));

			text.key = ntohl(text.key);

			text.Message = ntohl(text.Message);

			texts.push_back(text);
		}


		for(size_t i = 0; i < traceMessageTableSize; ++ i)
		{
			TraceMessage text;

			reader.Read((byte_t*)&text.Id,sizeof(text.Id));

			reader.Read((byte_t*)&text.Message,sizeof(text.Message));

			text.Id = ntohl(text.Id);

			text.Message = ntohl(text.Message);

			_traceMessageTable[text.Id] = text;
		}


		for(size_t i = 0; i < traceCatalogTableSize; ++ i)
		{
			TraceCatalog text;

			reader.Read((byte_t*)&text.Value,sizeof(text.Value));

			reader.Read((byte_t*)&text.Name,sizeof(text.Name));

			reader.Read((byte_t*)&text.Description,sizeof(text.Description));

			text.Value = ntohl(text.Value);

			text.Name = ntohl(text.Name);

			text.Description = ntohl(text.Description);

			_traceCatalogTable[text.Value] = text;
		}

		for(size_t i = 0; i < errorMessageTableSize; ++ i)
		{
			ErrorMessage text;

			reader.Read((byte_t*)&text.Code,sizeof(text.Code));

			reader.Read((byte_t*)&text.Name,sizeof(text.Name));

			reader.Read((byte_t*)&text.Description,sizeof(text.Description));

			text.Code = ntohl(text.Code);

			text.Name = ntohl(text.Name);

			text.Description = ntohl(text.Description);

			_errorMessageTable[text.Code] = text;
		}

		StringTable table;

		table.Read(stringTableSize, reader);

		for(size_t i = 0 ; i < table.Items(); ++ i)
		{
			tuple<const lemon::byte_t*,size_t> result = table.Get((lemon::uint32_t)i);

			lemon::String val = lemon::from_utf8((const char*)get<0>(result));

			_stringTable.Push(__cbuf(val.c_str()));
		}

		for(size_t i = 0; i < i18nTextTableSize; ++ i)
		{
			_textTable[(const char_t*)get<0>(_stringTable.Get(texts[i].key))] = texts[i];
		}
	}
}}