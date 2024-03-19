#include <iostream>
#include <vector>

#include "Database.h"
#include "Table.h"

#include <random>

// Function prototypes
void Demo(Database*);

int main() {
	srand(time(0));
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
	//db.processCommand("LOAD ./demo.db");
	//db.processCommand("SETTING Log-Timing true");
	//db.processCommand("SORT purchases TimeOfPurchase ASC");
	//db.processCommand("LOAD ./demo.db");
	//db.processCommand("SETTING Thread-Count 1");
	//db.processCommand("SORT purchases TimeOfPurchase ASC");
	//db.processCommand("LOAD ./demo.db");
	
	
	
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

	// ADD STUFF HERE
	db->processCommand("DROPALL");

	std::cout << BLUE << "You can sort tables. Sorting tables uses a parallel" << RESET << "\n";
	std::cout << BLUE << "quicksort algorithm that will use up to as many" << RESET << "\n";
	std::cout << BLUE << "threads as defined in settings." << RESET << "\n";
	std::cout << BLUE << "This means we can compare timing by limiting the" << RESET << "\n";
	std::cout << BLUE << "thread count to 1. (Main thread only)" << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "To showcase this we need to have large tables." << RESET << "\n";
	std::cout << BLUE << "We will quickly generate 2 large tables and fill" << RESET << "\n";
	std::cout << BLUE << "them with random data." << RESET << "\n";
	std::cout << BLUE << "This shouldn't take longer than a minute continue" << RESET << "\n";
	std::cout << BLUE << "when you are ready to generate the tables." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	db->processCommand("ADDTABLE largeTable INT_32,INT_32,DATETIME id,quantity,date");
	db->processCommand("ADDTABLE largeTable2 INT_32,INT_32,DATETIME id,quantity,date");
	for (int p = 0; p < 100; p++) {
		std::cout << (p + 1) * 1 << "% Complete\n";
		for (int i = 0; i < 10000; i++) {
			for (int b = 0; b < 12; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
			for (int b = 0; b < 12; b++) db->getDirectTableReference("largeTable2")->pushDirectData(rand() % 256);
		}
		system("cls");
	}
	db->getDirectTableReference("largeTable")->directSetRows(1000000);
	db->getDirectTableReference("largeTable2")->directSetRows(1000000);

	std::cout << BLUE << "As you can see below we have two new tables." << RESET << "\n";
	std::cout << BLUE << "We have peeked into 1 to see what the data" << RESET << "\n";
	std::cout << BLUE << "looks like." << RESET << "\n";

	std::cout << ">TABLES" << RESET << "\n";
	db->processCommand("TABLES");

	std::cout << ">PEEK largeTable" << RESET << "\n";
	db->processCommand("PEEK largeTable");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "We can change the settings that the database" << RESET << "\n";
	std::cout << BLUE << "uses via the settings command. We are going" << RESET << "\n";
	std::cout << BLUE << "to turn on log timing. This means we can see" << RESET << "\n";
	std::cout << BLUE << "how long each command took to run." << RESET << "\n";

	std::cout << ">SETTING Log-Timing true" << RESET << "\n";
	db->processCommand("SETTING Log-Timing true");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "We are now sorting the two tables this may" << RESET << "\n";
	std::cout << BLUE << "take a minute. " << RESET << "\n";
	std::cout << BLUE << "The first sort is with your computer's total" << RESET << "\n";
	std::cout << BLUE << "thread capacity being used." << RESET << "\n";
	std::cout << BLUE << "The second sort is sequential only using the" << RESET << "\n";
	std::cout << BLUE << "main thread. The settings are changed between sorts." << RESET << "\n";
	db->processCommand("SETTING Thread-Count 64");
	std::cout << ">SORT largeTable date ASC" << RESET << "\n";
	db->processCommand("SORT largeTable date ASC");
	std::cout << "\n>SETTING Thread-Count 1" << RESET << "\n";
	db->processCommand("SETTING Thread-Count 1");
	std::cout << "\n>SORT largeTable2 date ASC" << RESET << "\n";
	db->processCommand("SORT largeTable2 date ASC");

	std::cout << BLUE << "You can hopefully see the second sequential" << RESET << "\n";
	std::cout << BLUE << "sort ran slower than the first parallel one." << RESET << "\n";

	db->processCommand("SETTING Log-Timing true");
	db->processCommand("SETTING Thread-Count 2");
	db->processCommand("FIND largeTable id 125048");
	db->processCommand("SETTING Thread-Count 16");
	db->processCommand("FIND largeTable id 125048");

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");
}