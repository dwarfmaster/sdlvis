
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

#include "color.hpp"

Color::Color()
	: r(0), g(0), b(0)
{}

Color::Color(Uint8 mr, Uint8 mg, Uint8 mb)
	: r(mr), g(mg), b(mb)
{}

Color::Color(const Color& cp)
{
	r = cp.r;
	g = cp.g;
	b = cp.b;
}

Color::Color(std::string src)
	: r(0), g(0), b(0)
{
	if( src.size() != 6 && src.size() != 8 ) return;
	if( src.size() == 8 && src[0] != '0' && src[1] != 'x' ) return;
	if( src.size() == 8 ) src.erase(0,2);

	std::istringstream pr( src.substr(0,2) );
	pr >> std::hex >> (int&)r;
	std::istringstream pg( src.substr(2,2) );
	pg >> std::hex >> (int&)g;
	std::istringstream pb( src.substr(4,2) );
	pb >> std::hex >> (int&)b;
}

Color::Color(const SDL_Color& col)
{
	r = col.r;
	b = col.b;
	g = col.g;
}

Color::Color(Uint32 col)
{
	SDL_Surface* ecran = SDL_GetVideoSurface();
	SDL_GetRGB(col, ecran->format, &r, &g, &b);
}

Color& Color::operator=(const Color& cp)
{
	r = cp.r;
	b = cp.b;
	g = cp.g;
	return *this;
}

Uint32 Color::get32() const
{
	SDL_Surface* ecran = SDL_GetVideoSurface();
	return SDL_MapRGB(ecran->format, r, g, b);
}

SDL_Color Color::getSdl() const
{
	SDL_Color ret;
	ret.r = r;
	ret.b = b;
	ret.g = g;
	return ret;
}

std::string Color::getHex() const
{
	std::ostringstream oss;
	oss << std::hex << r << g << b;
	return oss.str();
}


