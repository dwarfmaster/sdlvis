
/*
 * Copyright (C) 2012 Chabassier Luc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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

