#include <LL1Parser.hpp>
#include <algorithm>

namespace PL0
{
	Grammar::Grammar(const std::string& rules)
	{
		std::ifstream file(rules);
		if (!file.is_open())
			throw OpenFileFailed(rules);
		std::stringstream ss;
		ss << file.rdbuf();
		std::string line;
		while (std::getline(ss, line))
			m_Rules.push_back(line);
		for (auto &rule : m_Rules)
		{
			char left = rule[0];
			if (!contain(m_Vn, left))
				m_Vn.push_back(left);
		}
		for (auto &rule : m_Rules)
		{
			std::string right = rule.substr(3);
			for (int i=0;i<right.size();i++)
				if (!(right[i] >= 'A' && right[i] <= 'Z'))
					if (!contain(m_Vt, right[i]))
						m_Vt.push_back(right[i]);
		}
		
		for (auto& vn : m_Vn) {
			m_First[vn] = std::set<char>();
			m_Follow[vn] = std::set<char>();
		}
		
		calFirstSet();
		calFollowSet();

		std::vector<char> terminals = m_Vt;
		terminals.push_back('#');
		auto it = std::find(terminals.begin(), terminals.end(), 'e');
		if (it != terminals.end()) {
			terminals.erase(it);
		}

		for (auto k1 : m_Vn) {
			for (auto k2 : terminals) {
				m_PredictTable[{k1, k2}] = " ";
				m_RuleRowTable[{k1, k2}] = 0;
			}
		}
		calPredictTable();
	}

	void Grammar::calFirstSet()
	{
		bool changed = true;
		while (changed) {
			changed = false;
			for (int i=0;i<m_Rules.size();i++)
			{
				std::string rule = m_Rules[i];
				char left = rule[0];
				std::string right = rule.substr(3);
				if (islower(right[0])) // ident or number or epsilon
				{
					if(m_First[left].insert(right[0]).second)
						changed = true;
				}
				else {
					// nonterminal signal
					for (int j=0;j<right.size();j++)
					{
						if (isupper(right[j])) // if signal is non terminal
						{
							size_t sizeBefore = m_First[left].size();
							std::set<char> temp = m_First[right[j]];
							m_First[left].insert(temp.begin(), temp.end());
							size_t sizeAfter = m_First[left].size();
							if (sizeBefore != sizeAfter)
								changed = true;
							if (temp.find('e') == temp.end()) // if e is not in the set
								break;
						}
						else // + - * / ( )
						{
							if (m_First[left].insert(right[j]).second)
								changed = true;
							break;
						}
					}
				}
			}
		}
	}

	void Grammar::calFollowSet()
	{
		m_Follow[m_Vn[0]].insert('#');  // Assume '#' is end-of-input marker for the start symbol

		bool changed = true;
		while (changed)
		{
			changed = false;
			for (int i = 0; i < m_Rules.size(); i++)
			{
				std::string rule = m_Rules[i];
				char left = rule[0];
				std::string right = rule.substr(3);
				for (int j = 0; j < right.size(); j++)
				{
					if (contain(m_Vn, right[j])) // Check if right[j] is a nonterminal
					{
						char B = right[j];
						size_t next = static_cast<size_t>(j + 1);
						bool epsilon = true;

						while (next < right.size() && epsilon)
						{
							epsilon = false;
							if (contain(m_Vn, right[next]))
							{
								for (char symbol : m_First[right[next]])
								{
									if (symbol != 'e') {//epsilon
										bool inserted = m_Follow[B].insert(symbol).second;
										changed = changed || inserted;
									}
									else
										epsilon = true;
								}

							}
							else if (contain(m_Vt, right[next])) {
								bool inserted = m_Follow[B].insert(right[next]).second;
								changed = changed || inserted;
								break;  
							}

							++next;
						}

						// If β derives ε or no symbols after B, add FOLLOW(A) to FOLLOW(B)
						if (next >= right.size() || epsilon)
						{
							for (char symbol : m_Follow[left])
							{
								bool inserted = m_Follow[B].insert(symbol).second;
								changed = changed || inserted;
							}
						}
					}
				}
			}
		}
	}

