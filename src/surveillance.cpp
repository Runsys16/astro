#include "surveillance.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Surveillance* Surveillance::s_pInstance;	// Instance de la classe
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Surveillance::Surveillance()
{
    bCharge = false;
    bRun    = false;
    dirname = "";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Surveillance::isImage(string name)
{
    if (    name.find( ".jpg")  != string::npos
         || name.find( ".jpeg") != string::npos
         || name.find( ".JPG")  != string::npos
         || name.find( ".fits") != string::npos
         || name.find( ".tga")  != string::npos
         || name.find( ".png")  != string::npos
       //*/
    )
    {
        logf( (char*)"  %s est une image", name.c_str() );
        return true;
    }        
    logf( (char*)"  %s n\'est pas une image", name.c_str() );

    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Surveillance::idleGL()
{
    //if ( bCharge && iState == 2 )
    if ( basename_charge.size() != 0  )
    {
    	logf( (char*)"Surveillance::idleGL()" );
    	log_tab(true);

    	string dir_name = dirname_charge.back();
    	string bas_name = basename_charge.back();
    	
        charge_image( dir_name, bas_name);
        
        logf( (char*)"charge(%s)", bas_name.c_str() );
        logf( (char*)"charge(%s)", dir_name.c_str() );

        iState = -1;
	    bCharge = false;
    	dirname_charge.pop_back();
    	basename_charge.pop_back();
    	

    	log_tab(false);
        return true;
    }

    //bCharge = false;
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Surveillance::displayInotifyEvent(struct inotify_event *i)
{
    string slog = "mask = ";

    if (i->mask & IN_ACCESS)        slog += "IN_ACCESS ";
    if (i->mask & IN_ATTRIB)        slog += "IN_ATTRIB ";
    if (i->mask & IN_CLOSE_NOWRITE) slog += "IN_CLOSE_NOWRITE ";
    if (i->mask & IN_CLOSE_WRITE)   slog += "IN_CLOSE_WRITE ";
    if (i->mask & IN_CREATE)        slog += "IN_CREATE ";
    if (i->mask & IN_DELETE)        slog += "IN_DELETE ";
    if (i->mask & IN_DELETE_SELF)   slog += "IN_DELETE_SELF ";
    if (i->mask & IN_IGNORED)       slog += "IN_IGNORED ";
    if (i->mask & IN_ISDIR)         slog += "IN_ISDIR ";
    if (i->mask & IN_MODIFY)        slog += "IN_MODIFY ";
    if (i->mask & IN_MOVE_SELF)     slog += "IN_MOVE_SELF ";
    if (i->mask & IN_MOVED_FROM)    slog += "IN_MOVED_FROM ";
    if (i->mask & IN_MOVED_TO)      slog += "IN_MOVED_TO ";
    if (i->mask & IN_OPEN)          slog += "IN_OPEN ";
    if (i->mask & IN_Q_OVERFLOW)    slog += "IN_Q_OVERFLOW ";
    if (i->mask & IN_UNMOUNT)       slog += "IN_UNMOUNT ";


    if (i->len > 0)                 
        logf( (char*)"wd=%2d; %s name = %s iState=%d", i->wd, (char*)slog.c_str(), i->name, iState );



    if ( (i->mask & IN_CREATE) && (i->len > 0) ){
        iState = 1;
    }

    if (    ( (i->mask & IN_CLOSE_WRITE) )  && (i->len > 0) && iState == 1)
    {
        iState = 2;
        
        logf( (char*)"  Notification : file ecrit \"%s\"",i->name );
        string f = string(i->name);
        vector<string> vff = split( f, "/" );
        
        int n = vff.size();
        int i;

        for ( i=0; i<(n-1); i++ )
        {
            dirname = dirname + vff[i] + "/";
        }

        basename = string(vff[i]);

        filename = dirname + basename;

        logf( (char*)"  fichier : %s", (char*)filename.c_str() );

        sleep(1);
        if (   isImage(basename) )
        {
		    basename_charge.push_back( basename );
		    dirname_charge.push_back( dirname );
	        logf( (char*)"  push_back(%s, %s )", (char*)dirname.c_str(), (char*)basename.c_str() );
 			bCharge = true;
 			basename = "";
 			//dirname = "";
 	    }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Surveillance::thread_surveille( string dir )
{
    logf( (char*)"Surveillance::thread_surveille()" );
    
    int j;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    //inotifyFd = inotify_init();                 /* Create inotify instance */
    inotifyFd = inotify_init1(IN_CLOEXEC);                 /* Create inotify instance */
    

    if (inotifyFd == -1)        logf( (char*)"[ERREUR] inotify_init");

    /* For each command-line argument, add a watch for all events */

    wd = inotify_add_watch(inotifyFd, (char*)dir.c_str(), IN_ALL_EVENTS);
    if (wd == -1)               logf( (char*)"[ERREUR] inotify_add_watch");

    logf( (char*)"[INFO]  Watching '%s' using wd = %d", (char*)dir.c_str(), wd);
    //dirname = string( dir );

    while(bRun) {                                  /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)           logf( (char*)"read() from inotify fd returned 0!");
        if (numRead == -1)          logf( (char*)"[ERREUR] read");

        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }
    
    inotify_rm_watch(inotifyFd, wd);
    close(inotifyFd);
    logf( (char*)"[INFO] Arret du thread de  surveillance de repertoire" );
    bThread = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Surveillance::start(string dir)
{
	if ( bRun && dirname.compare(dir) == 0 )
	{
	    logf( (char*)"Surveillance::start( \"%s\" )", (char*)dir.c_str() );
	    bRun = false;
	    logf( (char*)"[INFO] Arret de surveillance de repertoire" );
	    return;
	    
	} 

	if ( bRun )
	{
	    bRun = false;
	    logf( (char*)"  Arret de surveillance de repertoire" );
	    //th.terminate();
	    while( bThread ) sleep(1);
	    sleep(1);
	    
	}
	
	
	
    dirname = string( dir );
    bThread = true;
    bRun = true;
    th = std::thread(&Surveillance::thread_surveille, this, dir);
    th.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Surveillance::callback( bool b, int ii, char* str )
{
    logf( (char*)"Surveillance::callback( %s, %d, \"%s\" )", BOOL2STR(b), ii, (char*)str );
    
    if ( b )
    {
        //---------------------------------------------------------
        VarManager&         var = VarManager::getInstance();
    
        vector<string> cc = split( string(str), "/" );
        cc.pop_back();
        string rep = "/";
        for( int i=0; i<cc.size(); i++ )    rep = rep + cc[i] + "/";
        //---------------------------------------------------------
        logf( (char*)"  repertoire de surveillance : %s", (char*) rep.c_str() );
        var.set( "DirSurveillance", rep );
        
        dir = rep;

        iState = -1;
        start(dir);
    }
}


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

