#pragma once
#include <vector>
#include <string>
class Table
{
public:
	enum class DataType : uint8_t {
		INT_32, // 4 Bytes Long
		STRING_255, // 255 Bytes Long
		DATETIME // 4 Bytes Long
	};

	Table(std::string name);

	std::string getTableName();

	void setColTypes(std::vector<DataType>);
	void setColHeaders(std::vector<std::string>);

	std::vector<DataType> getColTypes();
	std::vector<std::string> getColHeaders();

	int addBlankRow();
	void setCellData(std::vector<uint8_t> newData, int rowIndex, int colIndex);

	std::string getStringFormattedOfTableData(int startRowIndex, int endRowIndex, bool displayHeaders = true);

	int getDataArrayIndexFromRowCol(int rowIndex, int colIndex);

	void pushDirectData(uint8_t byte);
	void directSetRows(int rowCount);
	int getRowCount();
	std::vector<uint8_t>* getDataVectorPointer();

	static std::vector<uint8_t> convertStringToData(DataType dataType, std::string stringToConvert);
	static std::string convertDataToString(DataType dataType, std::vector<uint8_t> data);
	static DataType convertStringToDataType(std::string dataTypeAsString);
private:
	std::string name;
	std::vector<DataType> colDataType;
	std::vector<std::string> colHeaders;
	std::vector<uint8_t> data;

	int rowWidth; // If we store this is means less computation as we can calculate it on column data type change. 
	int rowCount, colCount; // We store a row count and a col count, these can be used. 
};

