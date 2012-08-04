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

	class Package : private lemon::nocopyable
	{
	public:

		typedef std::vector<AST>			ASTs;
		
		Package(const lemon::String & name,const std::string & macroname,const std::string & catalogs,const lemon::uuid_t & id);

		void Scan(const lemon::String & source,const lemon::String object);

		void Generate(const lemon::String & object);

	private:

		const lemon::String				_name;

		const std::string				_macroname;

		const std::string				_catalogs;

		const lemon::uuid_t				_id;

		ASTs							_asts;
	};

}}}
#endif // LEMON_PPC_PACKAGE_HPP
