#include <iostream>
#include <vector>

#include "Database.h"
#include "Table.h"

int main() {

	/*std::vector<unsigned char> data;

	data.push_back(0x68);
	data.push_back(0x69);
	data.push_back(0x39);
	data.push_back(0x38);

	uint32_t number = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
	std::string newString = { (char)data[0], (char)data[1], (char)data[2], (char)data[3] };
	std::cout << number << " and " << newString;*/

	std::vector<Table::DataType> dataTypes{Table::DataType::INT_32, Table::DataType::STRING_255};

	// Create new database
	Database db;
	// Add table
	db.addTable("mytable");
	// Get pointer for now
	Table* myTable = db.getDirectTableReference("mytable");
	myTable->setColTypes(dataTypes);
	myTable->setColHeaders(std::vector<std::string>{"idLonglonglong", "name"});
	// Add 2 rows
	/*myTable->addBlankRow();
	myTable->addBlankRow();
	{
		std::vector<uint8_t> data{0x01, 0x00, 0x00, 0x00};
		myTable->setCellData(data, 0, 0);
	}
	{
		std::vector<uint8_t> data{ 0x02, 0x00, 0x00, 0x00 };
		myTable->setCellData(data, 1, 0);
	}
	{
		std::vector<uint8_t> data{ 0x68, 0x69, 0x39, 0x38 };
		myTable->setCellData(data, 0, 1);
	}
	{
		std::vector<uint8_t> data{ 0x65, 0x66, 0x31, 0x31 };
		myTable->setCellData(data, 1, 1);
	}*/
	//std::cout << myTable->getStringFormattedOfTableData(0, 2);
	db.processCommand("ADDROW mytable 1 hello");
	db.processCommand("ADDROW mytable 2 hi");
	db.processCommand("ADDROW mytable");
	db.processCommand("ADDROW mytable 3 yoyo");
	db.processCommand("PEEK mytable");

	return 0;
}