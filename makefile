
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

main.o: main.cpp v4l2.h control.h timer.h
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
	
$(BIN_NAME): main.o v4l2.o control.o timer.o camera.o camera_mgr.o pleiade.o
	@echo -- Edition des liens -----
	$(GPP) $^ -o $(BIN_NAME) $(LIBS) 

