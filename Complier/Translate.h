#pragma once
#include<vector>
class Translate
{
	virtual bool check() = 0;
	virtual std::vector<std::pair<int, std::vector<std::string>>> TranslateToCode() = 0;
	virtual std::vector<std::pair<int, std::vector<std::string>>> GetCode() {
		check();
		return TranslateToCode();
	};
};

