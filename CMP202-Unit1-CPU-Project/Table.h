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

	// Returns the table's name
	std::string getTableName();

	// Sets the tables column types, should only be done when table is empty to stop errors
	void setColTypes(std::vector<DataType>);
	// Sets the tables column headers, can be done at any time so long as there is the same number of headers as types
	void setColHeaders(std::vector<std::string>);

	// Returns the column types
	std::vector<DataType> getColTypes();
	// Returns the column headers
	std::vector<std::string> getColHeaders();

	// Adds a completely blank row and returns the row count before its addition
	int addBlankRow();
	// Set a specific cell's data
	void setCellData(std::vector<uint8_t> newData, int rowIndex, int colIndex);
	// Get a specific cell's data
	std::vector<uint8_t> getCellData(int rowIndex, int colIndex);
	// Get an entire row's data
	std::vector<uint8_t> getRowData(int rowIndex);

	// Get a formatted string containing the tables data from the rows specified. 
	std::string getStringFormattedOfTableData(int startRowIndex, int endRowIndex, bool displayHeaders = true);

	// Get the start index of a certain cell in the table. 
	int getDataArrayIndexFromRowCol(int rowIndex, int colIndex);

	// Directly push a byte onto the data vector
	// No safeguards
	void pushDirectData(uint8_t byte);
	// Directly set the number of rows
	// No safeguards
	void directSetRows(int rowCount);

	// Get the number of rows 
	int getRowCount();

	// Get a pointer reference to the data vector
	// Used in advanced manipulation 
	std::vector<uint8_t>* getDataVectorPointer();

	// Static functions for data conversion
	// Converts the string to a data vector when supplied with a type
	static std::vector<uint8_t> convertStringToData(DataType dataType, std::string stringToConvert);
	// Converts the data vector to a string when supplied with a type
	static std::string convertDataToString(DataType dataType, std::vector<uint8_t> data);
	// Converts a string to a datatype enum
	static DataType convertStringToDataType(std::string dataTypeAsString);

	// Functions used in sorting 
	// Returns true if the data in row 1 is larger than the data in row 2
	bool isLarger(int colIndex, int row1, int row2);
	// Swaps the data in rows 1 and 2
	void swapRows(int row1, int row2);

private:
	std::string name; // Table name
	std::vector<DataType> colDataType; // Column types
	std::vector<std::string> colHeaders; // Column headers
	std::vector<uint8_t> data; // Table's data

	int rowWidth; // If we store this is means less computation as we can calculate it on column data type change. 
	int rowCount, colCount; // We store a row count and a col count, these can be used. 
};

