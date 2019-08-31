#ifndef SURVEILLANCE_H
#define SURVEILLANCE_H

#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>

//#include "captures.h"
#include "main.h"

#include "button_callback.h"
//#include "file_browser.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Surveillance : public ButtonCallBack
{
private:
    int                 inotifyFd, wd;
    thread              th;
    string              dir;
    int                 iState;
    bool                bRun;
    
public:
    //---------------------------------------------------------
    // Gestion du singleton
    //---------------------------------------------------------
	static Surveillance* s_pInstance;	// Instance de la classe
    //---------------------------------------------------------
	Surveillance(Surveillance&);
	void operator =(Surveillance&);
    //---------------------------------------------------------
		inline static Surveillance& getInstance() {
		if (!Surveillance::s_pInstance)
			s_pInstance = new Surveillance;

		return *s_pInstance;
	}
    //---------------------------------------------------------
	inline static void DestroyInstance() {
		if(s_pInstance) delete s_pInstance;
		s_pInstance = 0;
	}
    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    Surveillance();

    void                displayInotifyEvent(struct inotify_event *);
    void                thread_charge_fichier(string, string);
    void                thread_surveille(string);
    void                start(string); 
    inline void         stop()                  { bRun = false; }

    virtual void        callback( bool, int, char* );

};

#endif

