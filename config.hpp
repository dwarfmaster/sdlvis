#ifndef DEF_CONFIG
#define DEF_CONFIG

#include <SDL/SDL.h>
#include <SDLP_tools.hpp>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

namespace opt = boost::program_options;
typedef boost::filesystem::path path_t;

struct config
{
	bool text; // Afficher le nom de l'image
	bool ptext;
	bool textaa; // Utiliser l'anti aliasing pour le texte
	bool number; // Afficher son numéro

	SDL_Rect size; // Taille de la fenêtre
	bool fullscreen; // Plein écran

	bool redim; // Redimentionner les images de tailles inférieures
	bool real; // Faut-il laisser dépasser les images plus grandes
	bool deform; // Déforme l'image pour remplir au mieux la fenêtre

	bool diap; // Diaporama
	Uint32 time; // Temps en millisecondes entre 2 images
	bool loop; // Revenir au début après la fin et inversement

	bool begin; // Charger toutes les images au démarrage
	bool prep; // Préparer toutes le images au démarrage

	enum Verbosity{MUTE, NORMAL, VERBOSE, HIGH};
	Verbosity verb;

	std::vector<path_t> paths; // Les images
};

class ConfigLoader
{
	public:
		ConfigLoader(int argc, char *argv[]); // Va charger les paramètres
		bool load(); // false indique que le programme ne doit pas continuer
		config get() const;

	private:
		opt::options_description m_desc; // Description des options
		opt::options_description m_opts; // Les options
		opt::variables_map m_vm; // Les variables
		std::vector<std::string> m_unknown; // Options inconnues

		config m_config; // La configuration

		SDL_Rect parseSize(const std::string& size);
		SDL_Rect maxSize(); // Taille max de l'écran (pour fullscreen)
		void checkPaths(); // Supprime les images dont les fichiers sont invalides
		void print(); // Affiche un compte rendu des options

		ConfigLoader();
		ConfigLoader(const ConfigLoader&);
		ConfigLoader& operator=(const ConfigLoader&);
};

#endif
