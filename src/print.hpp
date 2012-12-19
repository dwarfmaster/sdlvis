
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

#ifndef DEF_PRINT
#define DEF_PRINT

#include <boost/bind.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDLP_event.hpp>

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
			Picture() : path(""), surf(NULL), txt(NULL), redx(1.0), redy(1.0), prepared(false), err(false)
			{}

			path_t path;
			SDL_Surface* surf;
			SDL_Surface* txt;
			double redx;
			double redy;

			bool bigger; // Plus grand que la fenêtre
			bool prepared; // Évite de préparer 2 foix
			bool aa;
			bool err;

			SDL_Surface* zoom; // == NULL si pas de zoom
			bool zbigger; // Indique si la surface zoomée est plus grande que la fenêtre
			float zfact;
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
		void loadTimeout();
		void toggleAA();
		bool m_aa;

		inline bool mute();
		inline bool verbose();
		inline bool highverb();

		Timer* m_diapTimer; // Le timer pour le diaporama
		Timer* m_timeout;

		void end();
		bool m_quit;

		enum MoveDir{UP, DOWN, LEFT, RIGHT};
		void move(const sdl::Event* ev);
		void resetmv();
		// SDL_Rect m_dec; // Décalage de l'image par rapport au centre
		float m_xdec;
		float m_ydec;
		const float m_pixpermilli = 0.05;

		void zoomIn();
		void zoomOut();
		void zoomReset();
		void zTestBigger();
		void initZoom();

		Uint32 m_lastTime;

		SDL_Surface* ecran;
		SDL_Surface* m_err; // Image à afficher en cas d'erreur
		TTF_Font* m_font;

		enum{NEXT, PREV, FIRST, LAST, TOGGLE, TAA, MVUP, MVDOWN, MVLEFT, MVRIGHT, ZOOMIN, ZOOMOUT, ZOOMRESET, NUMBER};
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
