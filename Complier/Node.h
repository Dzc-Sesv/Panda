#pragma once
#include"Token.h"
#include<string>
#include<vector>
class Node{
protected:
	Token *token;
	std::string symbol;
	std::string place;
	std::vector<std::pair<int, std::vector<std::string>>> code;
	int Fc;
	int Tc;
public:
	virtual void SetToken(Token *token) = 0;
	virtual void SetSymbol(std::string symbol) = 0;
	virtual void SetPlace(std::string place) = 0;
	virtual void addNode(Node* node) =0;
	virtual Node* At(int index) = 0;
	virtual bool HasChild() = 0;
	virtual int ReturnSize() = 0;
	virtual std::string ReturnSymbol() = 0;
};
class TreeNode:public Node
{
public:
	virtual void SetToken(Token *token) {
		this->token = token;
	}
	virtual void SetSymbol(std::string symbol) {
		this->symbol = symbol;
	}
	virtual void SetPlace(std::string place) {
		this->place = place;
	}
	virtual void addNode(Node* node) {

	}
	virtual bool HasChild() {
		return false;
	}
	virtual int ReturnSize() {
		return -1;
	}
	virtual Node* At(int index) {
		return this;
	}
	virtual std::string ReturnSymbol() {
		return this->token->ToString();
	}
};
class InternalNode:public std::vector<Node*>,public Node
{
public:
	void SetToken(Token *token) {
		this->token = token;
	}
	void SetSymbol(std::string symbol) {
		this->symbol = symbol;
	}
	void SetPlace(std::string place) {
		this->place = place;
	}
	virtual void addNode(Node* node) {
		this->push_back(node);
	}
	virtual bool HasChild() {
		return true;
	}
	virtual int ReturnSize() {
		return (*this).size();
	}
	virtual Node* At(int index) {
		return (*this)[index];
	}
	virtual std::string ReturnSymbol() {
		return this->symbol;
	}
};