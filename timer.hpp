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
