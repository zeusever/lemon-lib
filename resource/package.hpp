/**
 * 
 * @file     resources
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/17
 */
#ifndef LEMON_RESOURCE_RESOURCE_HPP
#define LEMON_RESOURCE_RESOURCE_HPP

#include <map>
#include <vector>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>
#include <lemon/resource/stringtable.hpp>
#include <lemon/resource/reader.hpp>
#include <lemon/resource/writer.hpp>

namespace lemon{namespace resource{

	struct ErrorMessage { lemon::uint32_t Code; lemon::uint32_t Name; lemon::uint32_t Description; };

	struct TraceCatalog { lemon::uint32_t Value; lemon::uint32_t Name; lemon::uint32_t Description; };

	struct TraceMessage { lemon::uint32_t Id; lemon::uint32_t Message;};

	struct I18nText {lemon::uint32_t key; lemon::uint32_t Message; };

	class ResourceTable 
	{
	public:

		typedef std::map<lemon::uint32_t,ErrorMessage>		ErrorMessageTable;

		typedef std::map<lemon::uint32_t,TraceCatalog>		TraceCatalogTable;

		typedef std::map<lemon::uint32_t,TraceMessage>		TraceMessageTable;

		typedef std::map<lemon::String,I18nText>			TextTable;

		void NewErrorMessage(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description);

		void NewTraceCatalog(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description);

		void NewTraceMessage(lemon::uint32_t id,lemon::char_t const * message);

		void NewText(lemon::char_t const * key,lemon::char_t const * message);

		const ErrorMessage * GetErrorMessage(lemon::uint32_t code) const;

		const TraceCatalog * GetTraceCatalog(lemon::uint32_t code) const;

		const TraceMessage * GetTraceMessage(lemon::uint32_t code) const;

		const lemon::char_t * GetText(lemon::char_t const *  key) const;

		const lemon::char_t * String(lemon::uint32_t id) const ;

	public:

		void Write(const lemon::String & locale,IWriter & writer) const;

		void Read(IReader & reader);

	private:

		mutable ErrorMessageTable			_errorMessageTable;

		mutable TraceCatalogTable			_traceCatalogTable;

		mutable TraceMessageTable			_traceMessageTable;

		mutable TextTable					_textTable;

		StringTable							_stringTable;
	};


	class Package : private lemon::nocopyable
	{
	public:

		typedef std::map<lemon::String,ResourceTable>		LocaleResourceTables;

		void NewErrorMessage(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description,lemon::char_t const * locale);

		void NewTraceCatalog(lemon::uint32_t code,lemon::char_t const * name, lemon::char_t const * description,lemon::char_t const * locale);

		void NewTraceMessage(lemon::uint32_t id,lemon::char_t const * message,lemon::char_t const * locale);

		void NewText(lemon::char_t const * key,lemon::char_t const * message,lemon::char_t const * locale);

		LemonResourceInfo GetErrorMessage(lemon::uint32_t code,lemon::char_t const * locale) const;

		LemonResourceInfo GetTraceCatalog(lemon::uint32_t code,lemon::char_t const * locale) const;

		const lemon::char_t * GetTraceMessage(lemon::uint32_t code,lemon::char_t const * locale) const;

		const lemon::char_t * GetText(lemon::char_t const *  key,lemon::char_t const * locale) const;

	public:

		const LemonUuid * Uuid() const { return &_uuid; }

		const LemonVersion * Version() const  { return &_version;}

		void Uuid(const LemonUuid * val) { _uuid = *val; }

		void Version(const lemon::char_t * version);

	public:

		void Write(IWriter & writer) const;

		void Read(IReader & reader);
	public:

		lemon::uuid_t							_uuid;

		LemonVersion							_version;

		LocaleResourceTables					_resourceTables;
	};
}}

#endif // LEMON_RESOURCE_RESOURCE_HPP

