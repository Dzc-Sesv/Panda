#pragma once
#include"Node.h"
class ASTSource{
protected:
	InternalNode *RootNode;
public:
	virtual InternalNode* GetAST() = 0;
};

