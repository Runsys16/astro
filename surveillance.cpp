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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Surveillance::thread_charge_fichier(string dirname, string basename)
{
    logf( (char*)"5..." );  sleep(1);
    logf( (char*)"4..." );  sleep(1);
    logf( (char*)"3..." );  sleep(1);
    logf( (char*)"2..." );  sleep(1);
    logf( (char*)"1..." );  sleep(1);
    
    change_file(dirname, basename);
    iState = -1;
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
        logf( (char*)"wd=%2d; %s name = %s", i->wd, (char*)slog.c_str(), i->name );




    if ( (i->mask & IN_CREATE) && (i->len > 0) ){
        iState = 1;
    }

    if ( (i->mask & IN_CLOSE_WRITE) && (i->len > 0) && iState == 1)
    {
        iState = 2;
    }
    if ( (i->mask & IN_ATTRIB) && (i->len > 0) && iState == 2)
    {
        //inotify_rm_watch(inotifyFd, wd);
        
        logf( (char*)"  Notification : file ecrit \"%s\"",i->name );
        string filename = string(i->name);
        vector<string> vff = split( filename, "/" );
        
        int n = vff.size();
        string dirname = "/";
        string basename;
        int i;

        for ( i=0; i<(n-1); i++ )
        {
            dirname = dirname + vff[i] + "/";
        }

        basename = vff[i];
        logf( (char*)"  charge(%s, %s )", (char*)dir.c_str(), (char*)basename.c_str() );

        thread t = thread(&Surveillance::thread_charge_fichier, this, dir, basename);
        t.detach();
        
        //iState = 3;
    }
    //return;

    
    
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

    logf( (char*)"  Watching '%s' using wd = %d", (char*)dir.c_str(), wd);

    for (;;) {                                  /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)           logf( (char*)"read() from inotify fd returned 0!");
        if (numRead == -1)          logf( (char*)"[ERREUR] read");

        //logf("  Read %ld bytes from inotify fd", (long) numRead);
        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }
    
    inotify_rm_watch(inotifyFd, wd);
    close(inotifyFd);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Surveillance::start(string dir)
{
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
        vector<string> cc = split( string(str), "/" );
        cc.pop_back();
        string rep = "/";
        for( int i=0; i<cc.size(); i++ )    rep = rep + cc[i] + "/";
        //---------------------------------------------------------
        logf( (char*)"  repertoire de surveillance : %s", (char*) rep.c_str() );
        dir = rep;

        iState = -1;
        start(dir);
    }
}


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

