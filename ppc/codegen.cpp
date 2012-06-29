#include <lemon/ppc/codegen.hpp>
#include <lemon/ppc/errorcode.h>
namespace lemon{namespace ppc{namespace core{

	void CxxCodeGen::Generate(const char * macroName,const char * tag,std::ostream & stream,AST & ast)
	{
		stream << "#include <lemonxx/mpl/inttypes.hpp>" << std::endl << std::endl;

		stream << "#include <lemonxx/ppc/ppc.hpp>" << std::endl << std::endl;

		stream << "template<size_t lines> struct " << tag << ";" << std::endl << std::endl;

		AST::NodeListType::const_iterator iter,end = ast.NodeList.end();

		for(iter = ast.NodeList.begin(); iter != end; ++ iter)
		{
			stream << "struct" << tag << "<" << iter->lines << ">" << std::endl;

			stream << "{" << std::endl;


			stream << "\ttemplate<typename Writer";

			for(size_t i = 0; i < iter->args - 4; ++ i)
			{
				stream << ",typename P" << i;
			}

			stream << ">" << std::endl;

			stream << "\tvoid operator()(Writer *writer,size_t level,const LemonPPCFlag & flag";

			for(size_t i = 0; i < iter->args - 4; ++ i)
			{
				stream << ",P" << i << " p" << i;
			}

			stream << ")" << std::endl;

			stream << "\t{" << std::endl;

			stream << "\t\tlemon::ppc::write_header(writer,level,flag," << iter->args - 4 << ");" << std::endl;

			for(size_t i = 0; i < iter->args - 4; ++ i)
			{
				stream << "\t\tlemon::ppc::write_arg(writer," << " p" << i << ");" << std::endl;
			}

			stream << "\t}" << std::endl;

			stream << "};" << std::endl;
		}

		stream << "#define " << macroName << "\t" << tag << "<__LINE__>()" << std::endl << std::endl;
	}

}}}