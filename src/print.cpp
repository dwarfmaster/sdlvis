
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

#include "print.hpp"
#include "tools.hpp"


	Printer::Printer(config conf)
: m_config(conf)
{
	if( verbose() )
		std::cout << "\n" << std::endl; // Espace avant les messages de la suite

	if( !mute() )
		std::cout << "Loading elements." << std::endl;

	loadWindow();
	loadErr();
	if( m_config.text )
		loadFont(); // Charge la police
	load(); // Charge les images

	if( m_config.diap )
		loadDiap();
	if( m_config.timeout )
		loadTimeout();

	loadFirst();

	if( !m_config.mouse )
		SDL_ShowCursor( SDL_DISABLE );

	resetmv();
	initZoom(); // doit être appelé même quand il n'y a pas de zoom dynamique
}

void Printer::run()
{
	if( !mute() )
		std::cout << "Launching program." << std::endl;

	sdl::Event event;
	loadKeys(&event);

	SDL_Rect pos;

	if( m_config.diap )
		m_diapTimer->play();
	if( m_config.timeout )
		m_timeout->play();

	m_quit = false;

	m_lastTime = SDL_GetTicks();
	while( !m_quit )
	{
		event.update();
		if( m_config.diap )
			m_diapTimer->update();
		move(&event);

		SDL_FillRect(ecran, NULL, m_config.bgcolor.get32());

		SDL_Surface* toblit = (m_picts[m_act].zoom == NULL) ? m_picts[m_act].surf : m_picts[m_act].zoom;
		pos.x = ecran->w / 2 - toblit->w / 2;
		pos.y = ecran->h / 2 - toblit->h / 2;
		pos.x += static_cast<Sint16>(m_xdec);
		pos.y += static_cast<Sint16>(m_ydec);
		SDL_BlitSurface(toblit, NULL, ecran, &pos);

		if( m_config.text )
		{
			pos.x = ecran->w /2 - m_picts[m_act].txt->w / 2;
			pos.y = ecran->h - m_picts[m_act].txt->h;
			SDL_BlitSurface(m_picts[m_act].txt, NULL, ecran, &pos);
		}

		SDL_Flip(ecran);

		m_quit = event.quit() && !m_config.noquit;
		if( m_config.timeout )
			m_timeout->update();

		m_lastTime = SDL_GetTicks();
		SDL_Delay( 1000/30 );
	}
}

Printer::~Printer()
{
	if( !mute() )
		std::cout << "Stopping program." << std::endl;

	SDL_FreeSurface(m_err);

	if( m_config.text )
	{
		TTF_CloseFont( m_font );
		if( !m_config.ptext )
			SDL_FreeSurface( m_picts[m_act].txt );
		else
		{
			for(size_t i=0; i < m_picts.size(); ++i)
				SDL_FreeSurface( m_picts[i].txt );
		}
	}

	if( !m_config.begin )
	{
		for(size_t i=0; i < m_picts.size(); ++i)
		{
			if( m_picts[i].surf != m_err )
				SDL_FreeSurface( m_picts[i].surf );
		}
	}
	else if( m_picts[m_act].surf != m_err )
		SDL_FreeSurface( m_picts[m_act].surf );

	if( m_config.timeout )
		delete m_timeout;

	for(m_act = 0; m_act < m_picts.size(); ++m_act)
		zoomReset();
}

void Printer::load()
{
	if( verbose() )
		std::cout << "Preloading pictures." << std::endl;

	m_picts.resize( m_config.paths.size() );

	for(size_t i=0; i < m_picts.size(); ++i)
	{
		m_picts[i].path = m_config.paths[i];

		if( highverb() )
			std::cout << "\t[" << i+1 << "/" << m_picts.size() << "]> Preloading of " << m_picts[i].path.string() << std::endl;

		if( m_config.prep ) // Si on doit préparer les images au démarrage
		{
			prepare( &m_picts[i] );
			if( m_config.begin ) // Si on doit charger les images au démarrage
				load( &m_picts[i] );
		}

		if( m_config.ptext ) // Si on doit charger le texte au démarrage
			loadTxt( &m_picts[i], m_config.textaa, i + 1 );
	}
}

