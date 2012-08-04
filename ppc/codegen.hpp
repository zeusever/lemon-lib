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

	class CXXCodeGen : private lemon::nocopyable
	{
	public:
		
		
		CXXCodeGen(const lemon::uuid_t & packageId,const char * macroname,const char * catalogs,size_t moduleId);

		void Generate(std::ostream & stream,const AST & ast);

	private:

		void TitleG(std::ostream & stream);

		void GuardHeaderG(std::ostream & stream);

		void GuardTailG(std::ostream & stream);

		void PackageIdGetFunctionG(std::ostream & stream);

		void CatalogG(std::ostream & stream);

		void ClassDeclareG(std::ostream & stream);

		void PartialTemplateSpecialization(std::ostream & stream,const AST::Metadata & data);

		void MacroDefine(std::ostream & stream);

		void SuffixG();

	private:

		const lemon::uuid_t				_packageId;

		const lemon::uuid_t				_moduleGuid;

		const std::string				_macroname;

		const std::string				_catalogs;

		std::string						_packageIdFunctionName;

		std::string						_suffix;

		size_t							_moduleId;

		
	};

}}}
#endif //LEMON_PPC_CODEDENG_HPP