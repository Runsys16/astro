
GPP = g++

OBJDIR = ./build/
SRCDIR = ./src/

CFLAGS_PYTHON = `pkg-config --cflags python3`
CLIBS_PYTHON = `pkg-config --libs python3`

INCS     =  -I"/usr/include" -I"/usr/include/freetype2" $(CFLAGS_PYTHON)
CFLAGS   = $(INCS) -g -std=c++11 -O2 -fpermissive -mtune=core2 -Wno-deprecated -Wno-unused-result

PATHLIBS = -L"/usr/local/lib" -L"/usr/lib"
LIBS	 =  $(PATHLIBS) $(CLIBS_PYTHON) -lbluetooth -lX11 -lXrandr -lpthread -lGL -lglut -lGLU -lGLEW -lIL -lwmcgl -rdynamic -ljpeg

BIN_NAME = astro



.PHONY: all
all: $(BIN_NAME)

clean:
	rm -f $(OBJDIR)*.o
	rm -f $(BIN_NAME)

$(OBJDIR)main.o: main.cpp v4l2.h control.h timer.h status.cpp captures.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
		
$(OBJDIR)v4l2.o: v4l2.cpp v4l2.h control.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)control.o: control.cpp control.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)timer.o: timer.cpp timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)camera.o: camera.cpp camera.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)camera_mgr.o: camera_mgr.cpp camera_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)pleiade.o: pleiade.cpp pleiade.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)serial.o: serial.cpp serial.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)connexion_mgr.o: connexion_mgr.cpp connexion_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_console_serial.o: panel_console_serial.cpp panel_console_serial.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)console.o: console.cpp console.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)serveur_mgr.o: serveur_mgr.cpp serveur_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)capture.o: capture.cpp capture.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)var_mgr.o: var_mgr.cpp var_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)alert_box.o: alert_box.cpp alert_box.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)file_browser.o: file_browser.cpp file_browser.h panel_dir.h panel_file.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_capture.o: panel_capture.cpp panel_capture.h stars.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_camera.o: panel_camera.cpp panel_camera.h stars.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_zoom.o: panel_zoom.cpp panel_zoom.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)star.o: star.cpp star.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)stars.o: stars.cpp stars.h star.h panel_zoom.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)captures.o: captures.cpp captures.h stars.h capture.h main.h fits.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_dir.o: panel_dir.cpp panel_dir.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_file.o: panel_file.cpp panel_file.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)bluetooth.o: bluetooth.cpp bluetooth.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)surveillance.o: surveillance.cpp surveillance.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)fits.o: fits.cpp fits.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_courbe.o: panel_courbe.cpp panel_courbe.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_apn.o: panel_apn.cpp panel_apn.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_stdout.o: panel_stdout.cpp panel_stdout.h main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	

$(BIN_NAME): $(OBJDIR)main.o $(OBJDIR)v4l2.o $(OBJDIR)control.o $(OBJDIR)timer.o $(OBJDIR)camera.o \
             $(OBJDIR)camera_mgr.o $(OBJDIR)pleiade.o $(OBJDIR)serial.o $(OBJDIR)connexion_mgr.o \
             $(OBJDIR)panel_console_serial.o $(OBJDIR)console.o $(OBJDIR)serveur_mgr.o  $(OBJDIR)capture.o \
             $(OBJDIR)var_mgr.o $(OBJDIR)alert_box.o $(OBJDIR)file_browser.o $(OBJDIR)panel_capture.o \
             $(OBJDIR)stars.o $(OBJDIR)star.o $(OBJDIR)panel_camera.o $(OBJDIR)panel_zoom.o \
             $(OBJDIR)captures.o $(OBJDIR)panel_dir.o $(OBJDIR)panel_file.o $(OBJDIR)bluetooth.o \
             $(OBJDIR)surveillance.o $(OBJDIR)fits.o $(OBJDIR)panel_courbe.o $(OBJDIR)panel_apn.o \
             $(OBJDIR)panel_stdout.o
             
	@echo -- Edition des liens -----
	$(GPP) $^ -o $(BIN_NAME) $(LIBS) 


