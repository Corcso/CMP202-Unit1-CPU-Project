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

	std::string leftJoin(Table* leftTable, Table* rightTable, Table* resultsTable, int leftKeyCol, int rightKeyCol);

	void LJ_MatchRows(Table* leftTable, Table* rightTable, int leftKeyCol, int rightKeyCol, Channel<std::pair<int, int>>* dataOut);
	void LJ_UpdateResults(Table* leftTable, Table* rightTable, Channel<std::pair<int, int>>* dataIn, Table* resultsTable);

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
	struct LJ_Task {
		int startRow;
		int endRow;
	};
	std::queue<LJ_Task> LJ_Part1Farm;
	std::mutex LJ_Part1FarmMtx;
	std::mutex LJ_Part2ResultsMtx;

	// Temp
	std::mutex coutMutex;
};

