#pragma once
#include <string>
#include <bitset>
#include <fstream>
//using namespace std; // no God, damn nooooo!!!
using std::string;
using std::endl;

class IPv6Address
{
private:
	char scope[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	//string randAddressArray [8] = {"","","","","","","",""};
	//string randAddress;
public:
	string randAddress;
	string randomAddress();
	void wrRprt(string tmpAddress);
};

