/**
* 
* @file     codegen
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/06/28
*/
#ifndef LEMON_PPC_CODEDENG_HPP
#define LEMON_PPC_CODEDENG_HPP
#include <ostream>
#include <lemon/ppc/parser.hpp>

namespace lemon{namespace ppc{namespace core{

	class CxxCodeGen : private lemon::nocopyable
	{
	public:

		CxxCodeGen(const char * macroName,const char * assemblyGuid);

		void Generate(size_t moduleId,const char * tag,std::ostream & stream,AST & ast);

	};

}}}
#endif //LEMON_PPC_CODEDENG_HPP