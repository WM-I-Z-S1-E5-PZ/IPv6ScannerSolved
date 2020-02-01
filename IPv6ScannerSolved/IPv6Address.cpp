#include "IPv6Address.h"

string IPv6Address::randomAddress()
{

	randAddress = "";

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			randAddress = randAddress + scope[rand() % 16];
		}
		if (i < 7) {
			randAddress = randAddress + ":";
		}

	}

	wrRprt(randAddress);
	return randAddress;
}

void IPv6Address::wrRprt(string tmpAddress)
{
	std::fstream myRprt;
	myRprt.open("../Report/addresses.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	if (myRprt.is_open())
	{
		myRprt << tmpAddress << endl;
	}
	myRprt.close();
}