void Printer::loadTxt(Picture* pict, bool aa, size_t number)
{
	SDL_Color fg;
	fg.r = 255 - m_config.bgcolor.getSdl().r;
	fg.g = 255 - m_config.bgcolor.getSdl().g;
	fg.b = 255 - m_config.bgcolor.getSdl().b;

	std::string name("");

	if( m_config.number )
	{
		name += "[";
		name += toString(number);
		name += "/";
		name += toString( m_picts.size() );
		name += "]> ";
	}

	name += pict->path.filename().string();

	if( highverb() )
		std::cout << "\t-> Loading text surface \"" << name << "\"." << std::endl;
	SDL_Surface* tmp = NULL;

	tmp = TTF_RenderText_Shaded(m_font, name.c_str(), fg, m_config.bgcolor.getSdl());

	if( tmp == NULL ) // Si on n'arrive pas à charger le texte, on le désactive
	{
		if( !mute() )
			std::cout << "Impossible de charger le texte " << name <<".\n\t-> Désactivation du texte." << std::endl;

		m_config.text = false;
		m_config.ptext = false;
	}

	pict->txt = tmp;
}

void Printer::loadFont()
{
	if( verbose() )
		std::cout << "Loading font." << std::endl;

	std::string path = m_config.dir.string() + "/font.ttf";
	m_font = TTF_OpenFont(path.c_str(), 15);

	if( m_font == NULL )
	{
		if( !mute() )
			std::cout << "Erreur au chargement de la police.\n\t-> Désactivation du texte." << std::endl;

		m_config.text = false;
		m_config.ptext = false;
	}
}

bool Printer::prepare(Picture* pict, bool force_guard)
{
	if( pict->prepared | pict->err )
		return true;

	pict->surf = IMG_Load( pict->path.string().c_str() );
	if( pict->surf == NULL )
	{
		if( !mute() )
			std::cout << "Erreur au chargement de l'image " << pict->path.string() << std::endl;
		pict->err = true;
		return false;
	}

	if( pict->surf->w > ecran->w
			|| pict->surf->h > ecran->h ) // S'il est plus grand
	{
		pict->bigger = true;
		if( !m_config.real )
		{
			pict->redx = double(ecran->w) / double(pict->surf->w);
			pict->redy = double(ecran->h) / double(pict->surf->h);
		}
	}
	else if( m_config.redim ) // S'il est plus petit
	{
		pict->bigger = false;
		pict->redx = double(ecran->w) / double(pict->surf->w);
		pict->redy = double(ecran->h) / double(pict->surf->h);
	}

	if( !m_config.deform )
	{
		pict->redx = (pict->redx > pict->redy) ? pict->redy : pict->redx;
		pict->redy = pict->redx;
	}

	if( !m_config.begin && !force_guard )
	{
		SDL_FreeSurface( pict->surf );
		pict->surf = NULL;
	}

	if( highverb() )
		std::cout << "Prepared picture " << pict->path.filename().string() << " with redim factor (" << pict->redx << ";" << pict->redy << ")." << std::endl;

	pict->prepared = true;
	return true;
}

void Printer::load(Picture* pict, bool aa)
{
	if( pict->err )
	{
		pict->surf = m_err;
		return;
	}

	if( highverb() )
		std::cout << "\tLoading picture " << pict->path << std::endl;

	SDL_Surface* tmp = NULL;
	if( pict->surf == NULL)
		tmp = IMG_Load( pict->path.string().c_str() );
	else
		tmp = pict->surf;

	if(tmp == NULL)
	{
		if( !mute() )
			std::cout << "Error occured when loading the picture " << pict->path.string() << std::endl;
		pict->surf = m_err;
		pict->err = true;
	}
	else
	{
		pict->surf = zoomSurface(tmp, pict->redx, pict->redy, aa);
		pict->aa = aa;

		if(pict->surf == NULL)
		{
			if( !mute() )
				std::cout << "Error occuring when redimentionned the picture " << pict->path.string() << std::endl;
			pict->surf = m_err;
		}
		else
			SDL_FreeSurface(tmp);
	}
}

void Printer::toggleAA()
{
	SDL_FreeSurface( m_picts[m_act].surf );
	m_picts[m_act].surf = NULL;
	load( &m_picts[m_act], !m_picts[m_act].aa );
}

