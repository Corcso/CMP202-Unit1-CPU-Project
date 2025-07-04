#include "Table.h"

// Remove below
#include <iostream>

Table::Table(std::string name)
{
	// Setup table defaults
	this->name = name;
	colCount = 0; 
	rowCount = 0;
}

std::string Table::getTableName()
{
	return name;
}

void Table::setColTypes(std::vector<DataType> newDataTypes)
{
	// Set the new data types
	colDataType = newDataTypes;

	// Set the column count
	colCount = colDataType.size();

	// Calculate the row width, if we do it here it means less computation later
	rowWidth = 0;
	for (int i = 0; i < colDataType.size(); i++) {
		switch (colDataType[i]) {
		case DataType::INT_32:
			rowWidth += 4;
			break;
		case DataType::STRING_255:
			rowWidth += 255;
			break;
		case DataType::DATETIME:
			rowWidth += 4;
			break;
		}
	}
}

void Table::setColHeaders(std::vector<std::string> newColHeaders)
{
	// Set the column headers
	colHeaders = newColHeaders;
}

std::vector<Table::DataType> Table::getColTypes()
{
	return colDataType;
}

std::vector<std::string> Table::getColHeaders()
{
	return colHeaders;
}

int Table::addBlankRow()
{
	// Push back blank data onto vector based on column type
	for (int i = 0; i < colDataType.size(); i++) {
		switch (colDataType[i]) {
		case DataType::INT_32:
			// Push back 0
			data.push_back(0x00);
			data.push_back(0x00);
			data.push_back(0x00);
			data.push_back(0x00);
			break;
		case DataType::STRING_255:
			for (int c = 0; c < 255; c++) {
				// Push back 0 (Nothing)
				data.push_back(0x00);
			}
			break;
		case DataType::DATETIME:
			// Push back 0
			data.push_back(0x00);
			data.push_back(0x00);
			data.push_back(0x00);
			data.push_back(0x00);
			break;
		}
	}

	// Add 1 to row count and return old row count
	return rowCount++;
}

void Table::setCellData(std::vector<uint8_t> newData, int rowIndex, int colIndex)
{
	// Get the index of the desired cell in the data array
	int indexOfDataStart = getDataArrayIndexFromRowCol(rowIndex, colIndex);

	// Now add the data into the vector
	switch (colDataType[colIndex]) {
	case DataType::INT_32:
		if (newData.size() != 4) return; // Return if the data is the wrong size
		for (int offset = 0; offset < 4; offset++) data[indexOfDataStart + offset] = newData[offset];
		break;
	case DataType::STRING_255:
		if (newData.size() > 255) return; // Return if the data is the wrong size
		for (int offset = 0; offset < 255; offset++) data[indexOfDataStart + offset] = (offset < newData.size()) ? newData[offset] : 0x00;
		break;
	case DataType::DATETIME:
		if (newData.size() != 4) return; // Return if the data is the wrong size
		for (int offset = 0; offset < 4; offset++) data[indexOfDataStart + offset] = newData[offset];
		break;
	}

}

std::vector<uint8_t> Table::getCellData(int rowIndex, int colIndex)
{
	// Get the index of the desired cell in the data array
	int indexOfDataStart = getDataArrayIndexFromRowCol(rowIndex, colIndex);

	// Create data to return vector
	std::vector<uint8_t> dataToReturn;

	// Now add the data into the vector
	switch (colDataType[colIndex]) {
	case DataType::INT_32:
		for (int offset = 0; offset < 4; offset++) dataToReturn.push_back(data[indexOfDataStart + offset]);
		break;
	case DataType::STRING_255:
		for (int offset = 0; offset < 255; offset++) dataToReturn.push_back(data[indexOfDataStart + offset]);
		break;
	case DataType::DATETIME:
		for (int offset = 0; offset < 4; offset++) dataToReturn.push_back(data[indexOfDataStart + offset]);
		break;
	}

	return dataToReturn;
}

