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


	return randAddress;
}
