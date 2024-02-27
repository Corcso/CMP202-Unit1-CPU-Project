#include <iostream>
#include <vector>

#include "Database.h"
#include "Table.h"

#include <conio.h>

int main() {

	/*std::vector<unsigned char> data;

	data.push_back(0x68);
	data.push_back(0x69);
	data.push_back(0x39);
	data.push_back(0x38);

	uint32_t number = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
	std::string newString = { (char)data[0], (char)data[1], (char)data[2], (char)data[3] };
	std::cout << number << " and " << newString;*/

	std::vector<Table::DataType> dataTypes{Table::DataType::INT_32, Table::DataType::DATETIME };

	// Create new database
	Database db;
	// Add table
	//db.addTable("mytable");
	//// Get pointer for now
	//Table* myTable = db.getDirectTableReference("mytable");
	//myTable->setColTypes(dataTypes);
	//myTable->setColHeaders(std::vector<std::string>{"id", "date"});
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
	//db.processCommand("ADDROW mytable 1 hello dave@gmail.com;ADDROW mytable 3 hi me@yahoo.co.uk;ADDROW mytable 1  lol;ADDROW mytable 7 hi NA;PEEK mytable");
	//db.processCommand("MULTIADDROW mytable 1 hello dave@gmail.com 3 hi me@yahoo.co.uk 1  lol 7 hi NA;PEEK mytable");
	/*db.processCommand("ADDROW mytable 2 hi");
	db.processCommand("ADDROW mytable");
	db.processCommand("ADDROW mytable 3 yoyo");*/
	//db.processCommand("PEEK mytable");
	std::cout << Table::convertDataToString(Table::DataType::DATETIME, std::vector<uint8_t> { 0x3C, 0x4A, 0xDE, 0x65 });
	db.readDBFile("./demo.db");
	while (true) {
		std::string commandNow = "";

		std::cout << ">";
		std::getline(std::cin, commandNow);
		
		system("cls");
		
		std::cout << ">" << commandNow << "\n";

		std::string error = db.processCommand(commandNow);

		std::cout << "\33[41m" << error << "\33[0m\n";
	}

	return 0;
}