	void Grammar::calPredictTable()
	{
		for(int i = 0; i < m_Rules.size(); i++)
		{
			std::string rule = m_Rules[i];
			char left = rule[0];
			std::string right = rule.substr(3);
			if (contain(m_Vn, right[0])) // If the first symbol on the right-hand side is a nonterminal
			{
				for (char a : m_First.at(right[0])) // For each terminal in FIRST(right[0])
				{
					if (a != 'e')
					{
						m_PredictTable[{left, a}] = right;
						m_RuleRowTable[{left, a}] = i + 1;
					}
				}

				if (m_First.at(right[0]).find('e') != m_First.at(right[0]).end()) // If FIRST(right[0]) contains epsilon
				{
					for (char b : m_Follow.at(left))
					{
						m_PredictTable[{left, b}] = right;
						m_RuleRowTable[{left, b}] = i + 1;
					}
				}
			}
			else // If the first symbol on the right-hand side is a terminal
			{
				if (right[0] != 'e')
				{
					// Add the production rule to the predictive parsing table
					m_PredictTable[{left, right[0]}] = right;
                    m_RuleRowTable[{left, right[0]}] = i + 1;
				}
				else {
					// If the right-hand side is epsilon, consider follow set of the left-hand symbol
					for (char b : m_Follow.at(left))
					{
						// Add the production rule to the predictive parsing table
						m_PredictTable[{left, b}] = right;
                        m_RuleRowTable[{left, b}] = i + 1;
					}
				}
			}
			
		}
	}

	bool Grammar::contain(const std::vector<char>& vec,char c)
	{
		auto it = std::find(vec.begin(), vec.end(), c);
		return it != vec.end();
	}

	void Grammar::printRules() 
	{
		for (auto &rule : m_Rules)
			std::cout << rule << std::endl;
	}	

	void Grammar::printVn()
	{
		for (auto &vn : m_Vn)
			std::cout << vn << " ";
		std::cout << std::endl;
	}

	void Grammar::printVt()
	{
		for (auto &vt : m_Vt)
			std::cout << vt << " ";
		std::cout << std::endl;
	}

	void Grammar::printFirstSet()
	{			
		for (const auto& pair : m_First) {
			char non_terminal = pair.first;
			const std::set<char>& first_set = pair.second;

			std::cout << "FIRST(" << non_terminal << ") = {";

			for (auto iter = first_set.begin(); iter != first_set.end(); ++iter) {
				if (iter != first_set.begin()) {
					std::cout << ", ";
				}
				std::cout << *iter;
			}
			std::cout << "}" << std::endl;
		}
	}

	void Grammar::printFollowSet()
	{
		for (const auto& pair : m_Follow) {
			char non_terminal = pair.first;
			const std::set<char>& follow_set = pair.second;

			std::cout << "FOLLOW(" << non_terminal << ") = {";

			for (auto iter = follow_set.begin(); iter != follow_set.end(); ++iter) {
				if (iter != follow_set.begin()) {
					std::cout << ", ";
				}
				std::cout << *iter;
			}
			std::cout << "}" << std::endl;
		}
	}

	void Grammar::printPredictTable()
	{
		const int width = 8; 
		std::cout << std::left; 

		std::vector<char> terminals = m_Vt;
		terminals.push_back('#');
		auto it = std::find(terminals.begin(), terminals.end(), 'e');
		if (it != terminals.end()) {
			terminals.erase(it); 
		}

		std::cout << std::setw(width) << ' ';
		for (char terminal : terminals) {
			std::cout << std::setw(width) << terminal;
		}
		std::cout << std::endl;

		for (char nonTerminal : m_Vn) {
			std::cout << std::setw(width) << nonTerminal;
			for (char terminal : terminals) {
				auto it = m_PredictTable.find({ nonTerminal, terminal });
				if (it != m_PredictTable.end()) {
					std::cout << std::setw(width) << it->second;
				}
				else {
					std::cout << std::setw(width) << ' '; 
				}
			}
			std::cout << std::endl;
		}
		
	}

	LL1Parser::LL1Parser(const std::string& filename, const std::string& rules) : m_grammar(rules)
	{
		std::ifstream input(filename);
		if (!input.is_open())
			throw OpenFileFailed(filename);

		std::stringstream buffer;
		buffer << input.rdbuf();
		m_file = buffer.str();
		m_currentLine = 1;
		m_currentChar = getSign(m_currentLine);
		
		input.clear();
		input.seekg(0, std::ios::beg);
		std::string line;
		totalLines = 0;  
		while (std::getline(input, line)) {
			++totalLines;  
		}
		totalLines += 1; //input like (a+15)*b#
	}

