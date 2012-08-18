include ../param.mk

DEST=../$(PROG)
OBJS=main.o print.o config.o timer.o

LDFLAGS=`sdl-config --libs` `pkg-config --libs SDLP_event SDLP_tools SDL_image SDL_gfx SDL_ttf` -lboost_system -lboost_filesystem -lboost_program_options
CXXFLAGS=`sdl-config --cflags` `pkg-config --cflags SDLP_event SDLP_tools SDL_image SDL_gfx SDL_ttf` -Wall -g -std=gnu++0x -DVERSION=\"$(VERSION)\"

all : $(OBJS)
	g++ $(CXXFLAGS)	   -o $(DEST)	$^ $(LDFLAGS)

main.o : main.cpp
	g++ $(CXXFLAGS)	-c -o $@	$<

%.o : %.cpp %.hpp
	g++ $(CXXFLAGS)	-c -o $@	$<

clean :
	@touch $(OBJS) $(DEST)
	@rm -v $(OBJS) $(DEST)

rec : clean all


