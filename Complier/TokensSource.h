#pragma once
#include<string>
#include<fstream>
#include<set>
#include<map>
#include<vector>
#include"Token.h"
class TokensSource
{
public:
	virtual Token* GetToken() = 0;
};

