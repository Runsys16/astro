
GPP = g++

INCS     =  -I"/usr/include" -I"/usr/include/freetype2"
CFLAGS   = $(INCS) -g -std=c++11 -O2 -fpermissive -mtune=core2 -Wno-deprecated -Wno-unused-result

PATHLIBS = -L"/usr/local/lib" -L"/usr/lib"
LIBS	 =  $(PATHLIBS) -lX11 -lpthread -lGL -lglut -lGLU -lGLEW -lIL -lwmcgl -rdynamic -ljpeg

BIN_NAME = astro



.PHONY: all
all: $(BIN_NAME)

clean:
	rm -f *.o
	rm -f $(BIN_NAME)

main.o: main.cpp v4l2.h control.h timer.h status.cpp
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
		
v4l2.o: v4l2.cpp v4l2.h control.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
control.o: control.cpp control.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
timer.o: timer.cpp timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
camera.o: camera.cpp camera.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
camera_mgr.o: camera_mgr.cpp camera_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
pleiade.o: pleiade.cpp pleiade.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
serial.o: serial.cpp serial.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
connexion_mgr.o: connexion_mgr.cpp connexion_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
panel_console_serial.o: panel_console_serial.cpp panel_console_serial.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
console.o: console.cpp console.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
serveur_mgr.o: serveur_mgr.cpp serveur_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
capture.o: capture.cpp capture.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
var_mgr.o: var_mgr.cpp var_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
alert_box.o: alert_box.cpp alert_box.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	
file_browser.o: file_browser.cpp file_browser.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)
	

$(BIN_NAME): main.o v4l2.o control.o timer.o camera.o camera_mgr.o pleiade.o serial.o connexion_mgr.o panel_console_serial.o console.o serveur_mgr.o  capture.o var_mgr.o alert_box.o file_browser.o
	@echo -- Edition des liens -----
	$(GPP) $^ -o $(BIN_NAME) $(LIBS) 


