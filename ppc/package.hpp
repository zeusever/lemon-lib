/**
* 
* @file     package
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012
*/
#ifndef LEMON_PPC_PACKAGE_HPP
#define LEMON_PPC_PACKAGE_HPP
#include <vector>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>
#include <lemon/ppc/lexer.hpp>
#include <lemon/ppc/parser.hpp>
#include <lemon/ppc/codegen.hpp>

namespace lemon{namespace ppc{namespace core{

	struct CatalogItem
	{
		std::string			Text;

		std::string			Description;

		size_t				Value;
	};

	struct ErrorInfoMetadata
	{
		std::string			Name;

		std::string			Description;
	};

	class Package : private lemon::nocopyable
	{
	public:

		typedef std::vector<AST>				ASTs;

		typedef std::vector<CatalogItem>		Catalog;

		typedef std::vector<ErrorInfoMetadata>	ErrorInfos;
		
		Package(const char * assemblyFile);

		void Scan(const lemon::String & source,const lemon::String object);

		void Generate(const lemon::String & object);

		const ASTs & GetASTs() const { return _asts; }

		const Catalog & GetCatalog() const { return _catalog; }

		const lemon::uuid_t	&Guid() const { return _guid; }

		const std::string& TraceLogMacroName() const { return _traceLogMacroName; }

		const lemon::String& GetName() const { return _name ; }

	private:

		void ParseLuaFile(const char *assemblyFile);

		void Name(const char *name);

		void SetGuid(const char *guidString);

		void PushErrorInfo(const char *name,const char *description);

		void SetTraceLogMacroName(const std::string & val) { _traceLogMacroName = val; }

		void PushCatalogItem(const char * name,const char * description,size_t id);

	private:

		ASTs							_asts;

		Catalog							_catalog;

		ErrorInfos						_errorinfos;

		lemon::uuid_t					_guid;

		std::string						_traceLogMacroName;

		lemon::String					_name;
	};

}}}
#endif // LEMON_PPC_PACKAGE_HPP
