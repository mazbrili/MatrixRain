.PHONY: all clean $(PROGRAM)

PROGRAM	:= mrain
SRC	:= $(wildcard *.cpp) $(wildcard *.c)
DEP	:= $(patsubst %.cpp,%.d,$(SRC))  
OBJ	:= $(patsubst %.cpp,%.o,$(SRC))

CFLAGS	:= -Wall -O3 -funroll-loops  
INCLUDES:= -I. -I/usr/include/ -I/usr/
DEFINES	:= -DSTANDALONE -DHAVE_GL -DUSE_GL -DGL_GLEXT_PROTOTYPES -DGETTIMEOFDAY_TWO_ARGS -DHAVE_UNISTD_H
LIBS	:= -lX11 -lGL


PROJ_FILES	:= *.cpp *.h Makefile devlog manpage.1
PUBLIC_NAME	:= MatrixRain
PACK_NAME	:= $(PROGRAM)-0.0.`date +%G%m%d`
DESTDIR 	:=

INSTALL_BIN_DIR	:= /usr/lib/xscreensaver
INSTALL_DTP_DIR	:= /usr/share/applications/screensavers
INSTALL_MAN_DIR	:= /usr/share/man/man1


all: $(PROGRAM)

include $(wildcard *.d)

build-deb: build-tar
	mkdir $(PACK_NAME)
	cp -r $(PROJ_FILES) ./$(PACK_NAME)/
	cd $(PACK_NAME);\
	dh_make -s -c GPL -e pavel_karneliuk@users.sourceforge.net -f ../$(PACK_NAME).tar.gz;\
	rm -rf ./debian;\
	cp -r ../../debian ./debian;\
	dpkg-buildpackage -rfakeroot

build-tar: clean
	vim devlog
	echo `date -R` | cat - devlog >/tmp/devlog
	mv /tmp/devlog devlog
	tar -cvvzf $(PACK_NAME).tar.gz $(PROJ_FILES)

clean:
	rm -rf $(PROGRAM)* *.o *.d *.gz *~

$(PROGRAM): $(OBJ)
	g++ $(CFLAGS) -o $@ $(OBJ) $(LIBS)

$(OBJ): %.o: %.cpp
	g++ $(CFLAGS) $(INCLUDES) $(DEFINES) -MD -c $< -o $@

install: $(PROGRAM)
	mkdir -p $(DESTDIR)/$(INSTALL_BIN_DIR)
	install ./$(PROGRAM) $(DESTDIR)/$(INSTALL_BIN_DIR)/$(PROGRAM)
	install ./manpage.1 $(DESTDIR)/$(INSTALL_MAN_DIR)/$(PROGRAM).1
	mkdir -p $(DESTDIR)/$(INSTALL_DTP_DIR)
	echo "\n"\
	"[Desktop Entry]\n"\
	"Encoding=UTF-8\n"\
	"Name="$(PUBLIC_NAME)"\n"\
	"Comment=You in the Matrix.\n"\
	"TryExec="$(INSTALL_BIN_DIR)/$(PROGRAM)"\n"\
	"Exec="$(PROGRAM)" --root\n"\
	"Path="$(INSTALL_BIN_DIR)"\n"\
	"StartupNotify=false\n"\
	"Terminal=false\n"\
	"Type=Application\n"\
	"Categories=Screensaver\n"\
	"OnlyShowIn=GNOME\n"\
	"X-Ubuntu-Gettext-Domain=xscreensaver" >$(DESTDIR)/$(INSTALL_DTP_DIR)/$(PROGRAM).desktop

uninstall:
	rm $(DESTDIR)/$(INSTALL_BIN_DIR)/$(PROGRAM)
	rm $(DESTDIR)/$(INSTALL_MAN_DIR)/$(PROGRAM).1
	rm $(DESTDIR)/$(INSTALL_DTP_DIR)/$(PROGRAM).desktop

