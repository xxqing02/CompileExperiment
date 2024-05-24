#pragma once
#include "PL0.hpp"
#include <iostream>
#include <format>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <queue>

namespace PL0
{
	enum class TokenType
	{
		NONE,
		KEYWORD,
		IDENTIFIER,
		NUMBER,
		OPERATOR,
		DELIMITER,
		ENDOFFILE
	};

	struct Token
	{
		TokenType type;
		std::string value;
	};

	class Lexer
	{
	public:
		explicit Lexer(const std::string& filename);
		~Lexer();

		bool isKeyWords(std::string str);
		bool isOperatorWords(std::string str);
		bool isDelimiterWords(std::string str);
		bool isIdentifier(std::string str);
		Token nextToken();
		std::string showfile() { return m_file; }
		char showCurrentChar() { return m_currentChar; }
		char getNextChar(size_t line, size_t column);

	private:
		void nextChar();
		char getChar(size_t line, size_t column);
		void skipComment();
		void skipSpace();
		Token parseNumber();
		Token parseKeyWordOrIdentifier();
		Token parseOperator();
		Token parseDelimiter();
		Token parseEOF();
		Token parseUnknownSymbol();

	private:
		std::string m_file;
		char m_currentChar;
		size_t m_line;
		size_t m_column;
		std::queue<std::string> m_errors;
	};
}