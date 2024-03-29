#include <iostream>
#include <vector>

#include "Database.h"
#include "Table.h"

#include <random>

// Function prototypes
void Demo(Database*);
void getTimings(Database* db);

int main() {
	srand(time(0));

	// Create new database
	Database db;
	//getTimings(&db);
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

	return 0;
}




// This is the demo function which will walk the user through the database program showing threading capability
// And how to use the program
void Demo(Database* db) {

	// Reset the settings to default for the demo and get the thread count of the current processor
	int threadCount = std::thread::hardware_concurrency();
	db->processCommand("SETTING Log-Timing false");
	db->processCommand("SETTING Thread-Count "+std::to_string(threadCount));
	db->processCommand("SETTING Search-Block-Multiplier 8");
	system("cls");

	std::string enterHolder; // Holds whatever the user types when asked to press enter
	std::string BLUE = "\33[94m";
	std::string GREEN = "\33[32m";
	std::string RESET = "\33[0m";

	std::cout << BLUE << "Welcome to the demo of my CMP202 CPU project." << RESET << "\n";
	std::cout << BLUE << "The demo will take you through how the project works and showcase" << RESET << "\n";
	std::cout << BLUE << "various commands and how the program uses parallel programming to" << RESET << "\n";
	std::cout << BLUE << "speed up various execution points." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder); // This is the waiting for enter press part
	system("cls");

	std::cout << BLUE << "The project is a database program which can work with multiple" << RESET << "\n";
	std::cout << BLUE << "tables and perform a series of operations. It makes use of " << RESET << "\n";
	std::cout << BLUE << "parallelism in its sorting, searching and and inner join functions." << RESET << "\n";

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
	db->processCommand("LOAD ./demo.db;TABLES");

	std::cout << BLUE << "This database has 2 tables, a names and a purchases one." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "These tables are a good set to demo the inner join" << RESET << "\n";
	std::cout << BLUE << "command." << RESET << "\n";
	std::cout << BLUE << "This is one of the parallel commands. A speed " << RESET << "\n";
	std::cout << BLUE << "comparison of this command is later featured in" << RESET << "\n";
	std::cout << BLUE << "the demo." << RESET << "\n";

	std::cout << ">INJOIN names purchases CustomerID CustomerID namesAndPurchases;PEEK namesAndPurchases" << RESET << "\n";
	db->processCommand("INJOIN names purchases CustomerID CustomerID namesAndPurchases;PEEK namesAndPurchases");

	std::cout << BLUE << "The INJOIN command inner joins 2 tables based on " << RESET << "\n";
	std::cout << BLUE << "a common key and creates a new table from it." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "Next we will move onto the 3 parallel running commands." << RESET << "\n";
	std::cout << BLUE << "There are more non parallel commands available which" << RESET << "\n";
	std::cout << BLUE << "can be accessed with the HELP command." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	// Clear the database for new table generation
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
	std::cout << BLUE << "This shouldn't take longer than a minute." << RESET << "\n";
	std::cout << BLUE << "Continue when you are ready to generate the tables." << RESET << "\n";

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
	std::cout << "\n\n>SETTING Thread-Count " + std::to_string(threadCount) << RESET << "\n";
	db->processCommand("SETTING Thread-Count " + std::to_string(threadCount));
	std::cout << ">SORT largeTable date ASC" << RESET << "\n";
	db->processCommand("SORT largeTable date ASC");
	std::cout << "\n>SETTING Thread-Count 1" << RESET << "\n";
	db->processCommand("SETTING Thread-Count 1");
	std::cout << "\n>SORT largeTable2 date ASC" << RESET << "\n";
	db->processCommand("SORT largeTable2 date ASC");

	std::cout << BLUE << "You can hopefully see the second sequential" << RESET << "\n";
	std::cout << BLUE << "sort ran slower than the first parallel one." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "For the next section of the demo we will need" << RESET << "\n";
	std::cout << BLUE << "to generate a 100million row table. " << RESET << "\n";
	std::cout << BLUE << "This is to show the best speed up for searching." << RESET << "\n";
	std::cout << BLUE << "This will take up about 500MB of ram and shouldn't" << RESET << "\n";
	std::cout << BLUE << "take more than a minute." << RESET << "\n";
	std::cout << BLUE << "Continue when you are ready. " << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	db->processCommand("DROPALL");
	db->processCommand("ADDTABLE largeTable INT_32 data");
	for (int p = 0; p < 100; p++) {
		std::cout << (p + 1) * 1 << "% Complete\n";
		for (int i = 0; i < 1000000; i++) {
			for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
		}
		system("cls");
	}
	db->getDirectTableReference("largeTable")->directSetRows(100000000);

	std::cout << BLUE << "You can also use the FIND command to find all" << RESET << "\n";
	std::cout << BLUE << "instances of a certain value in a table. " << RESET << "\n";
	std::cout << BLUE << "Below FIND is set up already and running. " << RESET << "\n";
	std::cout << BLUE << "There will be a very low chance it finds" << RESET << "\n";
	std::cout << BLUE << "any rows but this is a good case for comparison." << RESET << "\n";
	std::cout << BLUE << "FIND uses a farm, this is optimal as when rows are" << RESET << "\n";
	std::cout << BLUE << "found extra computation is required making each task" << RESET << "\n";
	std::cout << BLUE << "uneven." << RESET << "\n";

	std::cout << "\n>SETTING Thread-Count 1" << RESET << "\n";
	db->processCommand("SETTING Thread-Count 1");
	std::cout << "\n>FIND largeTable data 0" << RESET << "\n";
	db->processCommand("FIND largeTable data 0"); 
	std::cout << "\n\n>SETTING Thread-Count " + std::to_string(threadCount) << RESET << "\n";
	db->processCommand("SETTING Thread-Count " + std::to_string(threadCount));
	std::cout << "\n>FIND largeTable data 0" << RESET << "\n";
	db->processCommand("FIND largeTable data 0");

	std::cout << BLUE << "\nYou can hopefully see the first sequential" << RESET << "\n";
	std::cout << BLUE << "search ran slower than the second parallel one." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "The third and final parallel algorithm is an inner join." << RESET << "\n";
	std::cout << BLUE << "As before 2 large tables will quickly be generated to show" << RESET << "\n";
	std::cout << BLUE << "a speedup. " << RESET << "\n";
	std::cout << BLUE << "Continue when you are ready. " << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	db->processCommand("DROPALL");
	db->processCommand("ADDTABLE largeTable INT_32,INT_32,DATETIME id,quantity,date");
	db->processCommand("ADDTABLE largeTable2 INT_32,INT_32,DATETIME id,quantity,date");
	for (int p = 0; p < 10; p++) {
		std::cout << (p + 1) * 10 << "% Complete\n";
		for (int i = 0; i < 1000; i++) {
			for (int b = 0; b < 12; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
			for (int b = 0; b < 12; b++) db->getDirectTableReference("largeTable2")->pushDirectData(rand() % 256);
		}
		system("cls");
	}
	db->getDirectTableReference("largeTable")->directSetRows(10000);
	db->getDirectTableReference("largeTable2")->directSetRows(10000);

	std::cout << BLUE << "Inner join uses a channel to send data" << RESET << "\n";
	std::cout << BLUE << "from the joint row finder to the new" << RESET << "\n";
	std::cout << BLUE << "table creator. " << RESET << "\n";
	std::cout << BLUE << "Since most computation is done on the" << RESET << "\n";
	std::cout << BLUE << "row finding, threads are split 3:1." << RESET << "\n";
	std::cout << BLUE << "You must have at least 4 threads to run" << RESET << "\n";
	std::cout << BLUE << "inner join. " << RESET << "\n";

	db->processCommand("SETTING Thread-Count " + std::to_string(threadCount));
	std::cout << ">INJOIN largeTable largeTable2 id id newT1" << RESET << "\n";
	db->processCommand("INJOIN largeTable largeTable2 id id newT1");
	db->processCommand("SETTING Thread-Count 1");
	std::cout << ">INJOIN largeTable largeTable2 id id newT2" << RESET << "\n";
	db->processCommand("INJOIN largeTable largeTable2 id id newT2");

	std::cout << BLUE << "\nYou can hopefully see the first parallel" << RESET << "\n";
	std::cout << BLUE << "inner join ran faster than the second sequential one." << RESET << "\n";

	std::cout << GREEN << "Press enter to continue..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	std::cout << BLUE << "\nThat's the end of the demo. All tables will be" << RESET << "\n";
	std::cout << BLUE << "wiped so you are on a blank database." << RESET << "\n";
	std::cout << BLUE << "Call LOAD ./demo.db to load the demo database." << RESET << "\n";
	std::cout << BLUE << "Call HELP to see all commands available." << RESET << "\n";

	std::cout << GREEN << "Press enter end the demo..." << RESET << "\n";
	std::getline(std::cin, enterHolder);
	system("cls");

	// Reset the settings to default for leaving the demo
	db->processCommand("SETTING Log-Timing false");
	db->processCommand("SETTING Thread-Count " + std::to_string(threadCount));
	db->processCommand("SETTING Search-Block-Multiplier 8");
	system("cls");
}

