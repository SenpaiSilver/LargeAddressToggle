#include <iostream>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "largeaddresstoggle.h"

#include "File.hh"

void process(char* path, char* cmd) {

}

int main(int ac, char** av) {
	if (ac >= 3) {
		File fle(av[2]);

		if (!fle.isOpen()) {
			return (EXIT_FAILURE);
		}
		if (strcmp("show", av[1]) == 0)
			fle.ShowInfo();

		if ((strcmp("enable", av[1]) == 0) && !fle.isLargeAddressAware()) {
			fle.EnableLargeAddress();
		}
		else if ((strcmp("enable", av[1]) == 0)) {
			std::cerr << "Already large address aware." << std::endl;
			return (EXIT_FAILURE);
		}
		if ((strcmp("disable", av[1]) == 0) && fle.isLargeAddressAware())
			fle.DisableLargeAddress();
		else if ((strcmp("disable", av[1]) == 0)) {
			std::cerr << "Already NOT large address aware." << std::endl;
			return (EXIT_FAILURE);
		}
	}
	else
		std::cerr << av[0] << "<show|enable|disable> <path>" << std::endl;
#ifdef _DEBUG
	std::getchar();
#endif
	return (EXIT_SUCCESS);
}