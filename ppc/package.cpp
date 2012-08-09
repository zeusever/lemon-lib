#include <iomanip>
#include <sstream>
#include <lemon/ppc/errorcode.h>
#include <lemon/ppc/package.hpp>
#include <lemonxx/luabind/luabind.hpp>


namespace lemon{namespace ppc{namespace core{

	Package::Package(const char * assemblyFile)
	{
		ParseLuaFile(assemblyFile);
	}

	void Package::Scan(const lemon::String & source,const lemon::String object)
	{
		Lexer lexer(source.c_str());

		Parser parser(_traceLogMacroName.c_str());

		_asts.push_back(AST());

		AST & ast = _asts.back();

		parser.Parse(lexer,ast);

		if(ast.Size())
		{
			CXXCodeGen gen(*this,_asts.size());

			std::ofstream stream(lemon::to_locale(object).c_str(),std::ios::trunc);

			if(!stream.is_open())
			{
				error_info errorCode;

				LEMON_USER_ERROR(errorCode,LEMON_PPC_OPENT_OBJECT_CXX_FILE_ERROR);

				errorCode.check_throw();
			}

			gen.Generate(stream,ast);
		}
	}

	void Package::Generate(const lemon::String & /*object*/)
	{

	}

	void Package::ParseLuaFile(const char *assemblyFile)
	{
		error_info errorCode;

		luabind::lua_state L;

		luabind::dofile(L,assemblyFile);

		luabind::module(L,"lemon")
			<= luabind::class_<Package>("package")
			.def("uuid",&Package::SetGuid)	
			.def("name",&Package::Name)	
			.def("errorinfo",&Package::PushErrorInfo)
			.def("tracelog",&Package::SetTraceLogMacroName)
			.def("catalog",&Package::PushCatalogItem)
			;

		std::stringstream stream;

		stream << "function parse(pkg)" << std::endl;

		stream << "\tprint(pkg)" << std::endl;

		stream << "\tpkg:uuid(assembly.guid)" << std::endl;

		stream << "\tpkg:name(assembly.name)" << std::endl;

		stream << "\tpkg:tracelog(assembly.tracelog.macro)" << std::endl;

		stream << "\t for i,v in ipairs(assembly.errorcode) do " << std::endl;

		stream << "\t\tif v.description ~= nil then pkg:errorinfo(v.name,v.description) else pkg:errorinfo(v.name,\"\") end" << std::endl;

		stream << "end" <<std::endl;

		stream << "\t for i,v in ipairs(assembly.tracelog.catalog) do " << std::endl;

		stream << "\t\tif v.description ~= nil then pkg:catalog(v.name,v.description,i) else pkg:catalog(v.name,\"\",i) end" << std::endl;

		stream << "end" <<std::endl;

		stream << "end" << std::endl;

		luabind::dostring(L,stream.str().c_str());

		luabind::call<void>(L,"parse",this);
	}

	void Package::SetGuid(const char * guidString)
	{
		int D[11]  = {0};

#ifdef _MSC_VER
		if(11 != sscanf_s(
			guidString,
			"{ %x, %x, %x, { %x, %x, %x, %x, %x, %x, %x, %x } }",
			D,D+1,D+2,D+3,D+4,D+5,D+6,D+7,D+8,D+9,D+10
			))
#else
		if(11 != sscanf(
			guidString,
			"{ %x, %x, %x, { %x, %x, %x, %x, %x, %x, %x, %x } }",
			D,D+1,D+2,D+3,D+4,D+5,D+6,D+7,D+8,D+9,D+10
			))
#endif //_MSC_VER
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_PPC_INVALID_ASSEMBLYINFO_LUA_FILE);

			errorCode.error_msg(LEMON_TEXT("assembly.guid format error,expect : { %x, %x, %x, { %x, %x, %x, %x, %x, %x, %x, %x } }"));

			errorCode.check_throw();
		}

		_guid.Data1 = (lemon_uint32_t)D[0];

		_guid.Data2 = (lemon_uint16_t)D[1];

		_guid.Data3 = (lemon_uint16_t)D[2];

		for(size_t i = 0; i < 8; ++i)
		{
			_guid.Data4[i] = (lemon_uint8_t)D[3 + i];
		}
	}

	void Package::PushErrorInfo(const char *name,const char *description)
	{
		ErrorInfoMetadata metadata = {name,description};

		_errorinfos.push_back(metadata);
	}

	void Package::Name(const char *name)
	{
		_name = lemon::from_utf8(name);
	}

	void Package::PushCatalogItem(const char * name,const char * description,size_t id)
	{
		CatalogItem item = {name,description,id};

		_catalog.push_back(item);
	}
}}}