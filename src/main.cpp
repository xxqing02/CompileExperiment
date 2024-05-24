#include "test.hpp"

int main(int argc,char* argv[])
{
	std::string inFilePath,fileName,outFilePath,test,inFilePathtest6;
	/*std::cin >> test;
	std::cin >> fileName;*/
	test = "test6";
	fileName = "example1";
	inFilePath = "test/" + test + "/" + fileName + ".pl0";
	inFilePathtest6 = "test/" + test + "/" + fileName + ".input";
	outFilePath = "output/" + test + "/" + fileName +".output";

	if (test == "test2")
		test2(inFilePath, outFilePath);
	else if (test == "test3")
		test3(inFilePath);
	else if (test == "test4")
		test4(inFilePath, outFilePath);
	else if (test == "test6")
		test6(inFilePathtest6, outFilePath);
	else
		std::cerr << "Invalid input filename!" << std::endl;
	
	return 0;
}