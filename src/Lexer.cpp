#include "Lexer.hpp"

namespace PL0 
{
	Lexer::Lexer(const std::string& filename)
		: m_line(1), m_column(0)
	{
		std::ifstream code(filename);

		if (!code.is_open())
			throw OpenFileFailed(filename);
		std::stringstream buffer;
		buffer << code.rdbuf();
		m_file = buffer.str();

		std::transform(m_file.begin(), m_file.end(), m_file.begin(), [](unsigned char c) { return std::tolower(c); });

		m_currentChar = getChar(m_line, m_column);
	}

	Lexer::~Lexer() {}

	bool Lexer::isKeyWords(std::string str) 
	{
		return PL0::KeyWords.find(str)!= PL0::KeyWords.end();
	}

	bool Lexer::isOperatorWords(std::string str)
	{
		return PL0::OperatorWords.find(str) != PL0::OperatorWords.end();
	}

	bool Lexer::isDelimiterWords(std::string str)
	{
		return PL0::DelimiterWords.find(str) != PL0::DelimiterWords.end();
	}

	bool Lexer::isIdentifier(std::string str)
	{
		if ( (str.empty()) || (!isalpha(str[0])) )
			return false;

		for (int i = 1; i < str.size(); i++)
			if (!(isalpha(str[i])) && !(isdigit(str[i])))
				return false;

		return true;
	}

	char Lexer::getChar(size_t line, size_t column)
	{
		size_t pos = 0;

		for (size_t i = 0; i < line - 1; i++)
		{
			pos = m_file.find('\n', pos);
			pos++;
		}

		pos += column;

		return m_file[pos];
	}

	void Lexer::nextChar()
	{
		m_column++;
		if (m_currentChar == '\n')
		{
			m_line++;
			m_column = 0;
		}

		m_currentChar = getChar(m_line, m_column);
	}

	char Lexer::getNextChar(size_t line, size_t column)
	{
		return getChar(line, column+1);
	}


	Token Lexer::nextToken()
	{
		skipSpace();
		skipComment();
		skipSpace();

		if (isdigit(m_currentChar))
			return parseNumber();
		else if (isDelimiterWords(std::string(1, m_currentChar)))
			return parseDelimiter();
		else if (isOperatorWords(std::string(1, m_currentChar))||m_currentChar==':')
			return parseOperator();
		else if (isalpha(m_currentChar))
			return parseKeyWordOrIdentifier();
		else if (m_currentChar=='\0')
			return parseEOF();
		else return parseUnknownSymbol();
	}

	void Lexer::skipComment()
	{
		if (m_currentChar == '{')
		{
			while (m_currentChar != '}' && m_currentChar != '\0')
				nextChar();
			if (m_currentChar == '\0')
				return;
			nextChar();
		}
	}

	void Lexer::skipSpace()
	{
		while (m_currentChar == ' '||m_currentChar == '\n'||m_currentChar == '\t'||m_currentChar == '\r')
		{
			nextChar();
		}
	}

	Token Lexer::parseNumber()
	{
		std::string word;
		while (isdigit(m_currentChar))
		{
			word += m_currentChar;
			nextChar();
		}
	
		if (isalpha(m_currentChar))
		{
			while (isdigit(m_currentChar) || isalpha(m_currentChar))
			{
				word += m_currentChar;
				nextChar();
			}
			std::cout << std::format("Error: Line {0}, {1} is not a valid identifier\n",m_line,word);
			return { TokenType::NONE, word };  
		}
			
		return { TokenType::NUMBER, word };
	}

	Token Lexer::parseDelimiter()
	{
		std::string delimiter = std::string(1, m_currentChar);
		nextChar();
		return { TokenType::DELIMITER, delimiter };
	}

	Token Lexer::parseOperator()
	{
		std::string op,op2;
		op = m_currentChar; 
		nextChar();
		op2 = op + m_currentChar;
		if (isOperatorWords(op2))
		{
			nextChar();
			return { TokenType::OPERATOR, op2 };
		}
		else if (isOperatorWords(op))
		{
			return { TokenType::OPERATOR, op };
		}
		else  {			
			std::cout<<std::format("Error: Line {0}, {1} is not a valid operator\n",m_line,op);
			return { TokenType::NONE, op};
		}
	}

	Token Lexer::parseKeyWordOrIdentifier()
	{
		std::string word;
		while (isalpha(m_currentChar))
		{
			word += m_currentChar;
			nextChar();
		}
		if (isKeyWords(word))
			return { TokenType::KEYWORD, word };

		while (isalpha(m_currentChar) || isdigit(m_currentChar))
		{
			word += m_currentChar;
			nextChar();
		}
		return { TokenType::IDENTIFIER, word };
	}

	Token Lexer::parseUnknownSymbol() 
	{
		std::string unknown;
		if (!isDelimiterWords(std::string(1, m_currentChar)) && !isalpha(m_currentChar) && !isdigit(m_currentChar))
		{
			unknown += m_currentChar;
			nextChar();
		}
		std::cout << std::format("Error: Line {0}, {1} is an unknown symbol\n",m_line,unknown);
		return { TokenType::NONE, unknown };
	}

	Token Lexer::parseEOF()
	{
		return { TokenType::ENDOFFILE, "end of file" };
	}
}
