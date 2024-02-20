#include <iostream>
#include <vector>
int main() {

	std::vector<unsigned char> data;

	data.push_back(0x68);
	data.push_back(0x69);
	data.push_back(0x39);
	data.push_back(0x38);

	uint32_t number = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
	std::string newString = { (char)data[0], (char)data[1], (char)data[2], (char)data[3] };
	std::cout << number << " and " << newString;

	return 0;
}