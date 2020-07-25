#pragma once
#include"PandaLex.h"
#include"ASTSource.h"
#include"Node.h"
#include"TokensSource.h"
struct Project
{
	std::string left;
	std::vector<std::string> right;
	std::set<std::string> follow;
	int index = 0;
	std::vector<std::string> production()
	{
		auto temp = right;
		temp.push_back(left);
		return temp;
	}
	bool IsEnd()
	{
		return this->index == right.size();
	}
	Project ReturnNextProject()
	{
			return Project{ this->left,this->right,this->follow,this->index + 1 };
	}
	bool operator==(const Project& Right) const
	{
		return left == Right.left&&right == Right.right&&follow == Right.follow&&index == Right.index;
	}
	bool operator<(const Project &p)const
	{
		if (right < p.right) return true;
		if (right > p.right) return false;
		if (left < p.left) return true;
		if (left > p.left) return false;
		if (follow < p.follow) return true;
		if (follow > p.follow) return false;
		return index < p.index;

	}
	std::string GetLeft()
	{
		return left;
	}
	std::vector<std::string> GetRight()
	{
		return right;
	}
	std::string GetSymbol()
	{
		return right[index];
	}
	bool isLastSymbol()
	{
		return index == right.size() - 1;
	}
	std::vector<std::string> ReturnLast()
	{
		std::vector<std::string> temp;
		temp.push_back(std::to_string(right.size() - index - 1));
		for (int i = index + 1; i != right.size(); i++)
		{
			temp.push_back(right.at(i));
		}
		temp.insert(temp.end(), follow.begin(), follow.end());
		return temp;
	}
};
class PandaParse:public ASTSource
{
public:
	virtual InternalNode* GetAST();
	PandaParse(std::string GrammerFileName, std::string LRName, TokensSource *lex);
private:
	PandaParse();
	TokensSource * tokenSource;
	std::ifstream GrammerFile;
	std::ofstream LRFile;
	std::ifstream LRIn;
	std::string GrammerFileName;
	std::string LRName;
	std::set<std::string> Nullable;
	std::map<std::string, std::vector<std::vector<std::string>>> Grammer;
	std::map<int, std::vector<std::string>> IndexOfGrammer;
	std::map<std::vector<std::string>, int> HashOfProduction;
	std::set<std::string> Not_Terminal;
	std::set<std::string> Terminal;
	std::map<std::string, std::set<std::string>> First;
	std::vector<std::set<Project>> ProC;
	std::map<int, std::map<std::string, std::string>> LR;
	int count = 0;//²úÉúÊ½ÐòºÅ
	int TempNum = 0;
	std::string StartSymbol;
	int State = 1;
	bool fileisChanged = false;
	bool FileIsChanged();
	void ReadGrammer();
	void CreatLR();
	bool  Reduce();
	void ReadLR();
	void GetFirst();
	std::set<std::string> GetSFirst(std::vector<std::string> sentence);
	std::set<Project> GetClosure(std::set<Project> I);
	std::set<Project> Goto(std::set<Project>, std::string x);
	void WriteLR();
};