	LL1Parser::~LL1Parser() {}

	std::string LL1Parser::getSign(int line)
	{
		std::string sign,linestring;
		linestring = getFileLine(line);
		int index = 0;
		while (linestring[index] != ',')
			index++;
		sign += linestring[++index];
		while (linestring[static_cast<size_t>(index + 1)] != ')')
			sign += linestring[++index];
		return sign;
	}

	void LL1Parser::getL_sdtFile(std::string filename)
	{
		std::ifstream file(filename);
		l_sdt << file.rdbuf();
	}

	std::string LL1Parser::getCode(int line)
	{
		std::string sign, linestring;
		linestring = getFileLine(line);
		int index = 0;
		while (linestring[static_cast<size_t>(index + 1)] != ',')
			sign += linestring[++index];
		return sign;
	}

	std::string LL1Parser::getFileLine(int line)
	{
		std::string res;
		size_t poi = 0;
		while (line > 1)
		{
			if (line > 0)
				line--;
			poi = m_file.find('\n', poi) + 1;
		}
		for (size_t i=poi;m_file[i]!='\n' and m_file[i]!='\0';i++)
			res+=m_file[i];
		return res;
	}

	Symbol LL1Parser::switchCode(std::string sign) 
	{
		switch (sign[0]) 
		{
			case 'i': return Symbol{ 'i',0 };
			case 'n': return Symbol{ 'n',std::stoi(getSign(m_currentLine)) };
			case 'p': return Symbol{ '+',0 };
			case 'm': return Symbol{ '-',0 };
			case 't': return Symbol{ '*',0 };
			case 's': return Symbol{ '/',0 };
			case 'l': return Symbol{ '(',0 };
			case 'r': return Symbol{ ')',0 };
			default : return Symbol{ '#',0 };
		}
	}

	Element LL1Parser::switchCode(std::string sign,bool a)
	{
		switch (sign[0])
		{
			case 'n': return Element{ "n", std::stoi(getSign(m_currentLine)) };
			case 'p': return Element{ "+" };
			case 'm': return Element{ "-" };
			case 't': return Element{ "*" };
			case 's': return Element{ "/" };
			case 'l': return Element{ "(" };
			case 'r': return Element{ ")" };
			default : return Element{ "#" };
		}
	}

	void LL1Parser::getOriginFile(std::string filename)
	{
		std::ifstream code(filename);
		if (!code.is_open())
			throw OpenFileFailed(filename);
		std::stringstream buffer;
		buffer << code.rdbuf();
		PL0 = buffer.str();
	}

	void LL1Parser::printStack()
	{
		std::cout << "Stack: ";
		for (int i = 0; i < m_stack.size(); i++)
			std::cout << m_stack[i];
	}

	bool LL1Parser::parse()
	{
		m_stack.push_back('#');
		m_stack.push_back('E'); 

		while (m_stack.back() != '#') {
			printStack();

			std::cout<< " Current char: " << m_currentChar << std::endl;
			
			Symbol c = (m_currentChar == "#") ? Symbol{'#', 0} : switchCode(getCode(m_currentLine));
			char top = m_stack.back();
			if (top == c.sign) {
				m_stack.pop_back();
				m_currentLine++;

				if (m_currentLine < totalLines)
					m_currentChar = getSign(m_currentLine);
				else 
					m_currentChar = "#";
			}
			else if (m_grammar.contain(m_grammar.m_Vn, top)) {
				
				std::string production = m_grammar.m_PredictTable[{top, c.sign}];
				if (production == " ") 
				{
					std::cout << "Error: No production found for " << top << " and " << m_currentChar[0] << std::endl;
					return false;
				}

				m_stack.pop_back();

				for (size_t i = production.size(); i-- > 0; ) {
					if (production[i] != 'e') {
						m_stack.push_back(production[i]);
					}
				}

			}
			else {
				std::cout << "Signal " << top <<" not found in expression " << std::endl;
				return false;
			}
		}

		if (m_stack.back() == '#' && m_currentChar == "#") {
			std::cout << "Parse successfully!" << std::endl;
			return true;
		}
		else {
			std::cout << "Parsing failed!" << std::endl;
			std::cout << PL0 << std::endl;
			for (size_t i = 0; i < m_currentLine; ++i) 
				std::cout << ' ';
			std::cout << '*' << std::endl;
			return false;
		}

		return m_currentLine==totalLines;
	}

