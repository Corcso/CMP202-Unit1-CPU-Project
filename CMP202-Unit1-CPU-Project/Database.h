#pragma once
#include "Table.h"
#include "Channel.h"
#include <atomic>

// Temp includes
#include <mutex>

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
	void quicksortFunc(Table* table, int begin, int end, int colIndex, int depth);

	std::string searchTableParallel(Table* desiredTable, int colIndex, std::vector<uint8_t> dataToFind);

	std::string findLeftJoin(Table* leftTable, Table* rightTable, int searchCol, std::vector<uint8_t> dataToFind, int leftKeyCol, int rightKeyCol);

	void FLJ_findLeftSideData(Table* desiredTable, int colIndex, std::vector<uint8_t> dataToFind, Channel<std::vector<uint8_t>>* dataOut, Table* resultsTable);
	void FLJ_findRightSideRow(Table* leftTable, Table* rightTable, int leftKeyCol, int rightKeyCol, Channel<std::vector<uint8_t>>* dataIn, Table* resultsTable);

	// Vector of tables in the database
	std::vector<Table> tables;

	// Settings
	void editSettings(std::string setting, std::string newValue);
	bool set_logTime;
	int set_threadCount;
	int set_searchBlockMult;

	// Multithreading global data
	std::atomic<int> threadsCreatedThisAlgo; // Atomic integer which stores the current created thread count. Atomic as multiple threads will increase this. 

	// Farm for the find left join
	struct FLJ_Task {
		int startRow;
		int endRow;
	};
	std::queue<FLJ_Task> FLJ_Part1Farm;
	std::queue<FLJ_Task> FLJ_Part2Farm;
	std::mutex FLJ_Part1FarmMtx;
	std::mutex FLJ_Part2FarmMtx;

	// Temp
	std::mutex coutMutex;
};

