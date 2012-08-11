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

