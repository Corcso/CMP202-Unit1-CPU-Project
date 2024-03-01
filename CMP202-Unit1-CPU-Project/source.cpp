#include <iostream>
#include <vector>

#include "Database.h"
#include "Table.h"

// Function prototypes
void Demo(Database*);

int main() {

	/*std::vector<unsigned char> data;

	data.push_back(0x68);
	data.push_back(0x69);
	data.push_back(0x39);
	data.push_back(0x38);

	uint32_t number = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
	std::string newString = { (char)data[0], (char)data[1], (char)data[2], (char)data[3] };
	std::cout << number << " and " << newString;*/

	std::vector<Table::DataType> dataTypes{Table::DataType::INT_32, Table::DataType::STRING_255, Table::DataType::STRING_255 };
	std::vector<Table::DataType> dataTypes2{Table::DataType::INT_32, Table::DataType::INT_32, Table::DataType::DATETIME, Table::DataType::STRING_255 };

	// Create new database
	Database db;
	// Add table
	/*Table* myTable = db.addTable("names");
	myTable->setColTypes(dataTypes);
	myTable->setColHeaders(std::vector<std::string>{"CustomerID", "First Name", "Last Name"});

	myTable = db.addTable("purchases");
	myTable->setColTypes(dataTypes2);
	myTable->setColHeaders(std::vector<std::string>{"PurchaseID", "CustomerID", "TimeOfPurchase", "PurchaseDetail"});*/
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
	//std::cout << Table::convertDataToString(Table::DataType::DATETIME, std::vector<uint8_t> { 0x3C, 0x4A, 0xDE, 0x65 });
	db.processCommand("LOAD ./demo.db");
	db.processCommand("SETTING Log-Timing true");
	db.processCommand("SORT purchases TimeOfPurchase ASC");
	db.processCommand("LOAD ./demo.db");
	db.processCommand("SETTING Thread-Count 1");
	db.processCommand("SORT purchases TimeOfPurchase ASC");
	
	while (true) {
		std::string commandNow = "";

		std::cout << ">";
		std::getline(std::cin, commandNow);
		if (commandNow == "EXIT") break;
		if (commandNow == "DEMO") Demo(&db);
		system("cls");
		
		std::cout << ">" << commandNow << "\n";

		std::string error = db.processCommand(commandNow);

		std::cout << "\33[41m" << error << "\33[0m\n";
	}
	//db.writeDBFile("./demotest.db");
	return 0;
}




// This is the demo function which will walk the user through the database program showing threading capability
// And how to use the program
void Demo(Database* db) {

	std::string enterHolder; // Holds whatever the user types when asked to press enter
	std::string BLUE = "\33[94m";
	std::string GREEN = "\33[32m";
	std::string RESET = "\33[0m";

	std::cout << BLUE << "Welcome to the demo of my CMP202 project." << RESET << "\n";
	std::cout << BLUE << "The demo will take you through how the project works and showcase" << RESET << "\n";
	std::cout << BLUE << "various commands and how the program uses parallel programming to" << RESET << "\n";
	std::cout << BLUE << "speed up various execution points." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder); // This is the waiting for enter press part
	system("cls");

	std::cout << BLUE << "The project is a database program which can work with multiple" << RESET << "\n";
	std::cout << BLUE << "tables and perform a series of operations. It makes use of " << RESET << "\n";
	std::cout << BLUE << "parallelism in its sorting, searching and conditional editing ADD MORE" << RESET << "\n";
	std::cout << BLUE << "operations. " << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "We can use the add table command to add a table to the database." << RESET << "\n";
	std::cout << ">ADDTABLE myNewTable INT_32,STRING_255,DATETIME id,name,dateOfBirth" << RESET << "\n";
	db->processCommand("ADDTABLE myNewTable INT_32,STRING_255,DATETIME id,name,dateOfBirth");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "We can then view all our tables with TABLES." << RESET << "\n";
	std::cout << ">TABLES" << RESET << "\n";
	db->processCommand("TABLES");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "The PEEK command shows the first 10 rows of a table" << RESET << "\n";
	std::cout << ">PEEK myNewTable" << RESET << "\n";
	db->processCommand("PEEK myNewTable");
	std::cout << BLUE << "Currently the table is empty, we can add more data with" << RESET << "\n";
	std::cout << BLUE << "the ADDROW command. We must give it data for every column." << RESET << "\n";
	std::cout << BLUE << "The <> around our data means spaces and semicolons will be ignored." << RESET << "\n";
	std::cout << BLUE << "This is required for things like dates or when you want spaces in" << RESET << "\n";
	std::cout << BLUE << "your strings." << RESET << "\n";
	std::cout << ">ADDROW myNewTable 1 <John Doe> <2001-03-14 00:00:00>" << RESET << "\n";
	std::cout << ">PEEK myNewTable" << RESET << "\n";
	db->processCommand("ADDROW myNewTable 1 <John Doe> <2001-03-14 00:00:00>");
	db->processCommand("PEEK myNewTable");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "You can also chain commands with ;" << RESET << "\n";
	std::cout << ">ADDROW myNewTable 2 <Jane Doe> <1998-11-23 00:00:00>;PEEK myNewTable" << RESET << "\n";
	db->processCommand("ADDROW myNewTable 2 <Jane Doe> <1998-11-23 00:00:00>;PEEK myNewTable");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "You can load and save databases using the LOAD" << RESET << "\n";
	std::cout << BLUE << "and SAVE functions. There is a demo database ready" << RESET << "\n";
	std::cout << BLUE << "for use. We will load that and see what tables it has." << RESET << "\n";
	std::cout << ">LOAD ./demo.db;TABLES" << RESET << "\n";
	db->processCommand("LOAD ./demo.db"); // FIX THIS NEED TWO COMMANDS
	db->processCommand("TABLES");

	std::cout << BLUE << "This database has 2 tables, a names and a purchases one." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");
}