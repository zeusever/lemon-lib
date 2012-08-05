#include <sstream>
#include <algorithm>
#include <lemon/ppc/codegen.hpp>
#include <lemon/ppc/errorcode.h>
#include <lemon/ppc/package.hpp>
#include <lemonxx/function/bind.hpp>
namespace lemon{namespace ppc{namespace core{

	CXXCodeGen::CXXCodeGen(const Package &package,size_t moduleId)
		:_package(package),_packageId(package.Guid()),_macroname(package.TraceLogMacroName()),_moduleId(moduleId)
	{
		SuffixG();
	}

	void CXXCodeGen::Generate(std::ostream & stream,const AST & ast)
	{
		TitleG(stream);

		GuardHeaderG(stream);

		CatalogG(stream);

		PackageIdGetFunctionG(stream);

		ClassDeclareG(stream);

		std::for_each(ast.NodeList.begin(),ast.NodeList.end(),lemon::bind(&CXXCodeGen::PartialTemplateSpecialization,this,lemon::ref(stream),lemon::_0));

		MacroDefine(stream);

		GuardTailG(stream);
	}

	void CXXCodeGen::TitleG(std::ostream & stream)
	{
		stream << "/*" << std::endl;
		stream << " * this file is auto generate by lemon ppc do not modify it by hand" << std::endl;
		stream << " * @file     codegen"<< std::endl;
		stream << " * @brief    Copyright (C) 2012  yayanyang All Rights Reserved"<< std::endl;
		stream << " * @author   yayanyang"<< std::endl;
		stream << " * @version  1.0.0.0  "<< std::endl;
		stream << " */" << std::endl;
	}

	void CXXCodeGen::GuardHeaderG(std::ostream & stream)
	{
		stream << "#ifndef LEMON_PPC_CXX_GEN_" << _suffix  << std::endl;

		stream << "#define LEMON_PPC_CXX_GEN_" << _suffix  << std::endl << std::endl;

		stream << "#include <lemonxx/ppc/ppc.hpp>"  << std::endl << std::endl;
	}

	void CXXCodeGen::GuardTailG(std::ostream & stream)
	{
		stream << "#endif //LEMON_PPC_CXX_GEN_" << _suffix << std::endl << std::endl;
	}

	void CXXCodeGen::PackageIdGetFunctionG(std::ostream & stream)
	{
		const lemon_byte_t * bytes = (const lemon_byte_t*)&_moduleGuid;

		char buffer[128] = {0};

		_packageIdFunctionName = "__packageid_";
		
		_packageIdFunctionName += _suffix;

		lemon_sprintf(
			buffer,sizeof(buffer),"{0x%02X%02X%02X%02X,0x%02X%02X,0x%02X%02X,{0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X}}",
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15]
		);

		stream << "const LemonUuid * " << _packageIdFunctionName << "()" << std::endl;

		stream << "{\n\n";

		stream << "\tconst static LemonUuid id = " << buffer << ";\n\n" ;

		stream << "\treturn &id;\n\n";

		stream << "}\n\n";
	}

	void CXXCodeGen::CatalogG(std::ostream & stream)
	{
		const Package::Catalog & catalog  = _package.GetCatalog();

		size_t counter = 0;

		for(size_t i = 0 ; i < catalog.size(); ++i)
		{
			std::string flag = catalog[i].Text;

			if(!flag.empty())
			{
				stream << "#ifndef " << flag << std::endl;

				stream << "#define " << flag << "\t" << counter << std::endl;

				stream << "#endif //" << flag << "\n" << std::endl;

				++ counter;
			}
		}
	}

	void CXXCodeGen::ClassDeclareG(std::ostream & stream)
	{
		stream << "template<size_t lines> struct __lemon_ppc_tracelog_" << _suffix << ";" << std::endl << std::endl;
	}

	void CXXCodeGen::SuffixG()
	{
		const lemon_byte_t * bytes = (const lemon_byte_t*)&_moduleGuid;

		char buffer[128] = {0};

		lemon_sprintf(
			buffer,sizeof(buffer),"%02X%02X%02X%02X_%02X%02X_%02X%02X_%02X%02X_%02X%02X%02X%02X%02X%02X",
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15]
		);

		_suffix = buffer;
	}

	void CXXCodeGen::PartialTemplateSpecialization(std::ostream & stream,const AST::Metadata & data)
	{
		size_t args = data.args - 4;

		stream << "template<> struct __lemon_ppc_tracelog_" << _suffix << "<" << data.lines << ">" << std::endl;

		stream << "{" << std::endl << std::endl;

		stream << "\ttemplate<typename Tracelog";

		for(size_t i = 0; i < args; ++ i)
		{
			stream << ", " << "typename A" << i;
		}

		stream << ">" << std::endl;

		stream << "\tvoid operator()(TraceLog & log, size_t level, size_t catalog,const char*,";

		for(size_t i = 0; i < args; ++ i)
		{
			stream << ", " << "A" << i << " a" << i;
		}

		stream << ")\n\t{" <<  std::endl << std::endl;

		stream << "\t\tsize_t length = sizeof(LemonTraceMessage)";

		for(size_t i = 0; i < args; ++ i)
		{
			stream << " + " << "lemon::ppc::type_trait<A" << i << ">::length(a" << i << ")";
		}

		stream << ";" <<  std::endl << std::endl;

		stream << "\t\tLemonTraceMessage * message = log.create_message(" << _packageIdFunctionName << "(),level,catalog,length);" <<  std::endl << std::endl;

		stream << "\t\tif(!message) return;" <<  std::endl << std::endl;

		stream << "\t\tlength = 0;" <<  std::endl << std::endl;

		for(size_t i = 0; i < args; ++ i)
		{
			stream << "\t\tlength = lemon::ppc::type_trait<A" << i << ">::write(message,length,a" << i << ")" << std::endl << std::endl;
		}

		stream << "\t\tlog.trace(message);" << std::endl << std::endl;

		stream << "\t}" <<  std::endl << std::endl;

		stream << "};" << std::endl << std::endl;
	}

	void CXXCodeGen::MacroDefine(std::ostream & stream)
	{
		stream << "#define " << _macroname << "\t__lemon_ppc_tracelog_" <<  _suffix << "<__LINE__>()" << std::endl << std::endl;
	}
}}}