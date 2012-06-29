#include <sstream>
#include <lemon/ppc/parser.hpp>
#include <lemon/ppc/errorcode.h>

namespace lemon{namespace ppc{namespace core{

	Parser::Parser(const char * macroName)
		:_macro(macroName)
	{

	}

	void Parser::Parse(Lexer & lexer,AST & ast)
	{
		_lexer = &lexer;

		_ast = &ast;

		Token token;

		while(_lexer->Next(token))
		{
			if(token.Type == Lexer::TOKEN_ID && token.Value == _macro)
			{
				ParseMacroArgs();
			}
		}
	}

	void Parser::ParseMacroArgs()
	{
		Token token;

		size_t lines = _lexer->Line();

		bool expectNext = true;

		size_t args = 0;

		std::string formatter;

		_lexer->Next(token);

		if(token.Type != '(') 
		{
			LEMON_DECLARE_ERRORINFO(errorCode);

			LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_MACRO_ERROR);

			throw lemon::Exception(errorCode);
		}

		while(_lexer->Next(token))
		{
			if(token.Type == '(')
			{
				expectNext = false;
			}
			else if(token.Type == ',' && expectNext)
			{
				++ args;
			}
			else if(token.Type == ')')
			{
				if(expectNext)
				{
					++ args;

					if(args < 4)
					{
						LEMON_DECLARE_ERRORINFO(errorCode);

						LEMON_USER_ERROR(errorCode,LEMON_PPC_EXPECT_FORMATTER_STRING);

						throw lemon::Exception(errorCode);
					}

					_ast->Add(formatter,lines,args);

					return;
				}

				expectNext = true;
			}
			else if(args == 3)
			{
				if(token.Type != Lexer::TOKEN_LIT_TEXT || !expectNext)
				{
					LEMON_DECLARE_ERRORINFO(errorCode);

					LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_MACRO_ERROR);

					throw lemon::Exception(errorCode);
				}

				formatter = token.Value;
				
				if(!_lexer->Next(token) || (token.Type != ',' && token.Type != ')'))
				{
					LEMON_DECLARE_ERRORINFO(errorCode);

					LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_MACRO_ERROR);

					throw lemon::Exception(errorCode);
				}

				++ args;

				if(token.Type == ')')
				{
					_ast->Add(formatter,lines,args);

					return;
				}
			}

		}

		LEMON_DECLARE_ERRORINFO(errorCode);

		LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_MACRO_ERROR);

		throw lemon::Exception(errorCode);
	}

}}}