#pragma once
#include <vector>
#include <string>
class Table
{
public:
	enum class DataType {
		INT_32,
		STRING
	};


	void setColTypes(std::vector<DataType>);
	void setColHeaders(std::vector<std::string>);

	void addBlankRow();

private:
	std::vector<DataType> ColDataType;
	std::vector<std::string> ColHeaders;
	std::vector<uint8_t> data;
};