	void LL1Parser::printExternStack() 
	{
		std::cout << "Extern Stack: "<<std::endl;
		for (size_t i = m_externStack.size()-1; i-- > 0;)
			std::cout << m_externStack[i].type << std::endl;
	}

	// function : S->/F{12}S{13} 传入的参数为 S /FS S 返回{13} 里的整数13
	int LL1Parser::getActionNumber(char left, std::string right, char sign)
	{
		l_sdt.clear(); 
		l_sdt.seekg(0, std::ios::beg); 
		
		int result = 0;
		std::string line;
		while (std::getline(l_sdt, line))
		{
			std::string lineRight = line.substr(3);
			if (line[0] == left && line[3] == right[0])
			{
				for (int i = 0; i < lineRight.size(); i++)
				{
					if (lineRight[i] == sign)
					{
						int index = i+2;
						std::string r;
						while (lineRight[index] != '}') {
							r += (1, lineRight[index]);
							index += 1;
						}
							
						result = std::stoi(r);
						break;
					}
				}
				break;
			}
		}
		return result;
	}

	void LL1Parser::semanticParse()
	{
		m_externStack.push_back({ "#" , 0 });
		m_externStack.push_back({ "Esyn", 0 });
		m_externStack.push_back({ "E", 0 });

		while (m_externStack.back().type != "#")
		{
			printExternStack();
			std::cout << "Current char: " << m_currentChar << std::endl; // 分析指针

			Element c = (m_currentChar == "#") ? Element{ "#",0 } : 
				switchCode(getCode(m_currentLine),true); //当前元素

			Element top = m_externStack.back();
			if (top.type == c.type)   // 栈顶元素和当前字符匹配  + - * / n
			{
				m_currentLine++;
				if (m_currentLine < totalLines)
					m_currentChar = getSign(m_currentLine);
				else
					m_currentChar = "#";

				if (top.type == "n")
				{
					m_externStack[m_externStack.size() - 2].hasValue = true;
					m_externStack[m_externStack.size() - 2].value = top.value;
				}
				m_externStack.pop_back();
			}
            else if (m_externStack.back().type.size() >= 5)  //也就是actionN
            {
                int aindex = std::stoi(m_externStack.back().type.substr(6));
                actionFunction(aindex);  // to be completed
                m_externStack.pop_back();
            }
            else if (m_externStack.back().type.size() == 4) // 综合属性
            {
				int value = m_externStack.back().value;
	
                if (m_externStack.back().hasValue)
                {
					size_t s = m_externStack.size();
                    if (m_externStack[m_externStack.size() - 2].type.size() >= 6 
						&& ( m_externStack[s - 2].type.substr(6) == "3" || m_externStack[s - 2].type.substr(6) == "5" || m_externStack[s - 2].type.substr(6) == "10" || m_externStack[s - 2].type.substr(6) == "12" ))  
                    {
							m_valueCache = value;
                    }
                    else 
                    {
                        m_externStack[m_externStack.size() - 2].hasValue = true;
                        m_externStack[m_externStack.size() - 2].value = value;
                    }
                }
				
                m_externStack.pop_back();

				if (m_externStack.back().type == "#" && m_currentChar == "#")
				{
					std::cout << "Parse successfully" << std::endl;
					std::cout << value << std::endl;
				}
            }
			else if (top.type.size()==1 && m_grammar.contain(m_grammar.m_Vn, top.type[0]))
			{
				// example : 栈顶字符为非终结符E 
				// E->T{a1}G{a2}   T Tsyn {a1} G Gsyn {a2}入栈
				
				size_t oldTopIndex =  m_externStack.size() - 1 ;
				// If top has value:
				//		top.value => newTop.value
				bool topHasValue = m_externStack.back().hasValue;
				int topValue = m_externStack.back().value;

				m_externStack.pop_back();

				std::string production = m_grammar.m_PredictTable[{top.type[0], c.type[0]}];
				int rowIndex = m_grammar.m_RuleRowTable[{top.type[0], c.type[0]}];

				if (production == " ")
				{
					std::cout << "Error: No production found for " << top.type[0] << " and " << c.type[0] << std::endl;
					return;
				}
					
				for (size_t i = production.size(); i-- > 0; ) {
					if (production[i] != 'e') {
						if (isupper(production[i])) {
							std::string Nonterminal( 1,production[i] );
							Element NonterminalElement{ Nonterminal };
							Element NonterminalSynElement{ Nonterminal + "syn", 0 };
							int actionindex = getActionNumber(top.type[0], production, production[i]);
							Element actionElement{ "action" + std::to_string(actionindex), 0 };

							m_externStack.push_back(actionElement);							
							m_externStack.push_back(NonterminalSynElement);
							m_externStack.push_back(NonterminalElement);
						}
						else // production[i]为 + - * / n ( )
						{
							if (production[i] == ')')
							{
								int actionindex = getActionNumber(top.type[0], production, production[i]);
								Element actionElement{ "action" + std::to_string(actionindex), 0 };
								m_externStack.push_back(actionElement);
							}
							
							Element element{ std::string(1,production[i]) };  // 将 n 压入栈中
							if (production[i] == 'n')
							{
								element.value = c.value;
							}
							m_externStack.push_back(element);
							
							
						}
					}
					else
					{
                        int actionindex = getActionNumber(top.type[0], production, production[i]);
                        Element actionElement{ "action" + std::to_string(actionindex), 0 };

                        m_externStack.push_back(actionElement);
					}
				}

                if (topHasValue)
                {
					if (production == "+TG" || production == "-TG" || production == "*FS" || production == "/FS")
                    {
						m_externStack[oldTopIndex + 3].hasValue = true;
						m_externStack[oldTopIndex + 3].value = topValue;
                    }
                    else
                    {
                        m_externStack.back().hasValue = true;
                        m_externStack.back().value = topValue;
                    }
                }
			}
			else {
				std::cout << "Grammar error!" << std::endl;
				std::cout << "Parse failed." << std::endl;
				break;
			}
		}

		if (m_currentChar != "#")
		{
			std::cout << "Parse failed,extra symbols appeared." << std::endl;
		}
	}

