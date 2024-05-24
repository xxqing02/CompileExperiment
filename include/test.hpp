#pragma once
#include "PL0.hpp"

void test2(std::string infile,std::string outaddress) 
{
	PL0::Lexer lexer(infile);
	std::ofstream out(outaddress);

	for (auto token = lexer.nextToken(); token.type != PL0::TokenType::ENDOFFILE; token = lexer.nextToken()) {
		if (token.type == PL0::TokenType::KEYWORD) {
			auto it = PL0::KeyWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::OPERATOR) {
			auto it = PL0::OperatorWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::DELIMITER) {
			auto it = PL0::DelimiterWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::IDENTIFIER)
			out << "(ident," << token.value << ")" << std::endl;
		else if (token.type == PL0::TokenType::NUMBER)
			out << "(number," << token.value << ")" << std::endl;
		else if (token.type == PL0::TokenType::NONE)
			out << "(error," << token.value << ")" << std::endl;
	}
	out.close();
}

void test3(std::string infile) 
{
	PL0::Lexer lexer(infile);
	std::ofstream out("test/test3/temp.txt");

	for (auto token = lexer.nextToken(); token.type != PL0::TokenType::ENDOFFILE; token = lexer.nextToken()) {
		if (token.type == PL0::TokenType::KEYWORD) {
			auto it = PL0::KeyWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::OPERATOR) {
			auto it = PL0::OperatorWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::DELIMITER) {
			auto it = PL0::DelimiterWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::IDENTIFIER)
			out << "(ident," << token.value << ")" << std::endl;
		else if (token.type == PL0::TokenType::NUMBER)
			out << "(number," << token.value << ")" << std::endl;
		else if (token.type == PL0::TokenType::NONE)
			out << "(error," << token.value << ")" << std::endl;
	}
	out.close();

	std::string rules = "test/test3/rules.txt";
	PL0::LL1Parser Parser("test/test3/temp.txt",rules);
	std::remove("test/test3/temp.txt");
	Parser.getOriginFile(infile);
	Parser.m_grammar.printPredictTable();
	Parser.parse();
};

void test4(std::string infile, std::string outaddress) 
{
	PL0::Lexer lexer(infile);
	std::ofstream out("test/test4/temp.txt");

	for (auto token = lexer.nextToken(); token.type != PL0::TokenType::ENDOFFILE; token = lexer.nextToken()) {
		if (token.type == PL0::TokenType::KEYWORD) {
			auto it = PL0::KeyWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::OPERATOR) {
			auto it = PL0::OperatorWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::DELIMITER) {
			auto it = PL0::DelimiterWords.find(token.value);
			out << "(" << it->second << "," << token.value << ")" << std::endl;
		}
		else if (token.type == PL0::TokenType::IDENTIFIER)
			out << "(ident," << token.value << ")" << std::endl;
		else if (token.type == PL0::TokenType::NUMBER)
			out << "(number," << token.value << ")" << std::endl;
		else if (token.type == PL0::TokenType::NONE)
			out << "(error," << token.value << ")" << std::endl;
	}
	out.close();

	std::string rules = "test/test4/rules.txt";
	PL0::LL1Parser Parser("test/test4/temp.txt", rules);
	std::remove("test/test4/temp.txt");

	std::string L_SDT="test/test4/L-SDT.txt";
	Parser.getL_sdtFile(L_SDT);
	//Parser.parse();
	Parser.semanticParse();

};

void test6(std::string infile, std::string outaddress) {
	std::ifstream in(infile);
	//std::ofstream out(outaddress);

	std::string line;
	std::vector<PL0::Quadruple> inputQuadruples;
	while (std::getline(in, line)) {
		std::vector<std::string> quadItmes = PL0::split(line, ',');
		PL0::Quadruple quad;
		quad.op = quadItmes.at(0);
		quad.arg1 = quadItmes.at(1);
		quad.arg2 = quadItmes.at(2) == " " ? "" : quadItmes.at(2);
		quad.result = quadItmes.at(3);
		inputQuadruples.push_back(quad);
	}

	PL0::Optimizer optimizer;
	optimizer.buildDAG(inputQuadruples);

	auto resultQuadruples = optimizer.colloectQuadruples();
	for (auto& quad : resultQuadruples) {
		std::cout << std::format("{}, {}, {}, {}\n", quad.op, quad.arg1, quad.arg2, quad.result);
	}

	//out.close();
};