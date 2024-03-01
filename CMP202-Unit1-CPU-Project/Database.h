#pragma once
#include "Table.h"
#include <atomic>

class Database
{
public:
	Database();

	Table* addTable(std::string tableName);

	void setColTypes(std::string tableName, std::vector<Table::DataType> colDataTypes);
	void setColHeaders(std::string tableName, std::vector<std::string> colHeaders);

	Table* getDirectTableReference(std::string tableName);

	std::string readDBFile(std::string filePath);
	std::string writeDBFile(std::string filePath);

	std::string processCommand(std::string command);
private:
	// Private functions
	std::string sortTableParallel(Table* desiredTable, std::string columnName, std::string sortOrder);
	int quicksortPartition(Table* table, int begin, int end, int colIndex);
	void quicksortFunc(Table* table, int begin, int end, int colIndex);

	// Vector of tables in the database
	std::vector<Table> tables;

	// Settings
	void editSettings(std::string setting, std::string newValue);
	bool set_logTime;
	int set_threadCount;

	// Multithreading global data
	std::atomic<int> currentThreadCount; // Atomic integer which stores the current live thread count. Atomic as multiple threads will increase and decrease this. 
};

