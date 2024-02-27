#include "Database.h"
#include "iostream"

void Database::addTable(std::string tableName)
{
	tables.push_back(Table(tableName));
}

void Database::setColTypes(std::string tableName, std::vector<Table::DataType> colDataTypes)
{
	// Find table and set its column types
	for (Table& table : tables) {
		if (table.getTableName() == tableName) table.setColTypes(colDataTypes);
	}
}

void Database::setColHeaders(std::string tableName, std::vector<std::string> colHeaders)
{
	// Find table and set its column headers
	for (Table& table : tables) {
		if (table.getTableName() == tableName) table.setColHeaders(colHeaders);
	}
}

Table* Database::getDirectTableReference(std::string tableName)
{
	// Find table and return its pointer
	for (Table& table : tables) {
		if (table.getTableName() == tableName) return &table;
	}
	// If no table found return a null pointer
	return nullptr;
}

std::string Database::processCommand(std::string command)
{
	// First split the command into its components, we will split by the space char
	std::vector<std::string> commandParts;
	std::string currentPartProcessing = "";
	for (char c : command) {
		if (c == ' ') {
			commandParts.push_back(currentPartProcessing);
			currentPartProcessing = "";
		}
		else currentPartProcessing += c;
	}
	commandParts.push_back(currentPartProcessing); // Push back last part

	// Now switch over the command parts, have to use else if here as switch doesn't support strings
	if (commandParts[0] == "PEEK") {
		// Peek follows the following format
		// PEEK {table name} 
		
		// Make sure command is correct length
		if (commandParts.size() != 2) return "INVALLID ARGUMENT COUNT";

		// Get table via name and then display the first 10 rows, with column headers
		Table* desiredTable = getDirectTableReference(commandParts[1]);
		// If found table cout its first 10 rows
		if (desiredTable) std::cout << desiredTable->getStringFormattedOfTableData(0, 10, true);
		else return "NO TABLE FOUND";
	}

	return "";
}