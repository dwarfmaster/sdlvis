#include "print.hpp"


	Printer::Printer(config conf)
: m_config(conf)
{
	if( verbose() )
		std::cout << "\n" << std::endl; // Espace avant les messages de la suite

	if( !mute() )
		std::cout << "Chargements." << std::endl;

	loadWindow();
	loadErr();
	if( m_config.text )
		loadFont(); // Charge la police
	load(); // Charge les images

	if( m_config.diap )
		loadDiap();

	loadFirst();

	if( !m_config.mouse )
		SDL_ShowCursor( SDL_DISABLE );
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

	while( !event.quit() )
	{
		event.update();
		if( m_config.diap )
			m_diapTimer->update();

		SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

		pos.x = ecran->w / 2 - m_picts[m_act].surf->w / 2;
		pos.y = ecran->h / 2 - m_picts[m_act].surf->h / 2;
		SDL_BlitSurface(m_picts[m_act].surf, NULL, ecran, &pos);

		if( m_config.text )
		{
			pos.x = ecran->w /2 - m_picts[m_act].txt->w / 2;
			pos.y = ecran->h - m_picts[m_act].txt->h;
			SDL_BlitSurface(m_picts[m_act].txt, NULL, ecran, &pos);
		}

		SDL_Flip(ecran);

		SDL_Delay( 1000/30 );
	}
}

Printer::~Printer()
{
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
	fg.r = fg.g = fg.b = 255;

	std::string name("");

	if( m_config.number )
	{
		name += "[";
		name += sdl::itoa(number);
		name += "/";
		name += sdl::itoa( m_picts.size() );
		name += "]> ";
	}

	name += pict->path.filename().string();

	if( highverb() )
		std::cout << "\t-> Loading text surface for " << name << std::endl;
	SDL_Surface* tmp = NULL;

	if( aa )
		tmp = TTF_RenderText_Blended(m_font, name.c_str(), fg);
	else
		tmp = TTF_RenderText_Solid(m_font, name.c_str(), fg);

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
	
	SDL_Surface* tmp = NULL;
	if( pict->surf == NULL)
		tmp = IMG_Load( pict->path.string().c_str() );
	else
		tmp = pict->surf;

	if(tmp == NULL)
	{
		if( !mute() )
			std::cout << "Erreur au chargement de l'image " << pict->path.string() << std::endl;
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
				std::cout << "Erreur au redimentionnement de l'image " << pict->path.string() << std::endl;
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
}

void Printer::loadWindow()
{
	if( !mute() )
		std::cout << "Chargement de la fenêtre." << std::endl;

	Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	if( m_config.fullscreen )
		flags |= SDL_FULLSCREEN;
	ecran = SDL_SetVideoMode(m_config.size.w, m_config.size.h, 32, flags);

	if(ecran == NULL)
		throw std::string("Erreur à l'ouverture de la fenêtre.");
}

void Printer::loadErr()
{
	std::string path = m_config.dir.string() + "/err.png";
	m_err = IMG_Load(path.c_str());
	if(m_err == NULL)
		throw std::string("Erreur au chargement de l'image d'erreur.");
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
	}
}


