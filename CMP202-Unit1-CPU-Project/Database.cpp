#include "Database.h"
#include "iostream"
#include "fstream"
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>

Database::Database()
{
	// Set default settings
	set_logTime = false;
	set_threadCount = std::thread::hardware_concurrency();
	set_searchBlockMult = 4;

	// Start up with current thread count as one, this is the main thread.
	threadsCreatedThisAlgo = 1;
}

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
	std::ifstream fileToRead(filePath, std::ios::binary | std::ios::ate);

	if (!fileToRead.good()) {
		return "ERROR OPENING FILE";
	}

	fileToRead.seekg(0, fileToRead.end);
	int length = fileToRead.tellg();
	fileToRead.seekg(0, fileToRead.beg);

	uint8_t* result = new uint8_t[length];
	fileToRead.read((char*)result, length);

	// Loop over the data from the file, making table objects
	int stage = 0; // 0 is getting table name, 1 is table datatypes, 2 is table headers, 3 is pre table data, 4 is table data
	// Various buffers, used to store what we are currently working on
	int miscIntBuffer = 0;
	std::vector<Table::DataType> dataTypesCol;
	std::vector<std::string> headerCol;
	std::string miscStringBuffer = "";
	Table* currentTableReference = nullptr;
	for (int b = 0; b < length; ++b){
		
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
				miscStringBuffer = "";
				break;
			case 1:
				currentTableReference->setColTypes(dataTypesCol);
				dataTypesCol.clear();
				break;
			case 2:
				currentTableReference->setColHeaders(headerCol);
				miscStringBuffer = "";
				headerCol.clear();
				break;
			}

			++stage; // Increase stage 
			continue; // Move on nothing else for this byte
		}
		else {
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
				// Stage 3 we know is exactly 8 bytes long 
				// This is the number of bytes of table data as an unsigned int
				// And the number of rows
				// We will process all 8 bytes and then skip over 7

				// We directly set the number of rows here
				currentTableReference->directSetRows(*((int*)&result[b + 4]));
				// We store the size of the data in the int buffer as it is used later
				miscIntBuffer = *((int*)&result[b]);
				b += 7;
				break;
			case 4:
				// Stage 4 is the data stage, directly push data into the table and when we reach number of bytes
				// This is kept in the mist int buffer so we will decrease this until 0 every byte processed
				// Once 0 then we go to stage 0 for a new table, if one
				currentTableReference->pushDirectData(result[b]);
				--miscIntBuffer;
				if (miscIntBuffer <= 0) {
					stage = 0;
					// Add one to b (skipping the FF seperator between tables
					++b;
				}
				break;
			}
		}

	}
	// Delete file data and close file
	fileToRead.close();
	delete[] result;

	return "";

}

