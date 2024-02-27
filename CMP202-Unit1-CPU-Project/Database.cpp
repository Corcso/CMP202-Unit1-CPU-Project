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
	// First split the command into its components, we will split by the space char, commands are split by ;
	std::vector <std::vector<std::string>> commandParts{ std::vector<std::string>() };
	std::string currentPartProcessing = "";
	int currentCommand = 0;
	bool currentlyIgnoringCommandChars = false; // This is true when it hits a < and false when it hits a >
	for (char c : command) {
		if (c == '<') currentlyIgnoringCommandChars = true;
		else if (c == '>') currentlyIgnoringCommandChars = false;
		else if ((c == ' ' || c == ';') && !currentlyIgnoringCommandChars) {
			commandParts[currentCommand].push_back(currentPartProcessing);
			currentPartProcessing = "";
			if (c == ';') {
				commandParts.push_back(std::vector<std::string>());
				currentCommand++;
			}
		}
		else currentPartProcessing += c;
	}
	commandParts[currentCommand].push_back(currentPartProcessing); // Push back last part

	// Loop over each command
	for (int i = 0; i < commandParts.size(); ++i) {
		// Now switch over the command parts, have to use else if here as switch doesn't support strings
		if (commandParts[i][0] == "PEEK") {
			// Peek follows the following format
			// PEEK {table name} 

			// Make sure command is correct length
			if (commandParts[i].size() != 2) return "INVALLID ARGUMENT COUNT";

			// Get table via name and then display the first 10 rows, with column headers
			Table* desiredTable = getDirectTableReference(commandParts[i][1]);
			// If found table cout its first 10 rows
			if (desiredTable) std::cout << desiredTable->getStringFormattedOfTableData(0, 10, true);
			else return "NO TABLE FOUND";
		}
		else if (commandParts[i][0] == "ADDROW") {
			// Add row follows the following format
			// ADDROW {table name} {data col 1} {data col 2}... 

			// Make sure command is correct length (Has to be 2+)
			if (commandParts[i].size() < 2) return "INVALLID ARGUMENT COUNT";

			// Get table via name and then display the first 10 rows, with column headers
			Table* desiredTable = getDirectTableReference(commandParts[i][1]);
			// Make sure table exists
			if (!desiredTable) return "TABLE "+ commandParts[i][1] +" NOT FOUND";

			std::vector<Table::DataType> colDataTypes = desiredTable->getColTypes();

			// Make sure argument count matches table number of columns + 2(ADDROW and table name) or is exactly 2(adds blank row)
			if (commandParts[i].size() != 2 && commandParts[i].size() != (2 + colDataTypes.size())) return "INVALLID ARGUMENT COUNT";

			// Create a blank row and for every column loop over and add the data
			// Loop wont run if for blank row
			int rowIndex = desiredTable->addBlankRow();
			for (int col = 0; col < commandParts[i].size() - 2; ++col) {
				desiredTable->setCellData(Table::convertStringToData(colDataTypes[col], commandParts[i][col + 2]), rowIndex, col);
			}
		}
		else if (commandParts[i][0] == "MULTIADDROW") {
			// Multi add row follows the following format
			// MULTIADDROW {table name} {data col 1 row 1} {data col 2 row 1}... {data col 1 row 2} {data col 2 row 2}...

			// Make sure command is correct length (Has to be 3+)
			if (commandParts[i].size() < 3) return "INVALLID ARGUMENT COUNT";

			// Get table via name and then display the first 10 rows, with column headers
			Table* desiredTable = getDirectTableReference(commandParts[i][1]);
			// Make sure table exists
			if (!desiredTable) return "TABLE " + commandParts[i][1] + " NOT FOUND";

			std::vector<Table::DataType> colDataTypes = desiredTable->getColTypes();

			// Make sure argument count matches table number of columns + 2(ADDROW and table name) or is exactly 2(adds blank row)
			if ((commandParts[i].size() - 2) % colDataTypes.size() != 0) return "INVALLID ARGUMENT COUNT";

			// Create a blank row and for every column loop over and add the data
			// Loop wont run if for blank row
			for (int row = 0; row < commandParts[i].size() / colDataTypes.size(); ++row) {
				int rowIndex = desiredTable->addBlankRow();
				for (int col = 0; col < colDataTypes.size(); ++col) {
					desiredTable->setCellData(Table::convertStringToData(colDataTypes[col], commandParts[i][(row * colDataTypes.size()) + col + 2]), rowIndex, col);
				}
			}
		}
		else if (commandParts[i][0] == "HELP") {
			// HELP command give information about other commands and data types
			// MULTIADDROW {table name} {data col 1 row 1} {data col 2 row 1}... {data col 1 row 2} {data col 2 row 2}...

			if (commandParts[i].size() > 1 && commandParts[i][1] == "TYPES") {
				std::cout << "Below is a list of available column types: \n"
					<< "INT_32 | Any number between -2,147,483,648 and 2,147,483,647 inclusive \n"
					<< "STRING_255 | Any text 255 chars long or less \n";
			}
			else {
				std::cout << "Below is a list of available commands: \n"
					<< "HELP | This command, displays all commands \n"
					<< "HELP TYPES | Details all column types \n"
					<< "Table commands\n"
					<< "PEEK {table name} | Shows the first 10 rows of a table\n"
					<< "ADDROW {table name} {data col 1} {data col 2}...  | Adds a row of data\n"
					<< "MULTIADDROW {table name} {data col 1 row 1} {data col 2 row 1}... {data col 1 row 2} {data col 2 row 2}... | Adds multiple rows of data\n";
			}
		}
		else {
			return "INVALID COMMAND " + commandParts[i][0];
		}
	}
	
	return "";
}