#!/usr/bin/python3
#-----------------------------------------------------------------------------
# depend.py
#-----------------------------------------------------------------------------
# Creation des dependance pour le fichier makefile
# this script run in src directory
#-----------------------------------------------------------------------------
#  exemple de ligne :
#		$(OBJDIR)Effects.o :   $(SRCDIR)Effects.cpp 
#			@echo ---------   compilation de $@
#			$(CPP) -c $< -o $@  $(CFLAGS)
#-----------------------------------------------------------------------------
# le programme liste toutes les entrees du repertoite courant.
#-----------------------------------------------------------------------------


import os
import sys
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
tab = ""
DEBUG=False
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
def log( str_log ):
	global tab
	
	if not DEBUG:
		return
	print( tab + str_log )
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
def log_tab( b ):
	global tab
	
	if ( b ):
		tab = tab + "   "
	else:
		tab = tab[:-3]
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
def list_dir( name="." ):
	for dirname, dirnames, filenames in os.walk(name):
		log( "-------------------------")
		log( dirname )
		#print( "-------------------------")
		#print filenames
		#print( "-------------------------")
		#print dirnames
		
		listfiles = []

		for filename in filenames:
			if filename.find(".cpp") != -1:
				#listfiles.append(os.path.join(dirname, filename))
				listfiles.append( filename )
			
		return listfiles
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
def have_file( includes, include ):
	return (include in includes)
#-----------------------------------------------------------------------------
#
# Recursif
#
#-----------------------------------------------------------------------------
def find_header( filename, dirname="./", includes = [], ext_dirname = "" ):
	log( '-------------------------------' )
	log( 'find header "%s"' % (dirname+ext_dirname+filename) )
	f = ""
	try:
		f = open( dirname+ext_dirname+filename )
	except OSError:
		#print( e )
		log( "Erreur d'ouverture de : \"%s\"" % (dirname+ext_dirname+filename) )
		sys.exc_info()
		sys.exit(1)
	#else:
	#	print( "OK \"%s\"" % (dirname+ext_dirname+filename) )
	#	print (f )

	i = 0

	#log( "%d" % len(f.readlines()) )
	for line in f.readlines():
		log( "-ligne %d %s" % (i, line[:-1]) )
		i += 1
		
		if line.find("#include \"") != -1 and line.split("#include \"")[0].find("//") == -1:
			#print( "------------" )
			include_name = line.split("\"")[1].split( dirname )[-1]
			include_name = dirname + ext_dirname + include_name

			try:
				ext_dirname = include_name.split(dirname)[1].split("/")[-2] + "/"
			except:
				ext_dirname = ""
			
			if ( ext_dirname != "" ):
				include_name = include_name.split(ext_dirname)[-1]
			
			make_name = ext_dirname + include_name.split(dirname)[-1]
			log( "MAKENAME : %s" % (make_name) )
			log( "Recursif dir  : %s" % dirname )
			log( "Recursif ext  : %s" % ext_dirname )
			log( "Recursif file : %s" % include_name )
			log( "Recursif inc : %s" % ";".join(includes) )
			
			if not have_file( includes, make_name ):
				includes.append( make_name )
				log_tab(True)
				find_header( include_name.split(dirname)[-1], dirname,  includes, ext_dirname)
				log_tab(False)
				


	f.close()
	return includes
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
#
#		MAIN
#
log ( "#-------------------------------------------------------" )	
log ( "#              project" )
log ( "#-------------------------------------------------------" )	
log
source="./"
source="/home/rene/programmes/opengl/video/astro/src/"
for filename in sorted(list_dir(source)):

	#log( "MAIN : parours : %s" % filename )

	try:
		includes = find_header( filename, dirname=source, includes=[] )
	except:
		log( "Erreur  : '%s'" % (filename) )
		sys.exit(1)

	name = filename.split(".cpp")[0]
	
	#----------------------------------------------------
	# print for makefile
	#----------------------------------------------------
	#	for include in includes:
	#	print( "$(SRCDIR)%s " % include ) ,
	include = ""
	if len(includes) != 0:
		include = "$(SRCDIR)" + " $(SRCDIR)".join(includes)
	
	
	print( "#--- Fabrication de %s.o -----------------------------------------" % name )
	print( "$(OBJDIR)%s.o :   $(SRCDIR)%s.cpp %s" % (name, name, include) )


	print
	print( "\t@echo ---------   compilation de $@" )
	print( "\t$(GPP) -c $< -o $@  $(CFLAGS)" )
	print( "" )
	
sys.exit(0)

