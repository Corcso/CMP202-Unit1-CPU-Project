#include "Database.h"
#include "iostream"
#include "fstream"

Table* Database::addTable(std::string tableName)
{
	tables.push_back(Table(tableName));
	return &tables[tables.size() - 1];
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

std::string Database::readDBFile(std::string filePath)
{
	// CODE FOR READING BINARY TO VECTOR OF UINT8_T
	// BY theformigoni
	// ACCESSED AT https://godbolt.org/z/ebnPKqz9Y
	// ACCESSED ON 27/02/2024
	
	// Creats an ifstream object and opens the file given by the argument 'file_name'
	// Opens in binary mode so that there are no misinterpretation of bytes
	// std::ios::ate changes the seek pointer to the end of the file
	std::ifstream input_file(filePath, std::ios::binary | std::ios::ate);

	if (!input_file.good()) { 
		std::cout << "Error with file" << input_file.rdstate() << '\n';
		exit(1);
	}

	// Returns the seek pointer of the file
	// The seek points to the end of the file because of 'std::ios::ate'
	std::streampos position{ input_file.tellg() };

	input_file.seekg(0, std::ios::beg);

	// Create the vector we wish to return
	// We need to cast so that the program compiles
	std::vector<uint8_t> result{ static_cast<unsigned char>(position) };

	std::cout << "length: " << position << '\n';

	result.insert(result.begin(),
		std::istream_iterator<uint8_t>(input_file),
		std::istream_iterator<uint8_t>());

	// END OF CITED CODE

	// Loop over the data from the file, making table objects
	int stage = 0; // 0 is getting table name, 1 is table datatypes, 2 is table headers, 3 is pre table data, 4 is table data
	// Various buffers, used to store what we are currently working on
	int miscIntBuffer = 0;
	std::vector<Table::DataType> dataTypesCol;
	std::vector<std::string> headerCol;
	std::string miscStringBuffer = "";
	Table* currentTableReference = nullptr;
	for (int b = 0; b < result.size(); ++b){ 
		
		// If we are on a unit seperator and not stage 4
		if (result[b] == 0x1F && stage != 4) {
			// Switch case over stages that use this seperator (like header columns to seperate each header)
			switch (stage)
			{
			case 2:
				headerCol.push_back(miscStringBuffer);
				miscStringBuffer = "";
				break;
			}

		}
		// Check if we are on a full byte, don't do this for step 4 though, we will use table data size in stage 3 for this.
		// Using full byte not group seperator (0x1D) so we can have up to 255 different data types and not have messy enum numbers
		else if (result[b] == 0xFF && stage != 4) { // 
			// Switch over each stage, doing the nessicary things with the buffers, like adding headers

			switch (stage)
			{
			case 0:
				currentTableReference = addTable(miscStringBuffer);
				break;
			case 1:
				currentTableReference->setColTypes(dataTypesCol);
				break;
			case 2:
				currentTableReference->setColHeaders(headerCol);
				break;
			}

			++stage; // Increase stage 
			continue; // Move on nothing else for this byte
		}

		// For each byte switch over stage as this decides the buffer
		switch (stage)
		{
		case 0:
			miscStringBuffer += (char)result[b]; // Store table name in string buffer
			break;
		case 1:
			dataTypesCol.push_back((Table::DataType)result[b]); // Enum is used so we can directly convert from number to enum
			break;
		case 2:
			miscStringBuffer += (char)result[b]; // Add each letter of the current header to string buffer, Unit seperator will seperate the column names
			break;
		case 3:
			// Stage 3 we know is exactly 4 bytes long this is the number of bytes of table data as an unsigned int
			// We will process all 4 bytes and then skip over 3
			miscIntBuffer = *((int*)&result[b]);
			b += 3;
			break;
		case 4:
			// Stage 4 is the data stage, directly push data into the table and when we reach number of bytes
			// This is kept in the mist int buffer so we will decrease this until 0 every byte processed
			// Once 0 then we go to stage 0 for a new table, if one
			currentTableReference->pushDirectData(result[b]);
			--miscIntBuffer;
			if (miscIntBuffer <= 0) {
				stage = 0;
			};
		}

	}
	return "";

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