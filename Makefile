include param.mk

PGDIR=/home/luc/Prog/
RCDIR=/usr/share/sdlvis/
RCS=font.ttf err.png

all : $(OBJS) png
	cd src && make

clean :
	cd src && make clean
	rm err.png

rec : clean all

install : png all
	@cp -v $(PROG) $(PGDIR)
	@mkdir -p $(RCDIR)
	@cp -v $(RCS) $(RCDIR)

uninstall :
	@touch $(PGDIR)$(PROG) $(RCDIR)
	@rm -v $(PGDIR)$(PROG)
	@rm -rv $(RCDIR)

reinstall : uninstall rec install

png : err.svg
	convert $< -size 800x600 err.png

.PHONY: png reinstall uninstall install rec clean all

