
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

#include "color.hpp"

namespace opt = boost::program_options;
typedef boost::filesystem::path path_t;

struct config
{
	bool text; // Afficher le nom de l'image
	bool ptext;
	bool textaa; // Utiliser l'anti aliasing pour le texte
	bool number; // Afficher son numéro
	Color bgcolor; // Couleur de fond

	SDL_Rect size; // Taille de la fenêtre
	bool fullscreen; // Plein écran

	bool redim; // Redimentionner les images de tailles inférieures
	bool real; // Faut-il laisser dépasser les images plus grandes
	bool deform; // Déforme l'image pour remplir au mieux la fenêtre
	bool inter; // Interactif
	bool mouse; // Montrer la souri

	bool diap; // Diaporama
	Uint32 time; // Temps en millisecondes entre 2 images
	bool loop; // Revenir au début après la fin et inversement

	bool begin; // Charger toutes les images au démarrage
	bool prep; // Préparer toutes le images au démarrage

	bool timeout; // Fin du programme au bout d'un temps
	Uint32 timeout_t; // Le temps
	bool noquit; // Empêche de quitter manuellement

	path_t dir; // Dossier des fichiers

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
