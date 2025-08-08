GPP = g++

OBJDIR = ./build/
SRCDIR = ./src/

SRC= $(wildcard $(SRCDIR)*.cpp)
OBJ= $(wildcard $(OBJDIR)*.o)
OBJ=$(SRC:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

CFLAGS_PYTHON = `pkg-config --cflags python3`
CLIBS_PYTHON = `pkg-config --libs python3`

INCS     =  -I"/usr/include" -I"/usr/include/freetype2" $(CFLAGS_PYTHON)
CFLAGS   = $(INCS) -g -std=c++11 -O2 -fpermissive -mtune=core2 -Wno-deprecated -Wno-unused-result

PATHLIBS = -L"/usr/local/lib" -L"/usr/lib"
LIBS	 =  $(PATHLIBS) $(CLIBS_PYTHON) -lbluetooth -lX11 -lXrandr -lpthread -lGL -lglut -lGLU -lGLEW -lIL -lwmcgl -rdynamic -ljpeg

BIN_NAME = astro



.PHONY: all
all: $(BIN_NAME)

echo:
	@echo ---------------- OBJ -------------------
	@echo ${OBJ}
	@echo ---------------- SRC -------------------
	@echo ${SRC}

clean:
	rm -f $(OBJDIR)*.o
	rm -f $(BIN_NAME)

$(BIN_NAME): $(OBJ)
	@echo -- Edition des liens -----
	@$(GPP) $^ -o $(BIN_NAME) $(LIBS) 
	@echo $(GPP) -o $(BIN_NAME) $(LIBS) 



#-------------------------------------------------------------------------------------
#        Scripts
#        Fabrication des dependances de chaque fichier.o
#-------------------------------------------------------------------------------------
# depend.py, build_makefile
#-------------------------------------------------------------------------------------
# Recuperation des scripts (dans le repertoire src) :
# $ cat ./Makefile | grep "^#00#" | sed -e "s/^....\(.*\)/\1/g" >depend.py
# $ cat ./Makefile | grep "^#01#" | sed -e "s/^....\(.*\)/\1/g" >build_makefile
#-------------------------------------------------------------------------------------



#00##!/usr/bin/python3
#00##-----------------------------------------------------------------------------
#00## depend.py
#00##-----------------------------------------------------------------------------
#00## Creation des dependance pour le fichier makefile
#00## this script run in src directory
#00##-----------------------------------------------------------------------------
#00##  exemple de ligne :
#00##		$(OBJDIR)Effects.o :   $(SRCDIR)Effects.cpp 
#00##			@echo ---------   compilation de $@
#00##			$(CPP) -c $< -o $@  $(CFLAGS)
#00##-----------------------------------------------------------------------------
#00## le programme liste toutes les entrees du repertoite courant.
#00##-----------------------------------------------------------------------------
#00#
#00#
#00#import os
#00#import sys
#00##-----------------------------------------------------------------------------
#00##
#00##-----------------------------------------------------------------------------
#00#tab = ""
#00#DEBUG=False
#00##-----------------------------------------------------------------------------
#00##
#00##-----------------------------------------------------------------------------
#00#def log( str_log ):
#00#	global tab
#00#	
#00#	if not DEBUG:
#00#		return
#00#	print( tab + str_log )
#00##-----------------------------------------------------------------------------
#00##
#00##-----------------------------------------------------------------------------
#00#def log_tab( b ):
#00#	global tab
#00#	
#00#	if ( b ):
#00#		tab = tab + "   "
#00#	else:
#00#		tab = tab[:-3]
#00##-----------------------------------------------------------------------------
#00##
#00##-----------------------------------------------------------------------------
#00#def list_dir( name="." ):
#00#	for dirname, dirnames, filenames in os.walk(name):
#00#		log( "-------------------------")
#00#		log( dirname )
#00#		#print( "-------------------------")
#00#		#print filenames
#00#		#print( "-------------------------")
#00#		#print dirnames
#00#		
#00#		listfiles = []
#00#
#00#		for filename in filenames:
#00#			if filename.find(".cpp") != -1:
#00#				#listfiles.append(os.path.join(dirname, filename))
#00#				listfiles.append( filename )
#00#			
#00#		return listfiles
#00##-----------------------------------------------------------------------------
#00##
#00##-----------------------------------------------------------------------------
#00#def have_file( includes, include ):
#00#	return (include in includes)
#00##-----------------------------------------------------------------------------
#00##
#00## Recursif
#00##
#00##-----------------------------------------------------------------------------
#00#def find_header( filename, dirname="./", includes = [], ext_dirname = "" ):
#00#	log( '-------------------------------' )
#00#	log( 'find header "%s"' % (dirname+ext_dirname+filename) )
#00#	f = ""
#00#	try:
#00#		f = open( dirname+ext_dirname+filename )
#00#	except OSError:
#00#		#print( e )
#00#		log( "Erreur d'ouverture de : \"%s\"" % (dirname+ext_dirname+filename) )
#00#		sys.exc_info()
#00#		sys.exit(1)
#00#	#else:
#00#	#	print( "OK \"%s\"" % (dirname+ext_dirname+filename) )
#00#	#	print (f )
#00#
#00#	i = 0
#00#
#00#	#log( "%d" % len(f.readlines()) )
#00#	for line in f.readlines():
#00#		log( "-ligne %d %s" % (i, line[:-1]) )
#00#		i += 1
#00#		
#00#		if line.find("#include \"") != -1 and line.split("#include \"")[0].find("//") == -1:
#00#			#print( "------------" )
#00#			include_name = line.split("\"")[1].split( dirname )[-1]
#00#			include_name = dirname + ext_dirname + include_name
#00#
#00#			try:
#00#				ext_dirname = include_name.split(dirname)[1].split("/")[-2] + "/"
#00#			except:
#00#				ext_dirname = ""
#00#			
#00#			if ( ext_dirname != "" ):
#00#				include_name = include_name.split(ext_dirname)[-1]
#00#			
#00#			make_name = ext_dirname + include_name.split(dirname)[-1]
#00#			log( "MAKENAME : %s" % (make_name) )
#00#			log( "Recursif dir  : %s" % dirname )
#00#			log( "Recursif ext  : %s" % ext_dirname )
#00#			log( "Recursif file : %s" % include_name )
#00#			log( "Recursif inc : %s" % ";".join(includes) )
#00#			
#00#			if not have_file( includes, make_name ):
#00#				includes.append( make_name )
#00#				log_tab(True)
#00#				find_header( include_name.split(dirname)[-1], dirname,  includes, ext_dirname)
#00#				log_tab(False)
#00#				
#00#
#00#
#00#	f.close()
#00#	return includes
#00##-----------------------------------------------------------------------------
#00##
#00##-----------------------------------------------------------------------------
#00##
#00##		MAIN
#00##
#00#log ( "#-------------------------------------------------------" )	
#00#log ( "#              project" )
#00#log ( "#-------------------------------------------------------" )	
#00#log
#00#source="./"
#00#source="/home/rene/programmes/opengl/video/astro/src/"
#00#for filename in sorted(list_dir(source)):
#00#
#00#	#log( "MAIN : parours : %s" % filename )
#00#
#00#	try:
#00#		includes = find_header( filename, dirname=source, includes=[] )
#00#	except:
#00#		log( "Erreur  : '%s'" % (filename) )
#00#		sys.exit(1)
#00#
#00#	name = filename.split(".cpp")[0]
#00#	
#00#	#----------------------------------------------------
#00#	# print for makefile
#00#	#----------------------------------------------------
#00#	#	for include in includes:
#00#	#	print( "$(SRCDIR)%s " % include ) ,
#00#	include = ""
#00#	if len(includes) != 0:
#00#		include = "$(SRCDIR)" + " $(SRCDIR)".join(includes)
#00#	
#00#	
#00#	print( "#--- Fabrication de %s.o -----------------------------------------" % name )
#00#	print( "$(OBJDIR)%s.o :   $(SRCDIR)%s.cpp %s" % (name, name, include) )
#00#
#00#
#00#	print
#00#	print( "\t@echo ---------   compilation de $@" )
#00#	print( "\t$(GPP) -c $< -o $@  $(CFLAGS)" )
#00#	print( "" )
#00#	
#00#sys.exit(0)
#00#




#01##!/bin/bash
#01##-----------------------------------------------------------------------------
#01## build_makefile
#01##-----------------------------------------------------------------------------
#01## this script run in src directory
#01##-----------------------------------------------------------------------------
#01#cd /home/rene/programmes/opengl/video/astro/
#01##echo $(pwd)
#01##exit
#01#cat ./scripts/makefile.base >./Makefile
#01#echo "#-------------------------------------------------------------------------------------" >> ./Makefile
#01#echo "#        Scripts" >> ./Makefile
#01#echo "#        Fabrication des dependances de chaque fichier.o" >> ./Makefile
#01#echo "#-------------------------------------------------------------------------------------" >> ./Makefile
#01#echo "# depend.py, build_makefile" >> ./Makefile
#01#echo "#-------------------------------------------------------------------------------------" >> ./Makefile
#01#echo "# Recuperation des scripts (dans le repertoire src) :" >> ./Makefile
#01#echo "# $ cat ./Makefile | grep \"^#00#\" | sed -e \"s/^....\(.*\)/\1/g\" >depend.py" >> ./Makefile
#01#echo "# $ cat ./Makefile | grep \"^#01#\" | sed -e \"s/^....\(.*\)/\1/g\" >build_makefile" >> ./Makefile
#01#echo "#-------------------------------------------------------------------------------------" >> ./Makefile
#01#
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#
#01#cat ./scripts/depend.py | sed -e 's/.*/#00#\0/g' >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#
#01#cat ./scripts/build_makefile | sed -e 's/.*/#01#\0/g' >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#echo  >> ./Makefile
#01#
#01##cd lib
#01#./scripts/depend.py >>./Makefile
#01##cd ..
#01#
#01#echo "# OK"  >>./Makefile




#--- Fabrication de alert_box.o -----------------------------------------
$(OBJDIR)alert_box.o :   $(SRCDIR)alert_box.cpp $(SRCDIR)alert_box.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de bluetooth.o -----------------------------------------
$(OBJDIR)bluetooth.o :   $(SRCDIR)bluetooth.cpp $(SRCDIR)bluetooth.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de camera.o -----------------------------------------
$(OBJDIR)camera.o :   $(SRCDIR)camera.cpp $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)camera_mgr.h $(SRCDIR)file_browser.h $(SRCDIR)panel_dir.h $(SRCDIR)panel_file.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de camera_mgr.o -----------------------------------------
$(OBJDIR)camera_mgr.o :   $(SRCDIR)camera_mgr.cpp $(SRCDIR)camera_mgr.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)pleiade.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de capture.o -----------------------------------------
$(OBJDIR)capture.o :   $(SRCDIR)capture.cpp $(SRCDIR)capture.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)panel_capture.h $(SRCDIR)fits.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_fits_correction.h $(SRCDIR)captures.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de captures.o -----------------------------------------
$(OBJDIR)captures.o :   $(SRCDIR)captures.cpp $(SRCDIR)captures.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)capture.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)panel_capture.h $(SRCDIR)fits.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_fits_correction.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de catalog.o -----------------------------------------
$(OBJDIR)catalog.o :   $(SRCDIR)catalog.cpp $(SRCDIR)catalog.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)star_catalogue.h $(SRCDIR)camera_mgr.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)star_compare.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de connexion_mgr.o -----------------------------------------
$(OBJDIR)connexion_mgr.o :   $(SRCDIR)connexion_mgr.cpp $(SRCDIR)connexion_mgr.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)camera_mgr.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de console.o -----------------------------------------
$(OBJDIR)console.o :   $(SRCDIR)console.cpp $(SRCDIR)console.h $(SRCDIR)MathlibD.h $(SRCDIR)panel_console_serial.h $(SRCDIR)Singleton.h $(SRCDIR)serial.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)panel_courbe.h $(SRCDIR)timer.h $(SRCDIR)serveur_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de control.o -----------------------------------------
$(OBJDIR)control.o :   $(SRCDIR)control.cpp $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)v4l2.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de file_browser.o -----------------------------------------
$(OBJDIR)file_browser.o :   $(SRCDIR)file_browser.cpp $(SRCDIR)file_browser.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)panel_dir.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)panel_file.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de fits.o -----------------------------------------
$(OBJDIR)fits.o :   $(SRCDIR)fits.cpp $(SRCDIR)fits.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_capture.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)panel_fits_correction.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de main.o -----------------------------------------
$(OBJDIR)main.o :   $(SRCDIR)main.cpp $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)camera.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)camera_mgr.h $(SRCDIR)connexion_mgr.h $(SRCDIR)pleiade.h $(SRCDIR)capture.h $(SRCDIR)panel_capture.h $(SRCDIR)fits.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_fits_correction.h $(SRCDIR)alert_box.h $(SRCDIR)file_browser.h $(SRCDIR)panel_dir.h $(SRCDIR)panel_file.h $(SRCDIR)captures.h $(SRCDIR)bluetooth.h $(SRCDIR)panel_apn.h $(SRCDIR)panel_stdout.h $(SRCDIR)status.inc
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_apn.o -----------------------------------------
$(OBJDIR)panel_apn.o :   $(SRCDIR)panel_apn.cpp $(SRCDIR)panel_apn.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_camera.o -----------------------------------------
$(OBJDIR)panel_camera.o :   $(SRCDIR)panel_camera.cpp $(SRCDIR)panel_camera.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)camera_mgr.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)star_compare.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_capture.o -----------------------------------------
$(OBJDIR)panel_capture.o :   $(SRCDIR)panel_capture.cpp $(SRCDIR)panel_capture.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)captures.h $(SRCDIR)capture.h $(SRCDIR)fits.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_fits_correction.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_console_serial.o -----------------------------------------
$(OBJDIR)panel_console_serial.o :   $(SRCDIR)panel_console_serial.cpp $(SRCDIR)panel_console_serial.h $(SRCDIR)MathlibD.h $(SRCDIR)Singleton.h $(SRCDIR)serial.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)timer.h $(SRCDIR)serveur_mgr.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_courbe.o -----------------------------------------
$(OBJDIR)panel_courbe.o :   $(SRCDIR)panel_courbe.cpp $(SRCDIR)panel_courbe.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_dir.o -----------------------------------------
$(OBJDIR)panel_dir.o :   $(SRCDIR)panel_dir.cpp $(SRCDIR)panel_dir.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)file_browser.h $(SRCDIR)panel_file.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_file.o -----------------------------------------
$(OBJDIR)panel_file.o :   $(SRCDIR)panel_file.cpp $(SRCDIR)panel_file.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)file_browser.h $(SRCDIR)panel_dir.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_fits.o -----------------------------------------
$(OBJDIR)panel_fits.o :   $(SRCDIR)panel_fits.cpp $(SRCDIR)panel_fits.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_fits_correction.o -----------------------------------------
$(OBJDIR)panel_fits_correction.o :   $(SRCDIR)panel_fits_correction.cpp $(SRCDIR)panel_fits_correction.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_stdout.o -----------------------------------------
$(OBJDIR)panel_stdout.o :   $(SRCDIR)panel_stdout.cpp $(SRCDIR)panel_stdout.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de panel_zoom.o -----------------------------------------
$(OBJDIR)panel_zoom.o :   $(SRCDIR)panel_zoom.cpp $(SRCDIR)panel_zoom.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de pleiade.o -----------------------------------------
$(OBJDIR)pleiade.o :   $(SRCDIR)pleiade.cpp $(SRCDIR)pleiade.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de serial.o -----------------------------------------
$(OBJDIR)serial.o :   $(SRCDIR)serial.cpp $(SRCDIR)serial.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de serveur_mgr.o -----------------------------------------
$(OBJDIR)serveur_mgr.o :   $(SRCDIR)serveur_mgr.cpp $(SRCDIR)serveur_mgr.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)timer.h $(SRCDIR)camera_mgr.h $(SRCDIR)camera.h $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h $(SRCDIR)star_compare.h $(SRCDIR)captures.h $(SRCDIR)capture.h $(SRCDIR)panel_capture.h $(SRCDIR)fits.h $(SRCDIR)panel_fits.h $(SRCDIR)panel_fits_correction.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de star.o -----------------------------------------
$(OBJDIR)star.o :   $(SRCDIR)star.cpp $(SRCDIR)stars.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de star_catalogue.o -----------------------------------------
$(OBJDIR)star_catalogue.o :   $(SRCDIR)star_catalogue.cpp $(SRCDIR)star_catalogue.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de star_compare.o -----------------------------------------
$(OBJDIR)star_compare.o :   $(SRCDIR)star_compare.cpp $(SRCDIR)star_compare.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)stars.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de stars.o -----------------------------------------
$(OBJDIR)stars.o :   $(SRCDIR)stars.cpp $(SRCDIR)stars.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h $(SRCDIR)star.h $(SRCDIR)panel_zoom.h $(SRCDIR)panel_camera.h $(SRCDIR)catalog.h $(SRCDIR)star_catalogue.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de surveillance.o -----------------------------------------
$(OBJDIR)surveillance.o :   $(SRCDIR)surveillance.cpp $(SRCDIR)surveillance.h $(SRCDIR)main.h $(SRCDIR)MathlibD.h $(SRCDIR)button_callback.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)Singleton.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)var_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de timer.o -----------------------------------------
$(OBJDIR)timer.o :   $(SRCDIR)timer.cpp $(SRCDIR)timer.h $(SRCDIR)Singleton.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de v4l2.o -----------------------------------------
$(OBJDIR)v4l2.o :   $(SRCDIR)v4l2.cpp $(SRCDIR)v4l2.h $(SRCDIR)control.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

#--- Fabrication de var_mgr.o -----------------------------------------
$(OBJDIR)var_mgr.o :   $(SRCDIR)var_mgr.cpp $(SRCDIR)var_mgr.h $(SRCDIR)Singleton.h $(SRCDIR)MathlibD.h $(SRCDIR)main.h $(SRCDIR)button_callback.h $(SRCDIR)surveillance.h $(SRCDIR)panel_courbe.h $(SRCDIR)console.h $(SRCDIR)panel_console_serial.h $(SRCDIR)serial.h $(SRCDIR)serveur_mgr.h $(SRCDIR)timer.h
	@echo ---------   compilation de $@
	$(GPP) -c $< -o $@  $(CFLAGS)

# OK
