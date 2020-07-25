#pragma once
#include<string>
class Token
{
protected:
	int GTypeId = 0;
	int UTypeId = 0;
	std::string value;
	int rowNum = 0;
	int colNum = 0;
public:
	Token(int GTypeId,int UTypeId, std::string value, int rowNum, int colNum) :GTypeId(GTypeId),UTypeId(UTypeId), value(value), rowNum(rowNum), colNum(colNum) {}
	virtual std::string ToString() {
		return this -> value;
	};
	virtual std::string GetLocation() {
		return	 "第" + std::to_string(this->rowNum) + "行	第" + std::to_string(this->colNum-this->value.size())+"列";
	}
	virtual std::string GetGTypeId() {
		return std::to_string(GTypeId);
	}
	virtual std::string GetUTypeId() {
		return std::to_string(UTypeId);
	}
};