std::vector<uint8_t> Table::getRowData(int rowIndex)
{
	// Get the index of the desired row in the data array
	int indexOfDataStart = getDataArrayIndexFromRowCol(rowIndex, 0);

	// Create data to return vector
	std::vector<uint8_t> dataToReturn(rowWidth);
	
	// Loop over row copying it into new vector
	for (int b = 0; b < rowWidth; b++) dataToReturn[b] = data[b + indexOfDataStart];

	// Return row data
	return dataToReturn;
}

std::string Table::getStringFormattedOfTableData(int startRowIndex, int endRowIndex, bool displayHeaders)
{
	std::string stringToReturn;
	// If end row index more than number of rows, use number of rows instead
	if (endRowIndex > rowCount) endRowIndex = rowCount;
	// Loop over column headers, adding them if asked for
	if (displayHeaders && colHeaders.size() == colCount) {
		stringToReturn += "\33[4m";
		for (int col = 0; col < colCount; ++col) {
			stringToReturn += colHeaders[col] + "\t";
		}
		// Add new line
		stringToReturn += "\33[0m\n";

	}
	// Loop over rows
	for (int row = startRowIndex; row < endRowIndex; ++row) {
		for (int col = 0; col < colCount; ++col) {
			// For our current cell calculate where we are
			int indexOfDataStart = getDataArrayIndexFromRowCol(row, col);
			// Then based on our column's datatype convert the relevant data and add it to the string to return
			std::vector<uint8_t> dataToStringify;
			switch (colDataType[col]) {
			case DataType::INT_32:
				//stringToReturn += std::to_string((int)((data[indexOfDataStart + 3] << 24) | (data[indexOfDataStart + 2] << 16) | (data[indexOfDataStart + 1] << 8) | data[indexOfDataStart]));
				for (int i = 0; i < 4; i++) dataToStringify.push_back(data[indexOfDataStart + i]);
				stringToReturn += Table::convertDataToString(DataType::INT_32, dataToStringify);
				break;
			case DataType::STRING_255:
				for (int i = 0; i < 255; i++) dataToStringify.push_back(data[indexOfDataStart + i]);
				stringToReturn += Table::convertDataToString(DataType::STRING_255, dataToStringify);
				break;
			case DataType::DATETIME:
				for (int i = 0; i < 4; i++) dataToStringify.push_back(data[indexOfDataStart + i]);
				stringToReturn += Table::convertDataToString(DataType::DATETIME, dataToStringify);
				break;
			}
			// Add the column seperator
			stringToReturn += "\t";
		}
		// Add the row seperator
		stringToReturn += "\n";
	}

	// Return the string to return
	return stringToReturn;
}

int Table::getDataArrayIndexFromRowCol(int rowIndex, int colIndex)
{
	// We can use the row width to calculate where we are
	int indexOfDataStart = rowWidth * (rowIndex); // Place us at the start of the row we are on
	// Then loop over until the index of our cell
	for (int i = 0; i < colIndex; i++) {
		switch (colDataType[i]) {
		case DataType::INT_32:
			indexOfDataStart += 4;
			break;
		case DataType::STRING_255:
			indexOfDataStart += 255;
			break;
		case DataType::DATETIME:
			indexOfDataStart += 4;
			break;
		}
	}
	return indexOfDataStart;
}

void Table::pushDirectData(uint8_t byte)
{
	data.push_back(byte);
}

void Table::directSetRows(int rowCount)
{
	this->rowCount = rowCount;
}

int Table::getRowCount()
{
	return rowCount;
}

std::vector<uint8_t>* Table::getDataVectorPointer()
{
	return &data;
}

