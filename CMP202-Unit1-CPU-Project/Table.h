#pragma once
#include <vector>
#include <string>
class Table
{
public:
	enum class DataType {
		INT_32, // 4 Bytes Long
		STRING_255 // 255 Bytes Long
	};

	Table(std::string name);

	std::string getTableName();

	void setColTypes(std::vector<DataType>);
	void setColHeaders(std::vector<std::string>);

	void addBlankRow();
	void setCellData(std::vector<uint8_t> newData, int rowIndex, int colIndex);

	std::string getStringFormattedOfTableData(int startRowIndex, int endRowIndex);

	int getDataArrayIndexFromRowCol(int rowIndex, int colIndex);
private:
	std::string name;
	std::vector<DataType> colDataType;
	std::vector<std::string> colHeaders;
	std::vector<uint8_t> data;

	int rowWidth; // If we store this is means less computation as we can calculate it on column data type change. 
	int rowCount, colCount; // We store a row count and a col count, these can be used. 
};

