#include "PreDefined.hpp"

namespace PL0
{
	map KeyWords = {
		{"begin", "beginsym"},
		{"call", "callsym"},
		{"const", "constsym"},
		{"do", "dosym"},
		{"end", "endsym"},
		{"if", "ifsym"},
		{"odd", "oddsym"},
		{"procedure", "proceduresym"},
		{"read", "readsym"},
		{"then", "thensym"},
		{"var", "varsym"},
		{"while", "whilesym"},
		{"write", "writesym"}
	};

	map OperatorWords = {
		{"+", "plus"},
		{"-", "minus"},
		{"*", "times"},
		{"/", "slash"},
		{"=", "eql"},
		{"#", "neq"},
		{"<", "lss"},
		{"<=", "leq"},
		{">", "gtr"},
		{">=", "geq"},
		{":=", "becomes"}
	};

	map DelimiterWords = {
		{"(", "lparen"},
		{")", "rparen"},
		{",", "comma"},
		{";", "semicolon"},
		{".", "period"}
	};
}