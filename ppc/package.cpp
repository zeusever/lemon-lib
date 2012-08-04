#include <lemon/ppc/package.hpp>



namespace lemon{namespace ppc{namespace core{

	Package::Package(const lemon::String & name,const std::string & macroname,const std::string & catalogs,const lemon::uuid_t & id)
		:_name(name),_macroname(macroname),_catalogs(catalogs),_id(id)
	{

	}

	void Package::Scan(const lemon::String & source,const lemon::String object)
	{
		Lexer lexer(source.c_str());

		Parser parser(_macroname.c_str());

		_asts.push_back(AST());

		AST & ast = _asts.back();

		parser.Parse(lexer,ast);

		CXXCodeGen gen(_id,_macroname.c_str(),_catalogs.c_str(),_asts.size());
	}

	void Package::Generate(const lemon::String & /*object*/)
	{

	}

}}}