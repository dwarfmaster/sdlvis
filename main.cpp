#include <iostream>
#include <exception>

#include "config.hpp"

int main(int argc, char *argv[])
{
	try
	{
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
			throw std::string("Erreur à l'initialsiation de la lib SDL.");

		ConfigLoader conf(argc, argv);
		conf.load();
	}
	catch(const std::string& str)
	{
		std::cout << "Error thrown by program : " << str << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << "Error thrown by standart library : " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Unknown error." << std::endl;
	}

	SDL_Quit();

	return 0;
}

