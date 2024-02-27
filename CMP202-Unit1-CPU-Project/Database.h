#pragma once
#include "Table.h"

class Database
{
public:
	Table* addTable(std::string tableName);

	void setColTypes(std::string tableName, std::vector<Table::DataType> colDataTypes);
	void setColHeaders(std::string tableName, std::vector<std::string> colHeaders);

	Table* getDirectTableReference(std::string tableName);

	std::string readDBFile(std::string filePath);

	std::string processCommand(std::string command);
private:
	std::vector<Table> tables;
};

