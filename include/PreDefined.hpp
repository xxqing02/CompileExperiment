#pragma once
#include <string>
#include <unordered_map>

namespace PL0 
{
	using map = std::unordered_map<std::string, std::string>;
	extern map KeyWords;
	extern map OperatorWords;
	extern map DelimiterWords;
}