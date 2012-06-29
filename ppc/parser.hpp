/**
* 
* @file     parser
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/06/28
*/
#ifndef LEMON_PPC_PARSER_HPP
#define LEMON_PPC_PARSER_HPP

#include <vector>
#include <lemon/ppc/lexer.hpp>

namespace lemon{namespace ppc{namespace core{

	struct AST : private lemon::nocopyable
	{
		struct Metadata{std::string formatter; size_t lines; size_t args; };

		typedef std::vector<Metadata>	NodeListType;

		NodeListType	NodeList;

		void Add(const std::string & formatter,size_t lines, size_t args)
		{
			Metadata d = {formatter,lines,args};

			NodeList.push_back(d);
		}
	};

	class Parser : private lemon::nocopyable
	{
	public:

		Parser(const char * macroName);
		
		void Parse(Lexer & lexer,AST & ast);

	private:

		void ParseMacroArgs();

	private:

		std::string		_macro;

		Lexer			*_lexer;

		AST				*_ast;
	};

}}}

#endif //LEMON_PPC_PARSER_HPP