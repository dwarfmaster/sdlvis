include param.mk

OBJS=main.o print.o config.o timer.o

LDFLAGS=`sdl-config --libs` `pkg-config --libs SDLP_event SDLP_tools SDL_image SDL_gfx SDL_ttf` -lboost_system -lboost_filesystem -lboost_program_options
CXXFLAGS=`sdl-config --cflags` `pkg-config --cflags SDLP_event SDLP_tools SDL_image SDL_gfx SDL_ttf` -Wall -g -std=gnu++0x -DVERSION=\"$(VERSION)\"

PGDIR=/home/luc/Prog/
RCDIR=/usr/share/sdlvis/
RCS=font.ttf err.png

all : $(OBJS)
	g++ $(CXXFLAGS)	   -o $(PROG)	$^ $(LDFLAGS)

main.o : main.cpp
	g++ $(CXXFLAGS)	-c -o $@	$<

%.o : %.cpp %.hpp
	g++ $(CXXFLAGS)	-c -o $@	$<

clean :
	@touch $(OBJS) $(PROG)
	@rm -v $(OBJS) $(PROG)

rec : clean all

install : png all
	@cp -v $(PROG) $(PGDIR)
	@mkdir $(RCDIR)
	@cp -v $(RCS) $(RCDIR)

uninstall :
	@touch $(PGDIR)$(PROG) $(RCDIR)
	@rm -v $(PGDIR)$(PROG)
	@rm -rv $(RCDIR)

reinstall : uninstall png rec install

png : err.svg
	convert $< -size 800x600 err.png

.PHONY: png

