
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

#ifndef DEF_TIMER
#define DEF_TIMER

#include <boost/function.hpp>
#include <SDL/SDL.h>

class Timer
{
	public:
		Timer(unsigned int time, boost::function<void ()> cb);
		void play();
		void pause();
		void toggle();
		void reset(); // Remet le temps restant au max
		void update();

	private:
		boost::function<void ()> m_callback;
		Uint32 m_time; // temps entre deux appels
		Uint32 m_lastTime;
		Uint32 m_timeStay; // temps restant
		bool m_pause;

		Timer();
		Timer(const Timer&);
		Timer& operator=(const Timer&);
};

#endif
