#ifndef DEF_PRINT
#define DEF_PRINT

#include <boost/bind.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDLP_event.hpp>
#include <SDLP_tools.hpp>

#include "config.hpp"
#include "timer.hpp"

class Printer
{
	public:
		Printer(config conf);
		void run();
		~Printer();

	private:
		struct Picture
		{
			Picture() : path(""), surf(NULL), txt(NULL), redx(1.0), redy(1.0), prepared(false)
			{}

			path_t path;
			SDL_Surface* surf;
			SDL_Surface* txt;
			double redx;
			double redy;

			bool bigger; // Plus grand que la fenêtre
			bool prepared; // Évite de préparer 2 foix
			bool aa;
		};

		config m_config;
		std::vector<Picture> m_picts;

		void load();
		bool prepare(Picture* pict, bool force_quard = false);
		void load(Picture* pict, bool aa = true); // La picture doit être préparée avant
		void loadTxt(Picture* pict, bool aa = false, size_t number=0);
		void loadFont();
		void loadWindow(); // Chargement de la fenêtre
		void loadErr(); // Chargement de l'image d'erreur
		void loadFirst();
		void loadDiap();
		void loadKeys(sdl::Event* event);
		void toggleAA();
		bool m_aa;

		inline bool mute();
		inline bool verbose();
		inline bool highverb();

		Timer* m_diapTimer; // Le timer pour le diaporama

		SDL_Surface* ecran;
		SDL_Surface* m_err; // Image à afficher en cas d'erreur
		TTF_Font* m_font;

		enum{NEXT, PREV, FIRST, LAST, TOGGLE, TAA, NUMBER};
		std::vector< sdl::AnEvent > m_keys;

		size_t m_act;
		void next(bool ret = false); // fonctions de déplacement
		void prev(bool ret = false);
		void first();
		void last();
		void update(size_t last);

		Printer();
		Printer(const Printer&);
		Printer& operator=(const Printer&);
};

#endif
