#include <iostream>

#include "config.hpp"

int main(int argc, char *argv[])
{
	ConfigLoader conf(argc, argv);
	conf.load();

	return 0;
}

