#include "config.hpp"

ConfigLoader::ConfigLoader(int argc, char *argv[])
	: m_desc("Allowed options")
{
	std::string home = getenv("HOME");
	path_t conf_path;

	// Les options
	opt::options_description generic("Generic options");
	generic.add_options()
		("help,h", "Produce help message")
		("version", "Print version, don't launch the program")
		("config,c", opt::value<path_t>(&conf_path)->default_value(home + "/.sdlvis.cfg"), "Use a configuration file")
		;

	opt::options_description verbosity("Verbosity options");
	verbosity.add_options()
		("mute,m", "Print no information, disable verbose and highverb")
		("verbose,v", "Print more informations, disable highverb and disabled by mute")
		("highverb,V", "Print lots of informations, disabled by mute and verbose")
		;

	opt::options_description draw("Drawing options");
	draw.add_options()
		("text,t", "Print the name of the picture on the screen")
		("pretext,T", "Preload the names of the pictures, enable text")
		("aatext,A", "Use anti-aliasing for the text, enable text")
		("number,n", "Print the numbre of the picture on the screen")
		("redim,r", "Resize the pictures smaller than the screen")
		("real,R", "No resize the pictures largest than the screen")
		("deform,D", "Deform the picture when they are resized to fill the window")
		("diap,d", opt::value<Uint32>(&m_config.time)->default_value(0), "Time in milliseconds between two pictures in diaporama (0 disable the diaporama)")
		("loop,l", "After the last, go to first, and inverse")
		("nointer", "Disable the keys")
		("hidemouse,h", "Hide the mouse")
		;

	opt::options_description screen("Screen options");
	screen.add_options()
		("size,s", opt::value<std::string>()->composing(), "The size of the window, like --size 800x600")
		("fullscreen,f", "Open the window in fullscreen mode, enable hidemouse")
		("fullsize,F", "Set the window in fullscreen mode, with the size of the screen, override size and enable fullscreen")
		;

	opt::options_description load("Loading options");
	load.add_options()
		("preload,L", "Load all the pictures at beginning, enable prepare")
		("prepare,p", "Scan all the pictures at beginning")
		("dontcheck", "Don't check the pictures paths at beginning")
		("res",  opt::value<path_t>(&m_config.dir)->default_value("/usr/share/sdlvis/"), "Directory for files.")
		;

	opt::options_description hidden;
	hidden.add_options()
		("picture", opt::value< std::vector<path_t> >()->composing(), "A picture to load")
		;

	// Classement
	opt::options_description cmdline;
	cmdline.add(generic).add(verbosity).add(draw).add(load).add(screen).add(hidden);
	opt::options_description conf;
	conf.add(verbosity).add(draw).add(load).add(screen).add(hidden);
	m_desc.add(generic).add(verbosity).add(draw).add(screen).add(load);
	m_opts.add(m_desc).add(hidden);

	opt::positional_options_description pos;
	pos.add("picture", -1);

	// Lecture de la ligne de commande
	opt::parsed_options parsed = opt::command_line_parser(argc, argv)
		.options(m_opts).positional(pos)
		.allow_unregistered().run();
	opt::store(parsed, m_vm);
	opt::notify(m_vm);

	m_unknown = opt::collect_unrecognized(parsed.options, opt::exclude_positional);

	// Lecture du fichier
	boost::filesystem::ifstream file(conf_path);
	if(file)
	{
		opt::store( opt::parse_config_file(file, conf), m_vm );
		opt::notify(m_vm);
	}
}

bool ConfigLoader::load()
{
	if( m_vm.count("version") )
		std::cout << "\tsdlvis " 
#ifdef VERSION
			<< VERSION 
#endif
			<< "\n Luc Chabassier <luc.linux@mailoo.org>\n" << std::endl;
	if( m_vm.count("help") )
		std::cout << m_desc << std::endl;
	if( m_vm.count("version") )
		return false; // Don't launch the program

	if( m_vm.count("mute") )
		m_config.verb = config::MUTE;
	else if( m_vm.count("verbose") )
		m_config.verb = config::VERBOSE;
	else if( m_vm.count("highverb") )
		m_config.verb = config::HIGH;
	else
		m_config.verb = config::NORMAL;

	m_config.number = m_vm.count("number");
	m_config.ptext = m_vm.count("pretext");
	m_config.textaa = m_vm.count("aatext");
	m_config.text = m_vm.count("text") || m_config.ptext || m_config.textaa;
	m_config.redim = m_vm.count("redim");
	m_config.real = m_vm.count("real");
	m_config.deform = m_vm.count("deform");
	m_config.loop = m_vm.count("loop");
	m_config.inter = !m_vm.count("nointer");

	if( m_config.time == 0 )
		m_config.diap = false;
	else
		m_config.diap = true;

	m_config.fullscreen = m_vm.count("fullscreen") || m_vm.count("fullsize");
	if( m_vm.count("size") && !m_vm.count("fullsize") )
		m_config.size = parseSize( m_vm["size"].as<std::string>() );
	else if( m_config.fullscreen )
		m_config.size = maxSize();
	else
	{
		m_config.size.w = 800;
		m_config.size.h = 600;
	}

	m_config.mouse = !m_vm.count("hidemouse") && !m_vm.count("fullscreen");

	m_config.begin = m_vm.count("preload");
	m_config.prep = m_vm.count("prepare") || m_config.begin;

	if( !m_vm.count("picture") )
		throw std::string("There is no picture.");
	m_config.paths = m_vm["picture"].as< std::vector<path_t> >();

	if( !m_vm.count("dontcheck") )
		checkPaths();
	if( m_config.paths.empty() )
		throw std::string("There is no usable picture.");

	print();

	return true;
}

