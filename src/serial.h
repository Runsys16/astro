#ifndef SERIAL_H
#define SERIAL_H  1

#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h> 
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

#include <string>
#include <thread>

#include "main.h"

extern float fTimeMili;
#define VER_ARDUINO		"1.0.1"


using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

SINGLETON_BEGIN( Serial )

    protected:
        string          dev_name;
        int             baudrate;
        int             fd;
        thread          th_serial;
        thread          th_sound;
        
        char            buffer[1024];
        int             idx;
        int             nbZero;
        
        bool            bPrintInfo;
        float           fTimeOut;
        
        bool            bFree;
        
        vector<string>  tCommandes;
        string			sVersionArduino;

		bool			bVersionArduino;
		string			sVersionArduinoValable;
		
		bool            bConnect;

    private:
        void            emet_commande();

    public :
        Serial();

        void            init( string );
        void            sopen();
        void            sclose();

        int             write_byte(char b);
        int             write_string(const char* str, bool);
        int             write_string(const char* str);
        void            push_cmd(string&);
        void            push_cmd(char *);

        void            read_thread();
        void            sound_thread();
        void            sound();

        void            start_thread();
        void            reset();
	    void			testVersionArduino();
	    
inline  bool            isConnect()                             { return bConnect; }
inline  bool            getFree()                               { return bFree; }

SINGLETON_END()

#endif