inline bool Printer::mute()
{
	return m_config.verb == config::MUTE;
}

inline bool Printer::verbose()
{
	return m_config.verb >= config::VERBOSE;
}

inline bool Printer::highverb()
{
	return m_config.verb >= config::HIGH;
}

void Printer::next(bool ret)
{
	size_t last = m_act;
	++m_act;
	if( m_act >= m_picts.size() )
	{
		if( ret )
			m_act = 0;
		else
		{
			--m_act;
			return;
		}
	}

	if( highverb() )
		std::cout << "Next to picture " << m_picts[m_act].path << std::endl;

	update(last);
}

void Printer::prev(bool ret)
{
	size_t last = m_act;
	if( m_act == 0 )
	{
		if( ret )
			m_act = m_picts.size() - 1;
		else
			return;
	}
	else
		--m_act;

	if( highverb() )
		std::cout << "Prev to picture " << m_picts[m_act].path << std::endl;

	update(last);
}

void Printer::first()
{
	size_t last = m_act;
	m_act = 0;

	update(last);
}

void Printer::last()
{
	size_t last = m_act;
	m_act = m_picts.size() - 1;

	update(last);
}

void Printer::update(size_t last)
{
	if( !m_config.prep )
		prepare( &m_picts[m_act], true );

	if( !m_config.begin )
	{
		if( m_picts[last].surf != m_err )
		{
			SDL_FreeSurface( m_picts[last].surf );
			m_picts[last].surf = NULL;
		}
		load( &m_picts[m_act] );
	}

	if( m_config.text && !m_config.ptext )
	{
		SDL_FreeSurface( m_picts[last].txt );
		m_picts[last].txt = NULL;
		loadTxt( &m_picts[m_act], m_config.textaa, m_act + 1 );
	}

	std::string caption("sdlvis - ");
	caption += m_picts[m_act].path.filename().string();
	SDL_WM_SetCaption(caption.c_str(), NULL);

	resetmv();
	zoomReset();
}

void Printer::loadWindow()
{
	if( !mute() )
		std::cout << "Loading window." << std::endl;

	Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	if( m_config.fullscreen )
		flags |= SDL_FULLSCREEN;
	ecran = SDL_SetVideoMode(m_config.size.w, m_config.size.h, 32, flags);

	if(ecran == NULL)
		throw std::string("Error occured when loading window.");
}

void Printer::loadErr()
{
	if( verbose() )
		std::cout << "Loading error picture." << std::endl;

	std::string path = m_config.dir.string() + "/err.png";
	m_err = IMG_Load(path.c_str());
	if(m_err == NULL)
		throw std::string("Error when loading error picture.");
}

void Printer::loadDiap()
{
	m_diapTimer = new Timer(m_config.time,
			boost::bind(&Printer::next, this, m_config.loop) );
}

void Printer::loadFirst()
{
	m_act = 0;

	if( !m_config.prep )
		prepare( &m_picts[m_act], true );

	if( !m_config.begin )
		load( &m_picts[m_act] );

	if( m_config.text && !m_config.ptext )
		loadTxt( &m_picts[m_act], m_config.textaa, m_act + 1 );

	if( m_config.diap )
		m_diapTimer->reset();

	std::string caption("sdlvis - ");
	caption += m_picts[m_act].path.filename().string();
	SDL_WM_SetCaption(caption.c_str(), NULL);
}

