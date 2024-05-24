#pragma once
#include "PL0.hpp"
#include <deque>
#include <set>
#include <map>
#include <iomanip>
#include <sstream>

namespace PL0
{
	struct Element {
		std::string type; 
		int value;
		bool hasValue = false;
		Element(const std::string& str):type(str),value(0){}
		Element(const std::string& str,int val):type(str),value(val){}
	};

	struct Symbol {
		char sign;
		int value;
	};

	class Grammar
	{
	public:
		Grammar(const std::string& rules);
		~Grammar() {};
		void calFirstSet();
		void calFollowSet();
		void calPredictTable();
		bool contain(const std::vector<char>& vec, char c);
		void printRules();
		void printVn();
		void printVt();
		void printFirstSet();
		void printFollowSet();
		void printPredictTable();
	public:
		std::vector<std::string> m_Rules;
		std::vector<char> m_Vn;
		std::vector<char> m_Vt;
		std::map<char, std::set<char>> m_First;
		std::map<char, std::set<char>> m_Follow;
		std::map<std::pair<char, char>, std::string> m_PredictTable;
		std::map<std::pair<char, char>, int> m_RuleRowTable;
	};

	class LL1Parser
	{
	public:
		LL1Parser(const std::string& filename,const std::string& rules);
		~LL1Parser();
		bool parse();
		void semanticParse();
		std::string getFileLine(int line);
		std::string getSign(int line);
		std::string getCode(int line);
		Symbol switchCode(std::string sign);
		Element switchCode(std::string sign,bool a);
		void getOriginFile(std::string filename);
		void getL_sdtFile(std::string filename);
		int getActionNumber(char left, std::string right, char Nonterminal);
		void printStack();
		void printExternStack();
		void actionFunction(int actionindex);

	public:
		std::string PL0;
		std::string m_file;
		std::stringstream l_sdt;
		std::string m_currentChar;
		int m_currentLine;
		int totalLines;
		std::deque<char> m_stack;
		std::deque<Element> m_externStack;
		Grammar m_grammar;

		mutable int m_valueCache = 0;
	};
}

