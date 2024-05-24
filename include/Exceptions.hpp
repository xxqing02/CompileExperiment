#pragma once
#include <exception>
#include <string>
#include <iostream>

namespace PL0
{	
	class Exception : public std::exception
	{

	public:
		Exception() = default;
		virtual ~Exception() = default;

	protected:
		std::string m_message;
	};

	class OpenFileFailed : public Exception
	{
	public:
		OpenFileFailed(const std::string& filename)
		{
			m_message = "Failed to open file: " + filename;
		}

	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};

	class UnknownWord : public Exception
	{
	public:
		UnknownWord(const std::string& word)
		{
			m_message = "Unknown word: " + word;
		}

	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};

	class InvalidIdent : public Exception
	{
	public:
		InvalidIdent(const std::string& ident)
		{
			m_message = "Invalid identifier: " + ident;
		}

	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};

	class InvalidOperator : public Exception
	{
	public:
		InvalidOperator(const std::string& op)
		{
			m_message = "Invalid operator: " + op;
		}
	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};

	class NotImmeplemented : public Exception
	{
	public:
		NotImmeplemented(const std::string& func)
		{
			m_message = "Not implemented: " + func;
		}

	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};

	class AleadyExist : public Exception
	{
	public:
		AleadyExist(const std::string& ident)
		{
			m_message = "Aleady exist: " + ident;
		}

	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};

	class UnMatched : public Exception
	{
	public:
		UnMatched(const std::string& str)
		{
			m_message = "Unmatched: " + str;
		}

	private:
		virtual const char* what() const noexcept override
		{
			return m_message.c_str();
		}
	};
}

