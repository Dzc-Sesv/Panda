#include "PandaTranslate.h"
bool PandaTranslate::DFS(Node *root) {

	if (!root->HasChild()) {
		if (root->ReturnSymbol() == "{") {
			CreateTable();
		}
		else if (root->ReturnSymbol() == "}"){
			PopTable();
		}
		return true;
	}
	else{
		for (int index = 0; index < root->ReturnSize(); index++) {
			DFS(root->At(index));
		}
		if (root->ReturnSymbol() == "<VariableDefination>") {
			VariableDetail detail;
			detail.Type = root->At(0)->At(0)->ReturnSymbol();
			InsertVariable(root->At(1)->ReturnSymbol(),detail);
		}
		else if (root->ReturnSymbol() == "")
		{

		}
		else if (root->ReturnSymbol() == "") {

		}
		else
		{

		}
		return true;
	}
}
std::vector<std::pair<int, std::vector<std::string>>> PandaTranslate::TranslateToCode() {
	 std::vector<std::pair<int, std::vector<std::string>>> temp;
	 return DFS(this->root) ? root->ReturnCode() : temp;
}
void PandaTranslate::CreateTable()
{
	std::map<std::string, VariableDetail> top;
	this->VarTable.push_back(top);
	std::map<std::string, ArrayDetail> TOP;
	this->ArrTable.push_back(TOP);
}
void PandaTranslate::PopTable()
{
	this->VarTable.pop_back();
	this->ArrTable.pop_back();
}
void PandaTranslate::InsertVariable(std::string name, ArrayDetail detail)
{
	this->ArrTable.back()[name] = detail;
}
void PandaTranslate::InsertVariable(std::string name,VariableDetail detail)
{
	this->VarTable.back()[name] = detail;
}
std::pair<int, std::vector<std::string>> PandaTranslate::GenCode(std::string a, std::string b, std::string c, std::string d)
{
	std::vector<std::string> temp{a,b,c,d};
	return std::make_pair(NXQ,temp);
}