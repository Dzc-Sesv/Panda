#pragma once
#include"PandaParse.h"
#include"Translate.h"
class PandaCheck:public Translate
{
private:
	ASTSource *astSource;
	PandaCheck();
public:
	PandaCheck(ASTSource *astSource) {
		this->astSource = astSource;
	}
	virtual bool check() {

	}
};

