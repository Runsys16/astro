
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

$(OBJDIR)main.o: $(SRCDIR)main.cpp $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)timer.h $(SRCDIR)status.cpp $(SRCDIR)captures.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
		
$(OBJDIR)v4l2.o: $(SRCDIR)v4l2.cpp $(SRCDIR)v4l2.h $(SRCDIR)control.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)control.o: $(SRCDIR)control.cpp $(SRCDIR)control.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)timer.o: $(SRCDIR)timer.cpp $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)camera.o: $(SRCDIR)camera.cpp $(SRCDIR)camera.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)camera_mgr.o: $(SRCDIR)camera_mgr.cpp $(SRCDIR)camera_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)pleiade.o: $(SRCDIR)pleiade.cpp $(SRCDIR)pleiade.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)serial.o: $(SRCDIR)serial.cpp $(SRCDIR)serial.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)connexion_mgr.o: $(SRCDIR)connexion_mgr.cpp $(SRCDIR)connexion_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_console_serial.o: $(SRCDIR)panel_console_serial.cpp $(SRCDIR)panel_console_serial.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)console.o: $(SRCDIR)console.cpp $(SRCDIR)console.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)serveur_mgr.o: $(SRCDIR)serveur_mgr.cpp $(SRCDIR)serveur_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)capture.o: $(SRCDIR)capture.cpp $(SRCDIR)capture.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)var_mgr.o: $(SRCDIR)var_mgr.cpp $(SRCDIR)var_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)alert_box.o: $(SRCDIR)alert_box.cpp $(SRCDIR)alert_box.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)file_browser.o: $(SRCDIR)file_browser.cpp $(SRCDIR)file_browser.h $(SRCDIR)panel_dir.h $(SRCDIR)panel_file.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_capture.o: $(SRCDIR)panel_capture.cpp $(SRCDIR)panel_capture.h $(SRCDIR)stars.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_camera.o: $(SRCDIR)panel_camera.cpp $(SRCDIR)panel_camera.h $(SRCDIR)stars.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_zoom.o: $(SRCDIR)panel_zoom.cpp $(SRCDIR)panel_zoom.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)star.o: $(SRCDIR)star.cpp $(SRCDIR)star.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)stars.o: $(SRCDIR)stars.cpp $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)captures.o: $(SRCDIR)captures.cpp $(SRCDIR)captures.h $(SRCDIR)stars.h $(SRCDIR)capture.h $(SRCDIR)main.h $(SRCDIR)fits.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_dir.o: $(SRCDIR)panel_dir.cpp $(SRCDIR)panel_dir.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_file.o: $(SRCDIR)panel_file.cpp $(SRCDIR)panel_file.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)bluetooth.o: $(SRCDIR)bluetooth.cpp $(SRCDIR)bluetooth.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)surveillance.o: $(SRCDIR)surveillance.cpp $(SRCDIR)surveillance.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)fits.o: $(SRCDIR)fits.cpp $(SRCDIR)fits.h $(SRCDIR)main.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_fits_correction.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_courbe.o: $(SRCDIR)panel_courbe.cpp $(SRCDIR)panel_courbe.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_apn.o: $(SRCDIR)panel_apn.cpp $(SRCDIR)panel_apn.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_stdout.o: $(SRCDIR)panel_stdout.cpp $(SRCDIR)panel_stdout.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_spin_edit_text.o: $(SRCDIR)panel_spin_edit_text.cpp $(SRCDIR)panel_spin_edit_text.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)star_catalogue.o: $(SRCDIR)star_catalogue.cpp $(SRCDIR)star_catalogue.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)catalog.o: $(SRCDIR)catalog.cpp $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_fits.o: $(SRCDIR)panel_fits.cpp $(SRCDIR)panel_fits.h $(SRCDIR)var_mgr.h $(SRCDIR)main.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
$(OBJDIR)panel_fits_correction.o: $(SRCDIR)panel_fits_correction.cpp $(SRCDIR)panel_fits.h $(SRCDIR)var_mgr.h $(SRCDIR)main.h $(SRCDIR)panel_spin_edit_text.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	

$(BIN_NAME): $(OBJDIR)main.o $(OBJDIR)v4l2.o $(OBJDIR)control.o $(OBJDIR)timer.o $(OBJDIR)camera.o \
             $(OBJDIR)camera_mgr.o $(OBJDIR)pleiade.o $(OBJDIR)serial.o $(OBJDIR)connexion_mgr.o \
             $(OBJDIR)panel_console_serial.o $(OBJDIR)console.o $(OBJDIR)serveur_mgr.o  $(OBJDIR)capture.o \
             $(OBJDIR)var_mgr.o $(OBJDIR)alert_box.o $(OBJDIR)file_browser.o $(OBJDIR)panel_capture.o \
             $(OBJDIR)stars.o $(OBJDIR)star.o $(OBJDIR)panel_camera.o $(OBJDIR)panel_zoom.o \
             $(OBJDIR)captures.o $(OBJDIR)panel_dir.o $(OBJDIR)panel_file.o $(OBJDIR)bluetooth.o \
             $(OBJDIR)surveillance.o $(OBJDIR)fits.o $(OBJDIR)panel_courbe.o $(OBJDIR)panel_apn.o \
             $(OBJDIR)panel_stdout.o $(OBJDIR)panel_spin_edit_text.o $(OBJDIR)star_catalogue.o \
             $(OBJDIR)catalog.o $(OBJDIR)panel_fits.o $(OBJDIR)panel_fits_correction.o
	@echo -- Edition des liens -----
	$(GPP) $^ -o $(BIN_NAME) $(LIBS) 