// This is a function I used to get the timings for the report
void getTimings(Database* db) {
	db->processCommand("SETTING Log-Timing true");
	//for (int i = 0; i < 5; i++) {
	//	std::cout << "SORT|16|1000000\n";
	//	db->processCommand("DROPALL");
	//	db->processCommand("ADDTABLE largeTable INT_32 data");
	//	for (int p = 0; p < 1000000; p++) {
	//		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//	}
	//	db->getDirectTableReference("largeTable")->directSetRows(1000000);
	//	db->processCommand("SORT largeTable data ASC");
	//}
	//db->processCommand("SETTING Thread-Count 1");
	//for (int i = 0; i < 5; i++) {
	//	std::cout << "SORT|1|1000000\n";
	//	db->processCommand("DROPALL");
	//	db->processCommand("ADDTABLE largeTable INT_32 data");
	//	for (int p = 0; p < 1000000; p++) {
	//		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//	}
	//	db->getDirectTableReference("largeTable")->directSetRows(1000000);
	//	db->processCommand("SORT largeTable data ASC");
	//}
	//std::cout << "\n\n=================================================\n\n";
	//std::cout << "SORT|2|1000000\n";
	//db->processCommand("SETTING Thread-Count 2");
	//db->processCommand("DROPALL");
	//db->processCommand("ADDTABLE largeTable INT_32 data");
	//for (int p = 0; p < 1000000; p++) {
	//	for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//}
	//db->getDirectTableReference("largeTable")->directSetRows(1000000);
	//db->processCommand("SORT largeTable data ASC");
	//std::cout << "SORT|4|1000000\n";
	//db->processCommand("SETTING Thread-Count 4");
	//db->processCommand("DROPALL");
	//db->processCommand("ADDTABLE largeTable INT_32 data");
	//for (int p = 0; p < 1000000; p++) {
	//	for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//}
	//db->getDirectTableReference("largeTable")->directSetRows(1000000);
	//db->processCommand("SORT largeTable data ASC");
	//std::cout << "SORT|8|1000000\n";
	//db->processCommand("SETTING Thread-Count 8");
	//db->processCommand("DROPALL");
	//db->processCommand("ADDTABLE largeTable INT_32 data");
	//for (int p = 0; p < 1000000; p++) {
	//	for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//}
	//db->getDirectTableReference("largeTable")->directSetRows(1000000);
	//db->processCommand("SORT largeTable data ASC");
	//std::cout << "SORT|32|1000000\n";
	//db->processCommand("SETTING Thread-Count 32");
	//db->processCommand("DROPALL");
	//db->processCommand("ADDTABLE largeTable INT_32 data");
	//for (int p = 0; p < 1000000; p++) {
	//	for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//}
	//db->getDirectTableReference("largeTable")->directSetRows(1000000);
	//db->processCommand("SORT largeTable data ASC");
	//std::cout << "\n\n=================================================\n\n";
	//db->processCommand("SETTING Thread-Count 16");
	//int rowCountThisTime = 4;
	//for (int i = 0; i < 10; i++) {
	//	std::cout << "SORT|1|" << rowCountThisTime << "\n";
	//	db->processCommand("DROPALL");
	//	db->processCommand("ADDTABLE largeTable INT_32 data");
	//	for (int p = 0; p < rowCountThisTime; p++) {
	//		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	//	}
	//	db->getDirectTableReference("largeTable")->directSetRows(rowCountThisTime);
	//	db->processCommand("SORT largeTable data ASC");
	//	rowCountThisTime *= 4;
	//}
	/*db->processCommand("DROPALL");
	db->processCommand("ADDTABLE largeTable INT_32 data");
	for (int p = 0; p < 100000000; p++) {
		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	}
	db->getDirectTableReference("largeTable")->directSetRows(100000000);
	for (int i = 0; i < 5; i++) {
		std::cout << "FIND|16|100000000\n";
		
		db->processCommand("FIND largeTable data 0");
	}
	db->processCommand("SETTING Thread-Count 1");
	for (int i = 0; i < 5; i++) {
		std::cout << "FIND|1|100000000\n";
		db->processCommand("FIND largeTable data 0");
	}
	std::cout << "\n\n=================================================\n\n";
	std::cout << "FIND|2|100000000\n";
	db->processCommand("SETTING Thread-Count 2");
	
	db->processCommand("FIND largeTable data 0");
	std::cout << "FIND|4|100000000\n";
	db->processCommand("SETTING Thread-Count 4");
	db->processCommand("DROPALL");
	db->processCommand("ADDTABLE largeTable INT_32 data");
	for (int p = 0; p < 100000000; p++) {
		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	}
	db->getDirectTableReference("largeTable")->directSetRows(100000000);
	db->processCommand("FIND largeTable data 0");
	std::cout << "FIND|8|100000000\n";
	db->processCommand("SETTING Thread-Count 8");
	db->processCommand("FIND largeTable data 0");
	std::cout << "FIND|32|100000000\n";
	db->processCommand("SETTING Thread-Count 32");
	db->processCommand("FIND largeTable data 0");
	std::cout << "\n\n=================================================\n\n";
	db->processCommand("SETTING Thread-Count 16");
	int rowCountThisTime = 4;
	for (int i = 0; i < 12; i++) {
		std::cout << "FIND|16|" << rowCountThisTime << "\n";
		db->processCommand("DROPALL");
		db->processCommand("ADDTABLE largeTable INT_32 data");
		for (int p = 0; p < rowCountThisTime; p++) {
			for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
		}
		db->getDirectTableReference("largeTable")->directSetRows(rowCountThisTime);
		db->processCommand("FIND largeTable data 0");
		rowCountThisTime *= 4;
	}*/

	db->processCommand("DROPALL");
	db->processCommand("ADDTABLE largeTable INT_32 data");
	for (int p = 0; p < 10000; p++) {
		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
	}
	db->getDirectTableReference("largeTable")->directSetRows(10000);
	db->processCommand("ADDTABLE largeTable2 INT_32 data");
	for (int p = 0; p < 10000; p++) {
		for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable2")->pushDirectData(rand() % 256);
	}
	db->getDirectTableReference("largeTable2")->directSetRows(10000);
	for (int i = 0; i < 5; i++) {
		std::cout << "INJOIN|16|10000\n";

		db->processCommand("INJOIN largeTable largeTable2 data data newTable" + std::to_string(i));
	}
	db->processCommand("SETTING Thread-Count 1");
	for (int i = 0; i < 5; i++) {
		std::cout << "INJOIN|1|10000\n";
		db->processCommand("INJOIN largeTable largeTable2 data data newTable" + std::to_string(i+5));
	}
	std::cout << "\n\n=================================================\n\n";
	std::cout << "INJOIN|2|10000\n";
	db->processCommand("SETTING Thread-Count 2");

	db->processCommand("INJOIN largeTable largeTable2 data data newTable10");
	std::cout << "INJOIN|4|10000\n";
	db->processCommand("SETTING Thread-Count 4");
	db->processCommand("INJOIN largeTable largeTable2 data data newTable11");
	std::cout << "INJOIN|8|10000\n";
	db->processCommand("SETTING Thread-Count 8");
	db->processCommand("INJOIN largeTable largeTable2 data data newTable12");
	std::cout << "INJOIN|32|10000\n";
	db->processCommand("SETTING Thread-Count 32");
	db->processCommand("INJOIN largeTable largeTable2 data data newTable13");
	std::cout << "\n\n=================================================\n\n";
	db->processCommand("SETTING Thread-Count 16");
	int rowCountThisTime = 4;
	for (int i = 0; i < 7; i++) {
		std::cout << "INJOIN|16|" << rowCountThisTime << "\n";
		db->processCommand("DROPALL");
		db->processCommand("ADDTABLE largeTable INT_32 data");
		for (int p = 0; p < rowCountThisTime; p++) {
			for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable")->pushDirectData(rand() % 256);
		}
		db->getDirectTableReference("largeTable")->directSetRows(rowCountThisTime);
		db->processCommand("ADDTABLE largeTable2 INT_32 data");
		for (int p = 0; p < rowCountThisTime; p++) {
			for (int b = 0; b < 4; b++) db->getDirectTableReference("largeTable2")->pushDirectData(rand() % 256);
		}
		db->getDirectTableReference("largeTable2")->directSetRows(rowCountThisTime);
		db->processCommand("INJOIN largeTable largeTable2 data data newTable" + std::to_string(i + 14));
		rowCountThisTime *= 4;
	}
}