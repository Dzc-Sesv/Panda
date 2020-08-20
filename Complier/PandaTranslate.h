#pragma once
#include"Translate.h"
#include"Node.h"
#include"ASTSource.h"
#include<map>
struct VariableDetail
{
	std::string Type;
	bool isAssign = false;
	std::string value;
};
struct  FunctionDetail
{
	std::string ReturnType;
	std::vector<std::string> ParaType;
};
struct ArrayDetail
{
	std::string Type;
	int Range = 0;
	std::vector<std::string> values;
};
class PandaTranslate:public Translate
{
private:
	Node *root;
	virtual std::vector<std::pair<int, std::vector<std::string>>> TranslateToCode();
	PandaTranslate() {};
	bool DFS(Node *root);
	std::vector<std::map<std::string, VariableDetail>> VarTable;
	std::map<std::string, FunctionDetail> FunTable;
	std::vector<std::map<std::string, ArrayDetail>> ArrTable;
	int NXQ = 1;
public:
	void CreateTable();
	void PopTable();
	void InsertVariable(std::string name,ArrayDetail detail);
	void InsertVariable(std::string name,VariableDetail detail);
	std::pair<int, std::vector<std::string>> GenCode(std::string a, std::string b,std::string c, std::string d);
	PandaTranslate(ASTSource *astSource) {
		root = astSource->GetAST();
	}
};

