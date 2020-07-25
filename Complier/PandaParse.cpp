#include "PandaParse.h"
#include"windows.h"
#include<iostream>
#include<queue>
PandaParse::PandaParse(std::string GrammerFile, std::string LRName,TokensSource* lex)
{
	this->tokenSource = lex;
	this->GrammerFile.open(GrammerFile);
	this->GrammerFileName = GrammerFile;
	this->LRName = LRName;
	this->fileisChanged = FileIsChanged();
	if (fileisChanged)
	{
		LRFile.clear();
		LRFile.open(LRName);
		if (!LRFile.is_open())
			getchar();
	}
	else
	{
		LRIn.clear();
		LRIn.open(LRName);
		if (!LRIn.is_open())
			getchar();
	}
}
bool PandaParse::FileIsChanged()
{
	HANDLE HGrammerFile = CreateFileA(this->GrammerFileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	HANDLE HlRFile = CreateFileA(this->LRName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	FILETIME fCreateTime1, fAccessTime1, fCreateTime2, fAccessTime2, fWriteTime1, fWriteTime2;
	ZeroMemory(&fWriteTime1, sizeof(FILETIME));
	ZeroMemory(&fWriteTime2, sizeof(FILETIME));
	GetFileTime(HGrammerFile, &fCreateTime1, &fAccessTime1, &fWriteTime1);
	GetFileTime(HlRFile, &fCreateTime2, &fAccessTime2, &fWriteTime2);
	SYSTEMTIME stLocal1, stLocal2;
	ZeroMemory(&stLocal1, sizeof(SYSTEMTIME));
	ZeroMemory(&stLocal2, sizeof(SYSTEMTIME));
	FileTimeToSystemTime(&fWriteTime1, &stLocal1);
	FileTimeToSystemTime(&fWriteTime2, &stLocal2);
	CloseHandle(HGrammerFile);
	CloseHandle(HlRFile);
	return stLocal1.wDay > stLocal2.wDay || stLocal1.wDay == stLocal2.wDay&&stLocal1.wHour > stLocal2.wHour || stLocal1.wDay == stLocal2.wDay&&stLocal1.wHour == stLocal2.wHour&&stLocal1.wMinute >= stLocal2.wMinute;
}
void PandaParse::ReadGrammer()
{
	int len;//文法行数
	GrammerFile >> len;
	GrammerFile >> StartSymbol;
	while (len)
	{
		std::string left, temp, tempright;
		std::vector<std::string> right;
		GrammerFile >> left >> temp >> tempright;
		std::string Temp;
		for (auto Char : tempright)
		{
			Temp.push_back(Char);
			if (Char == '>')//获得了非终结符
			{
				right.push_back(Temp);
				this->Not_Terminal.insert(Temp);
				Temp.clear();
			}
			else if (Char == ')')//获得了终结符
			{
				right.push_back(Temp);
				this->Terminal.insert(Temp);
				Temp.clear();
			}
			else if (Char == '@')
			{
				Nullable.insert(left);
				right.push_back(Temp);
				break;
			}
		}
		Grammer[left].push_back(right);
		right.push_back(left);
		IndexOfGrammer[count++] = right;
		HashOfProduction[right] = count - 1;
		len--;
	}
}
void PandaParse::CreatLR()
{
	GetFirst();
	std::set<std::string> follow;
	follow.insert("(-1#-1)");
	Project start{ this->StartSymbol, Grammer[this->StartSymbol][0], follow };
	std::set<Project> tempI;
	tempI.insert(start);
	std::set<Project> I = GetClosure(tempI);
	ProC.push_back(I);
	bool VPSisChanged = true;
	while (VPSisChanged)
	{
		VPSisChanged = false;
		int State = 0;
		size_t index = 0;
		while (index < ProC.size())
		{
			auto Set = ProC.at(index);
			State = index;
			for (auto project : Set)
			{
				if (project.IsEnd())
				{
					auto right = project.production();
					int num = HashOfProduction[right];
					auto opt = "r" + std::to_string(num);
					for (auto follow : project.follow)
					{
						if (LR[State][follow].size() > 0 && LR[State][follow] != opt)
							std::cout << "conflict" << std::endl;
						LR[State][follow] = opt;
					}
				}
			}
			for (auto Not_terminal : Not_Terminal)
			{
				auto J = Goto(Set, Not_terminal);
				if (!J.empty())
				{
					auto Jc = GetClosure(J);
					auto pos = find(ProC.begin(), ProC.end(), Jc);
					if (pos == ProC.end())
					{
						ProC.push_back(Jc);
						VPSisChanged = true;
						auto opt = "g" + std::to_string(ProC.size() - 1);
						if (LR[State][Not_terminal].size() > 0 && LR[State][Not_terminal] != opt)
							std::cout << "conflict" << std::endl;
						LR[State][Not_terminal] = opt;
					}
					else
					{
						auto k = pos - ProC.begin();
						auto opt = "g" + std::to_string(k);
						if (LR[State][Not_terminal].size() > 0 && LR[State][Not_terminal] != opt)
							std::cout << "conflict" << std::endl;
						LR[State][Not_terminal] = opt;
					}

				}
			}
			for (auto terminal : Terminal)
			{
				auto J = Goto(Set, terminal);
				if (!J.empty())
				{
					auto Jc = GetClosure(J);
					auto pos = find(ProC.begin(), ProC.end(), Jc);
					if (pos == ProC.end())
					{
						ProC.push_back(Jc);
						VPSisChanged = true;
						auto opt = "s" + std::to_string(ProC.size() - 1);
						if (LR[State][terminal].size() > 0 && LR[State][terminal] != opt)
							std::cout << "conflict" << std::endl;
						LR[State][terminal] = opt;
					}
					else
					{
						auto k = pos - ProC.begin();
						auto opt = "s" + std::to_string(k);
						if (LR[State][terminal].size() > 0 && LR[State][terminal] != opt)
							std::cout << "conflict" << std::endl;
						LR[State][terminal] = opt;
					}
				}
			}
			index++;
			Set.clear();
		}
	}
	WriteLR();
}
bool PandaParse::Reduce()
{
	bool success = true;
	std::stack<int> StateStack;
	std::stack<Node*> NodeStack;
	std::stack<Node*> tempNodeStack;
	std::stack<std::string> SymbolStack;
	StateStack.push(0);
	SymbolStack.push("#");
	Token *token = this->tokenSource->GetToken();
	while (SymbolStack.top() != StartSymbol || token->GetGTypeId () != "-1")
	{
		std::string AOG = LR[StateStack.top()]["(" + token->GetGTypeId()+"#"+token->GetUTypeId() + ")"];
		if (AOG.size() == 0)
		{
			std::cout << "语法错误" << std::endl;
			std::cout << "错误发生在：" <<token->GetLocation()<<" "<<token->ToString()<< std::endl;
			success = false;
			break;
		}
		if (AOG.at(0) == 'r')
		{
			std::string temp = AOG.substr(1, -1);
			int reduce = stoi(temp);
			int len = IndexOfGrammer[reduce].size() - 1;
			Node *tempNode = (Node*)new InternalNode();
			tempNode->SetSymbol(IndexOfGrammer[reduce].at(IndexOfGrammer[reduce].size() - 1));
			while (len)
			{
				StateStack.pop();
				SymbolStack.pop();
				tempNodeStack.push(NodeStack.top());
				NodeStack.pop();
				len--;
			}
			while (!tempNodeStack.empty())
			{
				tempNode->addNode(tempNodeStack.top());
				tempNodeStack.pop();
			}
			NodeStack.push(tempNode);
			SymbolStack.push(IndexOfGrammer[reduce].at(IndexOfGrammer[reduce].size() - 1));
			if (SymbolStack.top() != StartSymbol)
				StateStack.push(stoi(LR[StateStack.top()][SymbolStack.top()].substr(1, -1)));
		}
		else
		{
			std::string temp = AOG.substr(1, -1);
			int shift = stoi(temp);
			SymbolStack.push("(" + token->GetGTypeId() +"#"+ token->GetUTypeId() + ")");
			StateStack.push(shift);
			Node *tempNode = (Node*)new TreeNode();
			NodeStack.push(tempNode);
			tempNode->SetToken(token);

			token = this->tokenSource->GetToken();
		}
	}
	if (success) {
		this->RootNode = (InternalNode*)NodeStack.top();
	}
	return success ;
}
void PandaParse::ReadLR()
{
	std::string len1;
	int len = 1;
	int state = 0;
	getline(LRIn, len1);
	getline(LRIn, StartSymbol);
	len = stoi(len1);
	std::string sentence;
	while (len > 0)
	{
		getline(LRIn, sentence);
		std::string temp, temp1;
		for (auto iter = sentence.begin() + 2; iter != sentence.end(); iter++)
		{
			if (*iter == ':')
			{
				temp.append(temp1);
				temp1.clear();
			}
			else if (*iter == ' ')
			{
				LR[state][temp] = temp1;
				temp.clear();
				temp1.clear();
			}
			else
			{
				temp1.push_back(*iter);
			}
		}
		len--;
		state++;
		sentence.clear();
		temp.clear();
		temp1.clear();
	}
	LRIn.close();
}
void PandaParse::GetFirst()
{
	for (auto item : Terminal)
	{
		First[item].insert(item);
	}
	bool isChanged = true;
	while (isChanged)
	{
		isChanged = false;
		for (auto Not_terminal : this->Not_Terminal)//对于每一个非终结符
		{
			for (auto production : Grammer[Not_terminal])//对于每一个非终结符对应的产生式
			{
				for (auto symbol : production)//对于产生式中的每一个文法符号
				{
					if (find(Terminal.begin(), Terminal.end(), symbol) != Terminal.end())
					{
						if (First[Not_terminal].find(symbol) == First[Not_terminal].end())
							isChanged = true;
						First[Not_terminal].insert(symbol);
						break;
					}
					else
					{
						for (auto item : First[symbol])
						{
							if (First[Not_terminal].find(item) == First[Not_terminal].end())
								isChanged = true;
							First[Not_terminal].insert(item);
						}
						if (find(Nullable.begin(), Nullable.end(), symbol) == Nullable.end())//该文法符号不能为空
							break;
					}
				}
			}
		}
	}
}
std::set<std::string> PandaParse::GetSFirst(std::vector<std::string> sentence)
{
	std::set<std::string> first;
	for (int i = 1; i <= atoi(sentence.at(0).c_str()); i++)
	{
		if (Terminal.find(sentence.at(i)) != Terminal.end())
		{
			first.insert(sentence.at(i));
			break;
		}
		else if (Not_Terminal.find(sentence.at(i)) != Not_Terminal.end() && Nullable.find(sentence.at(i)) == Nullable.end())
		{
			for (auto s : First[sentence.at(i)])
			{
				first.insert(s);
			}
			break;
		}
		else
		{
			for (auto s : First[sentence.at(i)])
			{
				first.insert(s);
			}
		}
	}
	return first;
}
std::set<Project> PandaParse::GetClosure(std::set<Project> I) 
{
	std::set<Project> J;
	for (auto project : I)
		J.insert(project);
	bool isChanged = true;
	while (isChanged)
	{
		isChanged = false;
		for (auto project : J)
		{
			if (project.isLastSymbol())
			{
				auto temp = Grammer[project.GetSymbol()];
				for (auto right : temp)
				{
					Project tempProject{ project.GetSymbol(),right, project.follow };
					if (J.find(tempProject) == J.end())
					{
						J.insert(tempProject);
						isChanged = true;
					}
				}
			}
			else if (project.IsEnd())
				continue;
			else
			{
				auto tempVector = project.ReturnLast();
				auto follow = GetSFirst(tempVector);
				auto temp = Grammer[project.GetSymbol()];
				for (auto right : temp)
				{
					Project tempProject{ project.GetSymbol(),right, follow };
					if (J.find(tempProject) == J.end())
					{
						J.insert(tempProject);
						isChanged = true;
					}
				}
			}
		}
	}
	return J;
}
std::set<Project> PandaParse::Goto(std::set<Project>I, std::string x)
{
	std::set<Project> J;
	for (auto project : I)
	{
		if (!project.IsEnd())
		{
			if (project.GetSymbol() == x)
			{
				auto temp = project.ReturnNextProject();
				J.insert(temp);
			}
		}
	}
	return J;
}
void PandaParse::WriteLR()
{
	LRFile << ProC.size() << std::endl;
	LRFile << StartSymbol << std::endl;
	Terminal.insert("(-1#-1)");
	for (size_t state = 0; state < ProC.size(); state++)
	{
		std::string sentence;
		sentence.append(std::to_string(state) + " ");
		for (auto terminal : Terminal)
		{
			if (LR[state][terminal].size() > 0)
				sentence.append(terminal + ":" + LR[state][terminal] + " ");
		}
		for (auto Not_terminal : Not_Terminal)
		{
			if (LR[state][Not_terminal].size() > 0)
				sentence.append(Not_terminal + ":" + LR[state][Not_terminal] + " ");
		}
		LRFile << sentence << std::endl;
	}
	LRFile.close();
}
	InternalNode* PandaParse::GetAST()
{
	if (this->fileisChanged)
	{
		ReadGrammer();
		CreatLR();
	}
	else
	{
		ReadGrammer();
		ReadLR();
	}
	
	return Reduce()? RootNode:nullptr;
}