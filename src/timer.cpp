
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

#include "timer.hpp"

	Timer::Timer(unsigned int time, boost::function<void ()> cb)
: m_callback(cb), m_time(time), m_lastTime(SDL_GetTicks()), m_timeStay(m_time), m_pause(true)
{
}

void Timer::play()
{
	if( !m_pause )
		return;

	m_pause = false;
	m_lastTime = SDL_GetTicks();
}

void Timer::pause()
{
	update();
	m_pause = true;
}

void Timer::update()
{
	if( m_pause )
		return;

	Uint32 m_timeLapsed = SDL_GetTicks() - m_lastTime;
	while(m_timeLapsed > m_timeStay)
	{
		if( m_callback )
			m_callback();
		m_timeLapsed -= m_timeStay;
		m_timeStay = m_time;
	}

	m_timeStay -= m_timeLapsed;
	m_lastTime = SDL_GetTicks();
}

void Timer::toggle()
{
	if(m_pause)
		play();
	else
		pause();
}

void Timer::reset()
{
	m_lastTime = SDL_GetTicks();
	m_timeStay = m_time;
}
