#include <locale>
#include <fstream>
#include <lemon/ppc/lexer.hpp>
#include <lemon/ppc/errorcode.h>
#include <lemon/sys/errorcode.h>

namespace lemon{namespace ppc{namespace core{

	void Lexer::ReloadFile(const lemon_char_t * fileName)
	{
		LEMON_DECLARE_ERRORINFO(errorCode);

#ifdef WIN32
		std::ifstream stream(fileName);
#else
		std::ifstream stream(lemon::to_locale(fileName));
#endif //WIN32

		if(!stream.is_open())
		{
			LEMON_USER_ERROR(errorCode,LEMON_SYS_RESOURCE_ERROR);

			throw lemon::Exception(errorCode);
		}


		stream.seekg(0,std::ios::end);

		_buffer.resize(stream.tellg());

		if(_buffer.empty())
		{
			LEMON_USER_ERROR(errorCode,LEMON_PPC_FILE_IS_EMPTY);

			throw lemon::Exception(errorCode);
		}

		stream.seekg(0);

		stream.read(&_buffer[0],_buffer.size());

		_lines = 1;

		_columns = 1;

		_iterator = _buffer.begin();
	}

	void Lexer::ReloadString(const char * message)
	{
		_buffer.resize(strlen(message));

		memcpy(&_buffer[0],message,strlen(message));

		_lines = 1;

		_columns = 1;

		_iterator = _buffer.begin();
	}

	bool Lexer::Next(Token & token)
	{
		//LEMON_DECLARE_ERRORINFO(errorCode);

		if(_iterator == _buffer.end()) return false;

		for(;_buffer.end() != _iterator;)
		{
			switch(*_iterator)
			{
			case '#':	{ ReadPreprocess(token); return true; }

			case '/':	{ ReadComment(token); return true; }

			case '\r':
			case '\n': { ReadNewLine(); continue; }

			case '\'': 
			case '\"':	{ ReadLitText(token); return true; }

			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': 
				{
					ReadNumber(token); return true;
				}
			default:
				{
					if(isalpha(*_iterator) || '_' == *_iterator)
					{
						ReadID(token); return true;
					}
					else if(isspace(*_iterator))
					{
						NextChar();

						continue;
					}
					else 
					{
						token.Type = *_iterator;

						NextChar();

						return true;
					}
				}
			};
		}

		return false;
	}

	bool Lexer::IsNewLine()
	{
		return ('\r' == *_iterator || '\n' == *_iterator);
	}

	void Lexer::ReadNewLine()
	{
		char current = *_iterator;

		if(NextChar())
		{
			if(IsNewLine() && current != *_iterator) NextChar();
		}

		++ _lines;

		_columns = 1;
	}

	void Lexer::ReadPreprocess(Token & token)
	{
		buffer_type::const_iterator begin = _iterator;

		for(;;)
		{
			if(IsNewLine())
			{
				token.Type = TOKEN_PREPROCESS;

				token.Value = std::string(begin,_iterator);

				ReadNewLine();

				return;
			}

			if(!NextChar()) break;
		} 

		token.Type = TOKEN_PREPROCESS;

		token.Value = std::string(begin,_iterator);
	}

	void Lexer::ReadComment(Token & token)
	{
		LEMON_DECLARE_ERRORINFO(errorCode);

		bool cxxComment = false;

		if(!NextChar())
		{
			LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_LEX_ERROR);

			throw lemon::Exception(errorCode);
		}

		if('/' == *_iterator) cxxComment = true;

		++ _iterator;

		bool finish = false;

		buffer_type::const_iterator begin = _iterator;

		for(;;)
		{
			if(IsNewLine())
			{

				if(cxxComment)
				{
					token.Type = TOKEN_COMMENT;

					token.Value = std::string(begin,_iterator);

					ReadNewLine();

					return;
				}

				ReadNewLine();

				continue;
			}
			
			if(!cxxComment && '*' == *_iterator)
			{
				finish = true;
			}
			else if(!cxxComment && finish && '/' == *_iterator)
			{
				token.Type = TOKEN_COMMENT;

				token.Value = std::string(begin,_iterator - 1);

				NextChar();

				return;
			}
			else
			{
				finish = false;
			}

			if(!NextChar()) break;
		}

		if(cxxComment)
		{
			token.Type = TOKEN_COMMENT;

			token.Value = std::string(begin,_iterator);

			return;
		}
		else
		{
			LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_LEX_ERROR);

			throw lemon::Exception(errorCode);
		}
	}

	void Lexer::ReadLitText(Token & token)
	{
		LEMON_DECLARE_ERRORINFO(errorCode);

		char pattern = *_iterator;

		if(!NextChar())
		{
			LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_LEX_ERROR);

			throw lemon::Exception(errorCode);
		}

		buffer_type::const_iterator begin = _iterator;

		bool escape = false;

		do
		{
			if(escape)
			{
				escape = false; continue;
			}
			else if('\\' == *_iterator)
			{
				escape = true;
			}
			else if(pattern == *_iterator)
			{
				token.Type = TOKEN_LIT_TEXT;

				token.Value = std::string(begin,_iterator);

				NextChar();

				return ;
			}
		}while(NextChar());

		LEMON_USER_ERROR(errorCode,LEMON_PPC_C_CXX_LEX_ERROR);

		throw lemon::Exception(errorCode);
	}

	void Lexer::ReadNumber(Token & token)
	{
		buffer_type::const_iterator begin = _iterator;

		do
		{
			switch(*_iterator)
			{
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': 
				{
					continue;
				}
			}
			
			break;

			

		}while(NextChar());

		token.Type = TOKEN_LIT_NUMBER;

		token.Value = std::string(begin,_iterator);
	}

	void Lexer::ReadID(Token & token)
	{
		buffer_type::const_iterator begin = _iterator;

		while(NextChar())
		{
			if(!isalnum(*_iterator) && '_' != *_iterator) break;

		}

		token.Type = TOKEN_ID;

		token.Value = std::string(begin,_iterator);
	}

	bool Lexer::NextChar()
	{
		++ _iterator;

		++ _columns;

		return _iterator != _buffer.end();
	}
}}}