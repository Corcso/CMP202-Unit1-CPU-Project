#include "Database.h"

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
}

void Database::processCommand(std::string command)
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

	// Now switch over the command parts, have to use else if here as switch doesn't support strings
	if (commandParts[0] == "PEEK") {

	}
}