	void LL1Parser::actionFunction(int actionindex)
	{
        Element& top = m_externStack.back();
        Element& next = m_externStack[m_externStack.size() - 2];
		if (m_externStack.size() - 3 >= 0)
		{
			Element& nextnext = m_externStack[m_externStack.size() - 3];
			if (actionindex == 16)
			{
				nextnext.value = top.value;
			}
		}
		//E->T{ 1 }G{ 2 }
		if (actionindex == 1)
		{
			next.value = top.value;
		}

		if (actionindex == 2)
		{
			next.value = top.value;
		}

		//G-> + T{ 3 }G{ 4 }
		if (actionindex == 3)
		{
			next.value = top.value + m_valueCache;
		}

		if (actionindex == 4)
		{
			next.value = top.value;
		}
		
		//G-> - T{ 5 }G{ 6 }
		if (actionindex == 5)
		{
			next.value = top.value - m_valueCache;
		}
		if (actionindex == 6)
		{
			next.value = top.value;
		}

		//G->e{7}
		if (actionindex == 7)
		{
			next.value = top.value;
		}

		//T->F{8}S{9}
		if (actionindex == 8)
		{
			next.value = top.value;
		}
		if (actionindex == 9)
		{
			next.value = top.value;
		}

		//S->*F{10}S{11}
		if (actionindex == 10)
		{
			next.value = top.value * m_valueCache;
		}
		if (actionindex == 11)
		{
			next.value = top.value;
		}

		//S->/F{12}S{13}
		if (actionindex == 12)
		{
			if (m_valueCache == 0) {
				std::cout << "Error: Division by zero" << std::endl;
				exit(1);
			}
			next.value = top.value / m_valueCache;
		}

		if (actionindex == 13)
		{
			next.value = top.value;
		}

		//S->e{14}
		if (actionindex == 14)
		{
			next.value = top.value;
		}

		//F->n{15}
		if (actionindex == 15)
		{
			next.value = top.value;
		}

		//F->(E{16}){17}
		
		if (actionindex == 17)
		{
			next.value = top.value;
		}

		next.hasValue = true;
	}
}