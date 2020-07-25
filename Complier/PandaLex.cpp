#include "PandaLex.h"
#include<iostream>
void PandaLex::ReadLexFile()
{
	int lenNum = 0;
	std::string index;
	lexfile >> lenNum;
	std::string startSymbol;
	std::string temp;
	std::string Symbol;
	std::string endSymbol;
	bool isFirstSymbol = true;
	while (lenNum){
		lexfile >> startSymbol >> temp >> Symbol >> endSymbol;
		if (isFirstSymbol) {
			FirstSymbol = startSymbol;
			isFirstSymbol = false;
		}
		if( (StartSymbols.find(startSymbol) == StartSymbols.end())){
			StartSymbols.insert(startSymbol);
		}
		if (EndSymbols.find(endSymbol) == EndSymbols.end()) {
				EndSymbols.insert(endSymbol);
		}
		for (auto letter : Symbol){
			Symbols.insert(letter);
			NFA[startSymbol][letter].insert(endSymbol);
		}
		lenNum--;
	}
	lexfile >> lenNum;
	std::string end;
	int TypeId;
	while (lenNum)
	{
		lexfile >> end >> TypeId;
		EndStateToId[end[0]] = TypeId;
		lenNum--;
	}
	std::string name;
	lexfile >> name;
	lexfile >> lenNum;
	std::string keyword;
	while (lenNum)
	{
		lexfile >> keyword>>index;
		Store[name].insert(keyword);
		MapToUType[keyword] = stoi(index);
		lenNum--;
	}
	lexfile >> name;
	lexfile >> lenNum;
	std::string moperator;
	while (lenNum)
	{
		lexfile >> moperator>>index;
		Store[name].insert(moperator);
		MapToUType[moperator] = stoi(index);
		lenNum--;
	}
	lexfile >> name;
	lexfile >> lenNum;
	std::string boperator;
	while (lenNum)
	{
		lexfile >> boperator>>index;
		Store[name].insert(boperator);
		MapToUType[boperator] = stoi(index);
		lenNum--;
	}
	lexfile >> lenNum;
	int state;
	while (lenNum)
	{
		lexfile >> state >> name;
		GTypeToUType[state] = name;
		lenNum--;
	}
}
int PandaLex::ReturnUTypeId(int state, std::string value)
{
	if (Store[GTypeToUType[state]].find(value) != Store[GTypeToUType[state]].end()) {
		return MapToUType[value];
	}
	else
		return -1;
}
void PandaLex::NFAToDFA()
{
	std::vector<std::set<std::string>> WorkSetVector;//所有的集合
	std::stack<int> WorkSetStack;//新生成的集合
	std::set<std::string> StartWorkSet;
	StartWorkSet.insert(FirstSymbol);
	GetClouser(StartWorkSet);
	WorkSetVector.push_back(StartWorkSet);
	WorkSetStack.push(WorkSetVector.size());//新产生的集合的序号
	while (!WorkSetStack.empty()) {
		int index = WorkSetStack.top();
		std::set<std::string> Set = WorkSetVector[WorkSetStack.top()-1];
		std::set<std::string> temp;
		WorkSetStack.pop();
		for (auto letter : Symbols) {
			if (letter == '`')
				continue;
			for (auto Item:Set) {
				for (auto term : NFA[Item][letter]) {
					if (temp.find(term) == temp.end()) {
						temp.insert(term);
					}//对于每个集合
				}
				
			}
			GetClouser(temp);
			if (temp.size() == 0)
				continue;
			if (find(WorkSetVector.begin(), WorkSetVector.end(), temp) == WorkSetVector.end()) {//新产生的集合没有找到
				WorkSetVector.push_back(temp);
				WorkSetStack.push(WorkSetVector.size());
				char l = ReturnLetter(temp);
				if (l != '0') {
					DFAToId[WorkSetStack.top()] = EndStateToId[l];
					EndState.insert(WorkSetStack.top());
				}
				DFA[index][letter] = WorkSetStack.top();
			}
			else {
				DFA[index][letter] = find(WorkSetVector.begin(), WorkSetVector.end(), temp) - WorkSetVector.begin()+1;
			}
			temp.clear();
		}
	}
}
Token * PandaLex::GetToken()
{
	restart:
	std::string currentString;
	std::stack<int> state;
	std::stack<int> colNums;
	state.push(1);
	char letter; 
	int currentState = 1;
	do
	{
		letter = srcfile.get();
		if (letter == '\n'){
			this->rowNum++;
			colNums.push(colNum);
			this->colNum = 0;
			break;
		}
		else if (letter == '\t') {
			this->colNum += 4;
			break;
		}
		else if (letter == ' ') {
			this->colNum += 1;
			break;
		}
		else
		{
			currentString.push_back(letter);
			currentState = DFA[state.top()][letter];
			if (EndState.find(currentState) != EndState.end()) {
				while (!state.empty()){
					state.pop();
				}
			}
			state.push(currentState);
			colNum++;
		}
			
	} while (currentState);
	while (!state.empty()&&EndState.find(state.top()) == EndState.end())
	{
		state.pop();
		if (!state.empty()) {
			currentState = state.top();
			srcfile.seekg(-1, std::ios::cur);
			currentString.pop_back();
			if (this->colNum)
				colNum--;
			else {
				colNum = colNums.top();
				rowNum -= 1;
				colNums.pop();
			}
		}
	}
	if (!state.size()&&letter == EOF)
		return new Token(-1,-1,"-1",this->rowNum,this->colNum);
	if (!state.size() && (letter == '\t' || letter == '\n' || letter == ' '))
		goto restart;
	if (!state.size() && currentString == "") {
		std::cout << "词法错误" <<currentString<< std::endl;
		std::cout << "错误出现在："<<this->rowNum <<"行   "<< this->colNum <<"列		"<< std::endl;
		return new Token(-1, -1, "-1", this->rowNum, this->colNum);
	}
		
	return new Token(DFAToId[currentState], ReturnUTypeId(DFAToId[currentState],currentString), currentString, this->rowNum, this->colNum);
}
char PandaLex::ReturnLetter(std::set<std::string> WorkSet)
{
	for (auto letter : WorkSet) {
		if (letter[0]>= 'A'&&letter[0] <= 'Z') {
			return letter[0];
		}
	}
	return '0';
}
void PandaLex::GetClouser(std::set<std::string>& WorkSet)
{
	bool isChanged = true;
	while (isChanged)
	{
		isChanged = false;
		for (auto item : WorkSet) {
			for (auto term : NFA[item]['`']) {
				if (WorkSet.find(term) == WorkSet.end()) {
					WorkSet.insert(term);
					isChanged = true;
				}	
			}
		}
	}
}