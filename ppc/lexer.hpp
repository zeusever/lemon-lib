/**
* 
* @file     lexer
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/06/28
*/
#ifndef LEMON_PPC_LEXER_HPP
#define LEMON_PPC_LEXER_HPP
#include <vector>
#include <lemon/ppc/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace ppc{namespace core{

	struct Token
	{
		size_t				Type;

		std::string			Value;
	};

	class Lexer : private lemon::nocopyable
	{
	public:

		enum TokenType
		{

			TOKEN_RESERVED = 0xff, 
			
			TOKEN_PREPROCESS, 
			
			TOKEN_COMMENT,
			
			TOKEN_ID, 
			
			TOKEN_LIT_TEXT, 
			
			TOKEN_LIT_NUMBER
		};

		typedef std::vector<char>				buffer_type;

	public:

		Lexer():_lines(0),_columns(0) {}

		Lexer(const lemon_char_t * fileName) { ReloadFile(fileName); }

		bool Next(Token & token);
		
		void ReloadFile(const lemon_char_t * fileName);

		void ReloadString(const char * message);

		size_t	Line() const { return _lines; }

		size_t	Column() const { return _columns; }

	private:

		void ReadNewLine();

		void ReadPreprocess(Token & token);

		void ReadComment(Token & token);

		void ReadLitText(Token & token);

		void ReadNumber(Token & token);

		void ReadID(Token & token);

		bool IsNewLine();

		bool NextChar();

	private:

		size_t							_lines;

		size_t							_columns;

		buffer_type						_buffer;

		buffer_type::const_iterator		_iterator;					
	};

}}}


#endif //LEMON_PPC_LEXER_HPP