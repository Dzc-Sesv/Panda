#pragma once
#include "TokensSource.h"
#include"TokenFactory.h"
#include<fstream>
#include<string>
#include<stack>
class PandaLex :
	public TokensSource
{
public:
	PandaLex(std::string lexfilename, std::string srcfilename){
		lexfile.open(lexfilename);
		srcfile.open(srcfilename);
		ReadLexFile();
		NFAToDFA();
	};
	virtual Token* GetToken();
private:
	PandaLex();
	std::ifstream lexfile;
	std::ifstream srcfile;
	std::map<std::string, std::map<char, std::set<std::string>>> NFA;
	std::map<char, int> EndStateToId;
	std::map<int, int> DFAToId;
	std::map<int, std::map<char, int>> DFA;
	std::set<std::string> StartSymbols;
	std::set<std::string> EndSymbols;
	std::set<char> Symbols;
	std::set<int> EndState;
	std::string FirstSymbol;
	std::map<std::string, int>MapToUType;
	std::map<int, std::string> GTypeToUType;
	std::map<std::string, std::set<std::string>> Store;
	int colNum = 0;
	int rowNum = 1;
	char ReturnLetter(std::set<std::string> WorkSet);
	void NFAToDFA();
	void ReadLexFile();
	void GetClouser(std::set<std::string>& WorkSet);
	int ReturnUTypeId(int,std::string);
};

