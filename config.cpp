#include "config.hpp"

ConfigLoader::ConfigLoader(int argc, char *argv[])
	: m_desc("Allowed options")
{
	std::string home=getenv("HOME");
	path_t conf_path;

	// Les options
	opt::options_description generic("Generic options");
	generic.add_options()
		("help,h", "Produce help message")
		("version,V", "Print version, don't launch the program")
		("config,c", opt::value<path_t>(&conf_path)->default_value(home + "/.sdlvis.conf"), "Use a configuration file")
		;

	opt::options_description verbosity("Verbosity options");
	verbosity.add_options()
		("mute,m", "Print no information, disable verbose and highverb")
		("verbose,v", "Print more informations, disable highverb and disabled by mute")
		("highverb", "Print lots of informations, disabled by mute and verbose")
		;

	opt::options_description draw("Drawing options");
	draw.add_options()
		("text,t", "Print the name of the picture on the screen")
		("number,n", "Print the numbre of the picture on the screen")
		("redim,r", "Resize the pictures smaller than the screen")
		("real,R", "No resize the pictures largest than the screen")
		("diap,d", opt::value<Uint32>(&m_config.time)->default_value(0), "Time in milliseconds between two pictures in diaporama (0 disable the diaporama)")
		;

	opt::options_description screen("Screen options");
	screen.add_options()
		("size,s", opt::value<std::string>()->composing(), "The size of the window, like --size 800x600")
		("fullscreen,f", "Open the window in fullscreen mode")
		("fullsize,F", "Set the window in fullscreen mode, with the size of the screen, disable size and enable fullscreen")
		;

	opt::options_description load("Loading options");
	load.add_options()
		("preload,L", "Load all the pictures at beginning, enable prepare")
		("prepare,p", "Scan all the pictures at beginning")
		("dontcheck", "Don't check the pictures paths at beginning")
		;

	opt::options_description hidden;
	hidden.add_options()
		("picture", opt::value< std::vector<path_t> >()->composing(), "A picture to load")
		;

	// Classement
	opt::options_description cmdline;
	cmdline.add(generic).add(verbosity).add(draw).add(load).add(hidden);
	opt::options_description conf;
	conf.add(verbosity).add(draw).add(load).add(hidden);
	m_desc.add(generic).add(verbosity).add(draw).add(load);
	m_opts.add(m_desc).add(hidden);

	opt::positional_options_description pos;
	pos.add("picture", -1);

	// Lecture de la ligne de commande
	opt::store( opt::command_line_parser(argc, argv).options(m_opts).positional(pos).run(), m_vm );
	opt::notify(m_vm);

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
		std::cout << "\tsdlvis 0.1\n Luc Chabassier <luc.linux@mailoo.org>\n" << std::endl;
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
	m_config.text = m_vm.count("text");
	m_config.redim = m_vm.count("redim");
	m_config.real = m_vm.count("real");

	if( m_config.time != 0 )
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

	m_config.begin = m_vm.count("preload");
	m_config.prep = m_vm.count("prepare") || m_config.begin;

	if( !m_vm.count("picture") )
		throw std::string("Aucune image n'a été donnée.");
	m_config.paths = m_vm["picture"].as< std::vector<path_t> >();

	if( !m_vm.count("dontcheck") )
		checkPaths();
	if( m_config.paths.empty() )
		throw std::string("Aucune image valable n'a été donnée.");

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
	for(size_t k = 0; cont; ++k)
	{
		if(k >= size.size())
		{
			unsigned int nb = 0;
			std::istringstream(tmp) >> nb;
			if(first)
				m_config.size.w = nb;
			else
				m_config.size.h = nb;
			cont = false;
		}
		else if(size[k] >= '0' 
				&& size[k] <= '9')
			tmp += size[k];
		else if(size[k] == '*' 
				|| size[k] == 'x')
		{
			unsigned int nb = 0;
			std::istringstream(tmp) >> nb;
			if(first)
			{
				m_config.size.w = nb;
				tmp = "";
				first = false;
			}
			else
			{
				m_config.size.h = nb;
				cont = false;
			}
		}
		else
		{
			unsigned int nb = 0;
			std::istringstream(tmp) >> nb;
			if(first)
				m_config.size.w = nb;
			else
				m_config.size.h = nb;
			cont=false;
		}
	}
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
			m_config.paths.erase(it);
	}
}