std::string Database::writeDBFile(std::string filePath)
{
	// Open file stream
	std::ofstream fileToWrite(filePath, std::ios::out | std::ios::binary);

	if (!fileToWrite.good()) {
		return "ERROR OPENING FILE";
	}

	// Set seperators to be used
	const char* FF_SEPERATOR = new char{ (char)0xFF };
	const char* UNIT_SEPERATOR = new char{ (char)0x1F };
	// Loop over tables
	for (Table& table : tables) {
		// Write table name
		fileToWrite.write(table.getTableName().c_str(), table.getTableName().length());
		// Write seperator
		fileToWrite.write(FF_SEPERATOR, 1);
		// Write datatypes, using their enum values
		std::vector<Table::DataType> colDataTypes = table.getColTypes();
		fileToWrite.write((const char*)&(colDataTypes[0]), table.getColTypes().size());
		// Write seperator
		fileToWrite.write(FF_SEPERATOR, 1);
		// Write column headers, loop over them
		std::vector<std::string> colHeaders = table.getColHeaders();
		for (std::string& header : colHeaders) {
			// Write this header
			fileToWrite.write(header.c_str(), header.length());
			// Write unit seperator
			fileToWrite.write(UNIT_SEPERATOR, 1);
		}
		// Write seperator
		fileToWrite.write(FF_SEPERATOR, 1);
		// Write size of data array then the number of rows
		// Store both as variables first so we can get memory references to them and convert the pointers
		int dataSize = table.getDataVectorPointer()->size();
		int rowCount = table.getRowCount();
		fileToWrite.write((const char*)&dataSize, 4);
		fileToWrite.write((const char*)&rowCount, 4);
		// Write seperator
		fileToWrite.write(FF_SEPERATOR, 1);
		// Write all this tables data
		if(dataSize > 0) fileToWrite.write((const char*)&((*table.getDataVectorPointer())[0]), dataSize);
		// Finally write another seperator
		fileToWrite.write(FF_SEPERATOR, 1);
	}
	// Close the file stream
	fileToWrite.close();
	// Delete seperators from memory
	delete FF_SEPERATOR;
	delete UNIT_SEPERATOR;
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
			auto start = std::chrono::steady_clock::now();
			// Peek follows the following format
			// PEEK {table name} 

			// Make sure command is correct length
			if (commandParts[i].size() != 2) return "INVALLID ARGUMENT COUNT";

			// Get table via name and then display the first 10 rows, with column headers
			Table* desiredTable = getDirectTableReference(commandParts[i][1]);
			// If found table cout its first 10 rows
			if (desiredTable) { 
				std::cout << desiredTable->getStringFormattedOfTableData(0, 10, true);
			}
			else return "NO TABLE FOUND";
			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "PEEK took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
		}
		else if (commandParts[i][0] == "ADDROW") {
			auto start = std::chrono::steady_clock::now();
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
			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "ADDROW took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
		}
		else if (commandParts[i][0] == "MULTIADDROW") {
			auto start = std::chrono::steady_clock::now();
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
			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "MULTIADDROW took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
		}
		else if (commandParts[i][0] == "TABLES") {
			auto start = std::chrono::steady_clock::now();
			// Tables follows the following format
			// TABLES 

			// Make sure command is correct length
			if (commandParts[i].size() != 1) return "INVALLID ARGUMENT COUNT";

			// Loop over all tables and display their data
			std::cout << "\33[4mName\tRow Count\tSize\33[0m\n";
			for (Table& table : tables) {
				std::cout << table.getTableName() << "\t" << table.getRowCount() << "\t" << table.getDataVectorPointer()->size() / 1024.0f << "KB\n";
			}
			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "TABLES took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
		}
		else if (commandParts[i][0] == "ADDTABLE") {
			auto start = std::chrono::steady_clock::now();
			// Add table follows the following format
			// ADDTABLE {table name} {comma seperated data types} {comma seperated column headers}
			
			// Make sure command is correct length
			if (commandParts[i].size() < 4) return "INVALLID ARGUMENT COUNT";

			// Get vectors ready for column type and column headers
			std::vector<std::string> newColHeaders;
			std::vector<Table::DataType> newColDataTypes;

			// Append a comma on the end of each, this means it will also process the last element inside the loop
			commandParts[i][2] += ',';
			commandParts[i][3] += ',';

			std::string currentColumnString;
			for (char& c : commandParts[i][2]) {
				if (c == ',') {
					// Set datatype based on condition
					newColDataTypes.push_back(Table::convertStringToDataType(currentColumnString));
					currentColumnString = "";
				}
				else currentColumnString += c;
			}
			for (char& c : commandParts[i][3]) {
				if (c == ',') {
					// Set datatype based on condition
					newColHeaders.push_back(currentColumnString);
					currentColumnString = "";
				}
				else currentColumnString += c;
			}

			// Add a table with the table name provided, then use the comma seperated data for type and col names
			Table* newTable = addTable(commandParts[i][1]);
			newTable->setColHeaders(newColHeaders);
			newTable->setColTypes(newColDataTypes);

			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "ADDTABLE took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
		}
		else if (commandParts[i][0] == "SORT") {
			auto start = std::chrono::steady_clock::now();
			// Add table follows the following format
			// SORT {table name} {column name} {ASC/DSC}

			// Make sure command is correct length
			if (commandParts[i].size() != 4) return "INVALLID ARGUMENT COUNT";

			// Get table via name
			Table* desiredTable = getDirectTableReference(commandParts[i][1]);
			// Make sure table exists
			if (!desiredTable) return "TABLE " + commandParts[i][1] + " NOT FOUND";
			// Sort the table using the sort table function
			std::string error = sortTableParallel(desiredTable, commandParts[i][2], commandParts[i][3]);

			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "SORT took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
			if (error != "") return error;
			}
		else if (commandParts[i][0] == "FIND") {
			auto start = std::chrono::steady_clock::now();
			// Add table follows the following format
			// SORT {table name} {column name} {ASC/DSC}

			// Make sure command is correct length
			if (commandParts[i].size() != 4) return "INVALLID ARGUMENT COUNT";

			// Get table via name
			Table* desiredTable = getDirectTableReference(commandParts[i][1]);
			// Make sure table exists
			if (!desiredTable) return "TABLE " + commandParts[i][1] + " NOT FOUND";

			// Get column index from name
			int colIndex = 0;
			for (std::string& columnHeader : desiredTable->getColHeaders()) {
				if (columnHeader == commandParts[i][2]) break;
				++colIndex;
			}

			// Next get the data to search for in its raw data format
			std::vector<uint8_t> dataToFind = Table::convertStringToData(desiredTable->getColTypes()[colIndex], commandParts[i][3]);

			// Lastly print the output from the sort function
			// This is a temp table which stores the found rows
			std::cout << searchTableParallel(desiredTable, colIndex, dataToFind);

			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "FIND took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
			}
		else if (commandParts[i][0] == "LOAD") {
			auto start = std::chrono::steady_clock::now();
			// Load follows the following format
			// LOAD {file path}

			// Make sure command is correct length
			if (commandParts[i].size() != 2) return "INVALLID ARGUMENT COUNT";

			// Wipe all tables
			tables.clear();

			// Load file from provided file name, return what this function returns as it will error if anything is returned
			std::string error = readDBFile(commandParts[i][1]);
			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "LOAD took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
			return error;
		}
		else if (commandParts[i][0] == "DROPALL") {
			auto start = std::chrono::steady_clock::now();
			// Drop all follows the following format
			// DROPALL

			// Make sure command is correct length
			if (commandParts[i].size() != 1) return "INVALLID ARGUMENT COUNT";

			// Wipe all tables
			tables.clear();

			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "DROPALL took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
			return "";
			}
		else if (commandParts[i][0] == "SAVE") {
			auto start = std::chrono::steady_clock::now();
			// Save follows the following format
			// SAVE {file path} 

			// Make sure command is correct length
			if (commandParts[i].size() != 2) return "INVALLID ARGUMENT COUNT";

			// Write to file at provided file name, return what this function returns as it will error if anything is returned
			std::string error = writeDBFile(commandParts[i][1]);
			auto end = std::chrono::steady_clock::now();
			if (set_logTime) std::cout << "SAVE took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds to complete.\n";
			return error;
		}
		else if (commandParts[i][0] == "SETTING") {
			// Save follows the following format
			// SETTING {setting name} {setting new value} 

			// Make sure command is correct length
			if (commandParts[i].size() > 3) return "INVALLID ARGUMENT COUNT";

			// Write to file at provided file name, return what this function returns as it will error if anything is returned
			if (commandParts[i].size() == 3) editSettings(commandParts[i][1], commandParts[i][2]);
			else editSettings("", "");
		}
		else if (commandParts[i][0] == "HELP") {
			// HELP command give information about other commands and data types
			// MULTIADDROW {table name} {data col 1 row 1} {data col 2 row 1}... {data col 1 row 2} {data col 2 row 2}...

			if (commandParts[i].size() > 1 && commandParts[i][1] == "TYPES") {
				std::cout << "Below is a list of available column types: \n"
					<< "INT_32 | Any number between -2,147,483,648 and 2,147,483,647 inclusive \n"
					<< "STRING_255 | Any text 255 chars long or less \n"
					<< "DATETIME | Any date between 1900-01-01 00:00:00 and 2036-02-07 06:28:15 inclusive \n";
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

std::string Database::sortTableParallel(Table* desiredTable, std::string columnName, std::string sortOrder)
{
	// Make sure sort order is valid
	if (sortOrder != "ASC" && sortOrder != "DSC") return "INVALLID SORTING ORDER";
	// Get column data types and headers
	std::vector<Table::DataType> colDataTypes = desiredTable->getColTypes();
	std::vector<std::string> colHeaders = desiredTable->getColHeaders();
	// Find column index from header
	int colIndex = -1;
	for (int i = 0; i < colHeaders.size(); i++) {
		if (colHeaders[i] == columnName) {
			colIndex = i;
			break;
		}
	}
	// If not found return with error
	if (colIndex == -1) return "COLUMN " + columnName + " NOT FOUND";

	// We will use main thread as the first thread, then we will create other threads from this.

	int part = quicksortPartition(desiredTable, 0, desiredTable->getRowCount() - 1, colIndex);
	if (threadsCreatedThisAlgo >= set_threadCount) {
		// Run quicksort sequentially on this thread
		quicksortFunc(desiredTable, 0, part - 1, colIndex);
		quicksortFunc(desiredTable, part + 1, desiredTable->getRowCount() - 1, colIndex);
	}
	else {
		// Run quicksort parallel, we run the second half on this thread as not to waste it
		std::thread tBefore = std::thread(&Database::quicksortFunc, this, desiredTable, 0, part - 1, colIndex);
		threadsCreatedThisAlgo++;

		quicksortFunc(desiredTable, part + 1, desiredTable->getRowCount() - 1, colIndex);

		// Wait for the before thread to finish
		tBefore.join();
	}
	// Reset the threads created counter as we have finished the sort
	threadsCreatedThisAlgo = 1;
	return "";

}

int Database::quicksortPartition(Table* table, int begin, int end, int colIndex)
{
	int i = begin - 1; // count found less than the pivot
	int pivot = end;
	for (int j = begin; j < end; j++) {
		if (!table->isLarger(colIndex, j, pivot)) {
			i++;
			table->swapRows(i, j);
		}
	}
	table->swapRows(i + 1, end);
	return i + 1; // index of the pivot after swapping
}

void Database::quicksortFunc(Table* table, int begin, int end, int colIndex)
{
	if (begin < end) {
		int part = quicksortPartition(table, begin, end, colIndex);
		if (threadsCreatedThisAlgo >= set_threadCount) {
			// Run quicksort sequentially on this thread
			quicksortFunc(table, begin, part - 1, colIndex);
			quicksortFunc(table, part + 1, end, colIndex);
		}
		else {
			// Run quicksort parallel, we run the second half on this thread so we don't waste it
			std::thread tBefore = std::thread(&Database::quicksortFunc, this, table, begin, part - 1, colIndex);
			threadsCreatedThisAlgo++;

			quicksortFunc(table, part + 1, end, colIndex);

			// Wait for the before thread to finish
			tBefore.join();
		}
	}
}

std::string Database::searchTableParallel(Table* desiredTable, int colIndex, std::vector<uint8_t> dataToFind)
{
	// First create the queue and the mutex for farm useage. 
	struct Task {
		int startRow;
		int endRow;
	};
	std::queue<Task> farmQueue;
	std::mutex farmMutex;
	// Calculate the sizes of the search blocks, the last one will be the smallest. 
	int rowCount = desiredTable->getRowCount();
	int sizeOfBlock = rowCount / (set_searchBlockMult * set_threadCount);
	// Next fill the queue with tasks. 
	for (int i = sizeOfBlock; i < rowCount; i += sizeOfBlock + 1) {
		farmQueue.push(Task{ i - sizeOfBlock, i});
		// If the next block will go over the row count and we haven't captured all the rows yet
		// Add a search block from where we are until the end
		if (i + sizeOfBlock >= rowCount && i + 1 < rowCount) farmQueue.push(Task{ i + 1, rowCount - 1 });
	}
	// Create the table which will store our output
	Table resultsTable("Search Results");
	std::mutex resultsMutex;
	resultsTable.setColHeaders(desiredTable->getColHeaders());
	resultsTable.setColTypes(desiredTable->getColTypes());
	// Create our threads using a lambda func that will work on searching the table
	std::vector<std::thread*> threads(set_threadCount - 1);
	for (int i = 0; i < set_threadCount - 1; i++) {
		threads[i] = (new std::thread([&]() {
			while (true) {
				// Get the task from the farm
				farmMutex.lock();
				if (farmQueue.empty())
				{
					farmMutex.unlock();
					break;
				}
				Task myTask = farmQueue.front();
				farmQueue.pop();
				farmMutex.unlock();
				// Perform the search
				for (int row = myTask.startRow; row <= myTask.endRow; row++) {
					int indexToLook = desiredTable->getDataArrayIndexFromRowCol(row, colIndex);
					// Loop over data array, checking its equal to data in DB
					bool dataEqual = true;
					for (int b = 0; b < dataToFind.size(); b++) {
						if (dataToFind[b] != (*(desiredTable->getDataVectorPointer()))[indexToLook + b]) {
							dataEqual = false;
							break;
						}
					}
					// If the data is equal (found) add it to our results table
					if (dataEqual) {
						// Get this rows data
						std::vector<uint8_t> rowData = desiredTable->getRowData(row);
						// Lock using lock guard and add this row data to new table
						std::lock_guard<std::mutex> lockGuard(resultsMutex);
						for (int b = 0; b < rowData.size(); b++) {
							resultsTable.pushDirectData(rowData[b]);
						}
						resultsTable.directSetRows(resultsTable.getRowCount() + 1);
					}
				}
			}
			}));
	}
	// Wait for all threads to finish
	for (int i = 0; i < set_threadCount - 1; i++) {
		// Join then delete each thread. 
		threads[i]->join();
		delete threads[i];
	}
	//while (true) {
	//	// Get the task from the farm
	//	//farmMutex.lock();
	//	if (farmQueue.empty()) break;
	//	Task myTask = farmQueue.front();
	//	farmQueue.pop();
	//	//farmMutex.unlock();
	//	// Perform the search
	//	for (int row = myTask.startRow; row <= myTask.endRow; row++) {
	//		int indexToLook = desiredTable->getDataArrayIndexFromRowCol(row, colIndex);
	//		// Loop over data array, checking its equal to data in DB
	//		bool dataEqual = true;
	//		for (int b = 0; b < dataToFind.size(); b++) {
	//			if (dataToFind[b] != (*(desiredTable->getDataVectorPointer()))[indexToLook + b]) {
	//				dataEqual = false;
	//				break;
	//			}
	//		}
	//		// If the data is equal (found) add it to our results table
	//		if (dataEqual) {
	//			// Get this rows data
	//			std::vector<uint8_t> rowData = desiredTable->getRowData(row);
	//			// Lock using lock guard and add this row data to new table
	//			//std::lock_guard<std::mutex> lockGuard(resultsMutex);
	//			for (int b = 0; b < rowData.size(); b++) {
	//				resultsTable.pushDirectData(rowData[b]);
	//			}
	//			resultsTable.directSetRows(resultsTable.getRowCount() + 1);
	//		}
	//	}
	//}
	std::string resultTableView = resultsTable.getStringFormattedOfTableData(0, resultsTable.getRowCount());
	return resultTableView;
}




void Database::editSettings(std::string setting, std::string newValue)
{
	// If there is settings referenced change them
	int editedSettingNumber = 0; // Stores which setting was edited for colouring
	if (setting == "Log-Timing") {
		if (newValue == "true") set_logTime = true;
		else if (newValue == "false") set_logTime = false;
		editedSettingNumber = 1;
	}
	else if (setting == "Thread-Count") {
		set_threadCount = std::stoi(newValue);
		editedSettingNumber = 2;
	}

	// Print out all the settings
	std::cout << "\33[4mSettings\33[0m\n" <<
		((editedSettingNumber == 1) ? "\33[34m" : "") << "Log-Timing: " << ((set_logTime) ? "true" : "false") << "\33[0m" <<
		"\n\tYour computer has " << std::thread::hardware_concurrency() << " threads." <<
		((editedSettingNumber == 2) ? "\33[34m" : "") << "\nThread-Count: " << set_threadCount << "\33[0m"<<
		"\n";
}