config ConfigLoader::get() const
{
	return m_config;
}

SDL_Rect ConfigLoader::parseSize(const std::string& size)
{
	std::string tmp(""); // sert à stocker la string du nombre
	bool cont = true, first = true;
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = 800;
	rect.h = 600;
	for(size_t k = 0; cont; ++k)
	{
		if(k >= size.size())
		{
			unsigned int nb = 0;
			nb = sdl::atoi(tmp);
			if(first)
				rect.w = nb;
			else
				rect.h = nb;
			cont = false;
		}
		else if(size[k] >= '0' 
				&& size[k] <= '9')
			tmp += size[k];
		else if(size[k] == '*' 
				|| size[k] == 'x')
		{
			unsigned int nb = 0;
			nb = sdl::atoi(tmp);
			if(first)
			{
				rect.w = nb;
				tmp = "";
				first = false;
			}
			else
			{
				rect.h = nb;
				cont = false;
			}
		}
		else // Peu probable
		{
			unsigned int nb = 0;
			std::istringstream(tmp) >> nb;
			if(first)
				rect.w = nb;
			else
				rect.h = nb;
			cont=false;
		}
	}

	return rect;
}

SDL_Rect ConfigLoader::maxSize()
{
	SDL_Rect** modes = SDL_ListModes(NULL, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

	if(modes == (SDL_Rect**)0)
		return {0, 0, 0, 0};
	else if(modes == (SDL_Rect**)-1)
		return {0, 0, 1600, 900};
	else
		return *modes[0];
}

void ConfigLoader::checkPaths()
{
	for(std::vector<path_t>::iterator it = m_config.paths.begin(); it != m_config.paths.end(); ++it)
	{
		if( !boost::filesystem::exists(*it)
				|| !boost::filesystem::is_regular_file(*it) )
		{
			if( m_config.verb >= config::HIGH )
				std::cout << "Warning : the file " << it->string() << " is not regular or inexistant." << std::endl;

			std::vector<path_t>::iterator tit = it;
			--it;
			m_config.paths.erase(tit);
		}
	}
}

void ConfigLoader::print()
{
	if( m_config.verb > config::MUTE
			&& !m_unknown.empty() )
	{
		std::cout << "Warning, this options are unknown :" << std::endl;
		for(size_t i=0; i < m_unknown.size(); ++i)
			std::cout << "\t>> " << m_unknown[i] << std::endl;
		std::cout << std::endl;
	}

	if( m_config.verb >= config::VERBOSE )
	{
		std::cout << "Options :" << std::endl;
		std::cout << "\tPrint number : " << (m_config.number ? "yes" : "no") << std::endl;
		std::cout << "\tPrint picture name : " << (m_config.text ? "yes" : "no") << std::endl;
		if(m_config.text)
			std::cout << "\t\t-> Preload text : " << (m_config.ptext ? "yes" : "no") << std::endl;
		std::cout << "\tRedimension smaller pictures : " << (m_config.redim ? "yes" : "no") << std::endl;
		std::cout << "\tRedimension bigger pictures : " << (!m_config.real ? "yes" : "no") << std::endl;
		std::cout << "\tDeform redimensionned pictures : " << (!m_config.deform ? "yes" : "no") << std::endl;
		std::cout << "\tDiaporama : " << (m_config.diap ? "yes" : "no") << std::endl;
		if(m_config.diap)
			std::cout << "\t\t-> time : " << m_config.time << std::endl;
		std::cout << "\tFullscreen : " << (m_config.fullscreen ? "yes" : "no") << std::endl;
		std::cout << "\tWindow size : " << m_config.size.w << "x" << m_config.size.h << std::endl;
		std::cout << "\tPrepare pictures : " << (m_config.prep ? "yes" : "no") << std::endl;
		std::cout << "\tPreload pictures : " << (m_config.begin ? "yes" : "no") << std::endl;
		if( m_config.verb >= config::HIGH )
		{
			std::cout << "\tPictures to load (" << m_config.paths.size() << ") :" << std::endl;
			for(size_t i=0; i < m_config.paths.size(); ++i)
				std::cout << "\t\t" << i+1 << "> " << m_config.paths[i].string() << std::endl;
		}
	}
}

