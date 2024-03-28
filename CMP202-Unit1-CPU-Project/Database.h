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

	// Function for adding a table
	Table* addTable(std::string tableName);

	// Function for setting the column types of a specific table
	void setColTypes(std::string tableName, std::vector<Table::DataType> colDataTypes);
	// Function for setting the column headers of a specific table
	void setColHeaders(std::string tableName, std::vector<std::string> colHeaders);

	// Get a pointer to the table object based on its name
	// Data can move based on expanding vector when tables are added
	Table* getDirectTableReference(std::string tableName);


	// Function for reading out a file and filling the database
	// Wipes the database before hand
	std::string readDBFile(std::string filePath);
	// Function for saving the database to a file
	std::string writeDBFile(std::string filePath);

	// Main function to be used which processes the command given to it. 
	std::string processCommand(std::string command);
private:
	// Private functions

	// Quicksort functions
	// Function which the command processor calls, starts the parallel sort
	std::string sortTableParallel(Table* desiredTable, std::string columnName, std::string sortOrder);
	// Quicksort partition function
	int quicksortPartition(Table* table, int begin, int end, int colIndex, bool isDesc); 
	// Quicksort recursive function, threads are created on this function
	void quicksortFunc(Table* table, int begin, int end, int colIndex, int depth, bool isDesc);

	// Parallel search function, called by command processor. Creates threads with a lambda function
	std::string searchTableParallel(Table* desiredTable, int colIndex, std::vector<uint8_t> dataToFind);

	// Inner join function 
	std::string innerJoin(Table* leftTable, Table* rightTable, Table* resultsTable, int leftKeyCol, int rightKeyCol);
	// First part of the inner join, has 3/4th of the threads working here searching for matched rows (producer)
	void IJ_MatchRows(Table* leftTable, Table* rightTable, int leftKeyCol, int rightKeyCol, Channel<std::pair<int, int>>* dataOut);
	// Second part of the inner join, has 1/4th of the threads working here adding data to the new table (consumer)
	void IJ_UpdateResults(Table* leftTable, Table* rightTable, Channel<std::pair<int, int>>* dataIn, Table* resultsTable);

	// Vector of tables in the database
	std::vector<Table> tables;

	// Settings
	void editSettings(std::string setting, std::string newValue);
	bool set_logTime;
	int set_threadCount;
	int set_searchBlockMult;

	// Multithreading global data
	std::atomic<int> threadsCreatedThisAlgo; // Atomic integer which stores the current created thread count. Atomic as multiple threads will increase this. 

	// Farm for the inner join
	struct IJ_Task {
		int startRow;
		int endRow;
	};
	std::queue<IJ_Task> IJ_Part1Farm;
	std::mutex IJ_Part1FarmMtx;
	std::mutex IJ_Part2ResultsMtx;

};

