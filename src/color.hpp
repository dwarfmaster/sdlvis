
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

#ifndef DEF_COLOR
#define DEf_COLOR

#include <SDL/SDL.h>
#include <string>
#include <sstream>

class Color
{
	public:
		Color();
		Color(Uint8 mr, Uint8 mg, Uint8 mb);
		Color(const Color& cp);
		Color(std::string src); // Hexadécimal, exemple : "0xFF0000" ou "FF0000"
		Color(const SDL_Color& col);
		Color(Uint32 col);

		Color& operator=(const Color& cp);

		Uint32 get32() const;
		SDL_Color getSdl() const;
		std::string getHex() const;

	private:
		Uint8 r,g,b;
};