void Printer::loadKeys(sdl::Event* event)
{
	if( verbose() )
		std::cout << "Loading keys." << std::endl;
	event->addQuitKey(SDLK_q);
	event->addQuitKey(SDLK_ESCAPE);

	if( m_config.inter )
	{
		m_keys.resize(NUMBER);

		m_keys[NEXT].addKey(SDLK_RIGHT);
		event->addPEvent( "next", &m_keys[NEXT],
				boost::bind(&Printer::next, this, m_config.loop) );

		m_keys[PREV].addKey(SDLK_LEFT);
		event->addPEvent( "prev", &m_keys[PREV],
				boost::bind(&Printer::prev, this, m_config.loop) );

		m_keys[FIRST].addKey(SDLK_UP);
		event->addPEvent( "first", &m_keys[FIRST],
				boost::bind(&Printer::first, this) );

		m_keys[LAST].addKey(SDLK_DOWN);
		event->addPEvent( "last", &m_keys[LAST],
				boost::bind(&Printer::last, this) );

		if( m_config.diap)
		{
			m_keys[TOGGLE].addKey(SDLK_SPACE);
			event->addPEvent( "pause", &m_keys[TOGGLE],
					boost::bind(&Timer::toggle, m_diapTimer) );
		}

		m_keys[TAA].addKey(SDLK_a);
		event->addPEvent( "toggle_aa", &m_keys[TAA],
				boost::bind(&Printer::toggleAA, this) );

		if( m_config.dzoom )
		{
			m_keys[ZOOMIN].addKey(SDLK_m);
			event->addPEvent( "zoom_in", &m_keys[ZOOMIN],
					boost::bind(&Printer::zoomIn, this),
					boost::none, 0, 100);

			m_keys[ZOOMOUT].addKey(SDLK_l);
			event->addPEvent( "zoom_out", &m_keys[ZOOMOUT],
					boost::bind(&Printer::zoomOut, this),
					boost::none, 0, 100);

			m_keys[ZOOMRESET].addKey(SDLK_p);
			event->addPEvent( "zoom_reset", &m_keys[ZOOMRESET],
					boost::bind(&Printer::zoomReset, this) );
		}
	}
}

void Printer::end()
{
	m_quit = true;
}

void Printer::loadTimeout()
{
	if( verbose() )
		std::cout << "Setting timout." << std::endl;

	m_timeout = new Timer( m_config.timeout_t * 1000,
			boost::bind(&Printer::end, this) );
}

void Printer::move(const sdl::Event* ev)
{
	if( !m_picts[m_act].zbigger
			&& (!m_picts[m_act].bigger
				|| !m_config.real) ) // Si l'image n'est pas plus grande, on ne la déplace pas
		return;

	Uint32 timeElapsed = SDL_GetTicks() - m_lastTime;
	float vit = m_pixpermilli * timeElapsed;
	if( ev->isKeyPressed(SDLK_v) )
		vit *= 3;

	if( ev->isKeyPressed(SDLK_e) ) m_ydec += vit;
	if( ev->isKeyPressed(SDLK_d) ) m_ydec -= vit;
	if( ev->isKeyPressed(SDLK_f) ) m_xdec -= vit;
	if( ev->isKeyPressed(SDLK_s) ) m_xdec += vit;
}

void Printer::resetmv()
{
	m_ydec = m_xdec = 0.0;
}

void Printer::zoomIn()
{
	Picture& actual = m_picts[m_act];
	float fact = actual.zfact + 0.1;
	zoomReset();
	actual.zoom = zoomSurface(actual.surf, fact, fact, actual.aa);
	actual.zfact = fact;

	if( actual.zoom == NULL )
		zoomReset();
	zTestBigger();
}

void Printer::zoomOut()
{
	Picture& actual = m_picts[m_act];
	float fact = actual.zfact - 0.1;
	if( fact <= 0.1 )
		fact = 0.1;

	zoomReset();
	actual.zoom = zoomSurface(actual.surf, fact, fact, actual.aa);
	actual.zfact = fact;

	if( actual.zoom == NULL )
		zoomReset();
	zTestBigger();
}

void Printer::zoomReset()
{
	if( m_picts[m_act].zoom != NULL )
	{
		SDL_FreeSurface(m_picts[m_act].zoom);
		m_picts[m_act].zoom = NULL;
	}
	m_picts[m_act].zfact = 1.0;
	zTestBigger();
}

void Printer::zTestBigger()
{
	Picture& actual = m_picts[m_act];
	if( actual.zoom != NULL
			&& (actual.zoom->w >= ecran->w
				|| actual.zoom->h >= ecran->h) )
		actual.zbigger = true;
	else
		actual.zbigger = actual.bigger;
}

void Printer::initZoom()
{
	for(size_t i=0; i < m_picts.size(); ++i)
	{
		m_picts[i].zoom = NULL;
		m_picts[i].zbigger = m_picts[i].bigger;
		m_picts[i].zfact = 1.0;
	}
}

