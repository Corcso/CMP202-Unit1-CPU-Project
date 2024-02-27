#include "Table.h"

Table::Table(std::string name)
{
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
		}
	}
}

void Table::setColHeaders(std::vector<std::string> newColHeaders)
{
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
	}

}

std::string Table::getStringFormattedOfTableData(int startRowIndex, int endRowIndex, bool displayHeaders)
{
	std::string stringToReturn;
	// If end row index more than number of rows, use number of rows instead
	if (endRowIndex > rowCount) endRowIndex = rowCount;
	// Loop over column headers, adding them if asked for
	if (displayHeaders && colHeaders.size() == colCount) {
		for (int col = 0; col < colCount; ++col) {
			stringToReturn += colHeaders[col] + "\t";
		}
		// Add new line
		stringToReturn += "\n";

		// Add line seperator NEEDS WORK
		//int headTextLength = stringToReturn.length() - 2 + (colCount * 3);
		//for (int c = 0; c < headTextLength; ++c) {
		//	stringToReturn += "-";
		//}

		//// Add new line
		//stringToReturn += "\n";
	}
	// Loop over rows
	for (int row = startRowIndex; row < endRowIndex; ++row) {
		for (int col = 0; col < colCount; ++col) {
			// For our current cell calculate where we are
			int indexOfDataStart = getDataArrayIndexFromRowCol(row, col);
			// Then based on our column's datatype convert the relevant data and add it to the string to return
			switch (colDataType[col]) {
			case DataType::INT_32:
				stringToReturn += std::to_string((int)((data[indexOfDataStart + 3] << 24) | (data[indexOfDataStart + 2] << 16) | (data[indexOfDataStart + 1] << 8) | data[indexOfDataStart]));
				break;
			case DataType::STRING_255:
				// Loop over all charaters
				for (int c = 0; c < 255; ++c) { 
					// Stop looping if hit blank (end of string)
					if (data[indexOfDataStart + c] == 0x00) break;
					// Otherwise add the current character onto the string to output
					stringToReturn += (char)data[indexOfDataStart + c];
				}
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
		}
	}
	return indexOfDataStart;
}

std::vector<uint8_t> Table::convertStringToData(DataType dataType, std::string stringToConvert)
{
	std::vector<uint8_t> data;

	switch (dataType) {
	case DataType::INT_32:
	{
		int INT_32_TypedData = std::stoi(stringToConvert);
		uint8_t* firstBytePointer = (uint8_t*)&INT_32_TypedData;
		for (int i = 0; i < 4; i++) data.push_back(firstBytePointer[i]);
	}
		break;
	case DataType::STRING_255:
	{
		const char* STRING_255_TypedData = stringToConvert.c_str();
		for (int i = 0; i < stringToConvert.length(); i++) data.push_back(STRING_255_TypedData[i]);
	}
		break;
	}

	return data;
}