std::vector<uint8_t> Table::convertStringToData(DataType dataType, std::string stringToConvert)
{
	// Create the output vector
	std::vector<uint8_t> data;
	// Switch over the datatypes 
	switch (dataType) {
	case DataType::INT_32:
	{
		// If int, convert string to int and read data from memory direct into data vector
		int INT_32_TypedData = std::stoi(stringToConvert);
		uint8_t* firstBytePointer = (uint8_t*)&INT_32_TypedData;
		for (int i = 0; i < 4; i++) data.push_back(firstBytePointer[i]);
	}
		break;
	case DataType::STRING_255:
	{
		// If string read each character into data vector
		const char* STRING_255_TypedData = stringToConvert.c_str();
		for (int i = 0; i < stringToConvert.length(); i++) data.push_back(STRING_255_TypedData[i]);
	}
		break;
	case DataType::DATETIME:
	{
		// Datetime has a complex conversion
		// Split string at space for date and time components
		std::vector<int> timeComponents; // Stored: year, month, day, hour, minute, second
		std::string currentPartProcessing = "";
		for (char c : stringToConvert) {
			if (c == ' ' || c == ':' || c == '-') {
				timeComponents.push_back(std::stoi(currentPartProcessing));
				currentPartProcessing = "";
			}
			else currentPartProcessing += c;
		}
		timeComponents.push_back(std::stoi(currentPartProcessing));

		// Create timestamp initially at 0
		unsigned int timestamp = 0;

		// Loop until we reach 1900/1/1 counting down days, ignoring time. 
		while (!(timeComponents[0] == 1900 && timeComponents[1] == 1 && timeComponents[2] == 1)) {
			--timeComponents[2];
			// If day is below 1 we are onto the month before, so check what day we should end up on
			if (timeComponents[2] < 1) {
				// Check for going into 31 day months (bar december)
				if (timeComponents[1] == 2 || timeComponents[1] == 4 || timeComponents[1] == 6 || timeComponents[1] == 8 || timeComponents[1] == 9 || timeComponents[1] == 11) {
					--timeComponents[1];
					timeComponents[2] = 31;
				}
				// Check for going into 30 day months
				else if (timeComponents[1] == 5 || timeComponents[1] == 7 || timeComponents[1] == 10 || timeComponents[1] == 12) {
					--timeComponents[1];
					timeComponents[2] = 30;
				}
				// Check for going into february
				else if (timeComponents[1] == 3) {
					// Check if leap year
					if (timeComponents[0] % 400 == 0 || (timeComponents[0] % 100 != 0 && timeComponents[0] % 4 == 0)) {
						--timeComponents[1];
						timeComponents[2] = 29;

					}
					else{
						--timeComponents[1];
						timeComponents[2] = 28;
					}
				}
				// Else we are in going into december so roll under year
				else {
						--timeComponents[0];
						timeComponents[1] = 12;
						timeComponents[2] = 31;
				}
			}
			// Every day we decrease add 86400 seconds to timestamp
			timestamp += 86400;
		}
		// Now add time onto time stamp, time is consistant accross days so can be added here
		timestamp += (timeComponents[3]) * 3600 + (timeComponents[4] * 60) + (timeComponents[5] * 60);
		// Finally convert timestamp to byte array
		uint8_t* firstBytePointer = (uint8_t*)&timestamp;
		for (int i = 0; i < 4; i++) data.push_back(firstBytePointer[i]);
	}
	break;
	}

	return data;
}

