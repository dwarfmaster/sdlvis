#include <iostream>
#include <exception>

#include "config.hpp"
#include "print.hpp"

int main(int argc, char *argv[])
{
	try
	{
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
			throw std::string("Erreur à l'initialsiation de la lib SDL.");

		if(TTF_Init() < 0)
			throw std::string("Erreur à l'initialisation de la lib SDL_ttf.");

		ConfigLoader conf(argc, argv);
		if( !conf.load() )
			return 0;

		Printer print( conf.get() );
		print.run();
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

	TTF_Quit();
	SDL_Quit();

	return 0;
}