std::string Table::convertDataToString(DataType dataType, std::vector<uint8_t> data)
{
	// Create the output string
	std::string stringToReturn;
	// Switch over each datatype
	switch (dataType) {
	case DataType::INT_32:
	{
		// For ints cast the byte pointer to an int pointer and then convert that int to a string
		int INT_32_TypedData = *((int*)&data[0]);
		stringToReturn += std::to_string(INT_32_TypedData);
	}
	break;
	case DataType::STRING_255:
	{
		// Loop over all charaters
		for (int c = 0; c < 255; ++c) {
			// Stop looping if hit blank (end of string)
			if (data[c] == 0x00) break;
			// Otherwise add the current character onto the string to output
			stringToReturn += (char)data[c];
		}
	}
	break;
	case DataType::DATETIME:
	{
		// This works the same as string -> data but counting backwards
		unsigned int DATETIME_TypedData = *((uint32_t*)&data[0]);
		unsigned int daysSinceEpoch = DATETIME_TypedData / 86400;
		int year = 1900;
		int month = 1; 
		int day = 1;
		for (int i = 0; i < daysSinceEpoch; i++) {
			++day;
			// If day is 28 or over start checking if we are into next month
			if (day >= 28) {
				// Check for 31 day months (bar december)
				if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10) {
					if (day > 31) {
						++month;
						day = 1;
					}
				}
				// Check for 30 day months
				else if (month == 4 || month == 6 || month == 9 || month == 11) {
					if (day > 30) {
						++month;
						day = 1;
					}
				}
				// Check for february
				else if (month == 2) {
					// Check if leap year
					if (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0)) {
						if (day > 29) {
							++month;
							day = 1;
						}
					}
					else if (day > 28) {
						++month;
						day = 1;
					}
				}
				// Else we are in december so roll over year if needed
				else {
					if (day > 31) {
						++year;
						month = 1;
						day = 1;
					}
				}
			}
		}
		// Now calculate time which should be easy as time is consistant accross days
		int hour = (DATETIME_TypedData - (daysSinceEpoch * 86400)) / 3600;
		int minute = (DATETIME_TypedData - (daysSinceEpoch * 86400) - (hour * 3600)) / 60;
		int second = DATETIME_TypedData - (daysSinceEpoch * 86400) - (hour * 3600) - (minute * 60);
		// Finally format this in the string
		stringToReturn += 
			std::to_string(year) + "-" + ((month < 10) ? "0" : "") + std::to_string(month) + "-" + ((day < 10) ? "0" : "") + std::to_string(day)
			+ " " + 
			((hour < 10) ? "0" : "") + std::to_string(hour) + ":" + ((minute < 10) ? "0" : "") + std::to_string(minute) + ":" + ((second < 10) ? "0" : "") + std::to_string(second);
	}
	break;
	}

	return stringToReturn;
}

Table::DataType Table::convertStringToDataType(std::string dataTypeAsString)
{
	// Return the relevant datatype enum based on string input
	if (dataTypeAsString == "INT_32") {
		return DataType::INT_32;
	}
	else if (dataTypeAsString == "STRING_255") {
		return DataType::STRING_255;
	}
	else if (dataTypeAsString == "DATETIME") {
		return DataType::DATETIME;
	}
	// If string doesn't match just default to int
	return DataType::INT_32;

}

bool Table::isLarger(int colIndex, int row1, int row2)
{
	// Get the data at each cell to compare
	std::vector<uint8_t> dataAtRow1 = getCellData(row1, colIndex);
	std::vector<uint8_t> dataAtRow2 = getCellData(row2, colIndex);
	// Switch over each vector
	switch (colDataType[colIndex]) {
	case DataType::INT_32:
	{
		// Convert row data back to ints and compare
		int data1 = *(int*)&(dataAtRow1[0]);
		int data2 = *(int*)&(dataAtRow2[0]);
		return (data1 > data2);
	}
	case DataType::STRING_255:
	{
		//Convert row data back to strings and compare
		std::string data1;
		std::string data2;
		for (int offset = 0; offset < 255; offset++) data1 += (char)dataAtRow1[offset];
		for (int offset = 0; offset < 255; offset++) data2 += (char)dataAtRow2[offset];
		return (data1 > data2);
	}
	case DataType::DATETIME:
	{
		// Convert row data back to ints and compare (we can just compare the ints and the later date will be higher)
		int data1 = *(int*)&(dataAtRow1[0]);
		int data2 = *(int*)&(dataAtRow2[0]);
		return (data1 > data2);
	}
	}

	return false;
}

void Table::swapRows(int row1, int row2)
{
	// Get the starting index of both rows
	int row1StartingIndex = getDataArrayIndexFromRowCol(row1, 0);
	int row2StartingIndex = getDataArrayIndexFromRowCol(row2, 0);

	// Get vector to temp store the data of row 1
	std::vector<uint8_t> tempDataRow1(rowWidth);

	// Copy row1's data into the temp data store
	for (int i = 0; i < rowWidth; i++) tempDataRow1[i] = data[row1StartingIndex + i];
	// Copy row2's data into row 1
	for (int i = 0; i < rowWidth; i++) data[row1StartingIndex + i] = data[row2StartingIndex + i];
	// Copy row1's data into row 2
	for (int i = 0; i < rowWidth; i++) data[row2StartingIndex + i] = tempDataRow1[i];

	return;
	
}


