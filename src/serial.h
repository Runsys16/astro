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

extern double fTimeMili;
#define VER_ARDUINO		"1.0.7"

typedef union
{
	uint16_t	u;

	struct  	{
		bool bRotAD			: 1;
		bool bRotDC			: 1;
		bool bRotJoyAD		: 1;
		bool bRotJoyDC		: 1;
		bool bRotSiderale	: 1;
		bool bRetStellar	: 1;
		bool bJoy			: 1;
		int	 zero			: 9;
	}b;
} f_bool;






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
        thread          th_write_g;
        thread          th_serial;
        thread          th_sound;
        
        char            buffer[1024];
        int             idx;
        int             nbZero;
        
        bool            bPrintInfo;
        double			fTimeOut;
		double			fTimer10s;
        bool            bFree;
        bool			bOldJoy;
        
        vector<string>  tCommandes;
        string			sVersionArduino;

		bool			bVersionArduino;
		string			sVersionArduinoValable;
		
		bool            bConnect;
		int				nbConnect;

    private:
        void            emet_commande();

    public :
        Serial();
        void            init( string );

        void			idle(double);
        void            sopen();
        void            sclose();

        int             write_byte(char b);
        int             write_string(const char* str, bool);
        int             write_string(const char* str);
        void			write_g_thread();
        void			write_g();
        void            push_cmd(string&);
        void            push_cmd(char *);

        void            to_bin(unsigned, char*, int);
        void            read_thread();
        void            sound_thread();
        void            sound();

        void            start_thread();
        void            reset();
	    void			testVersionArduino();
	    
	    void			print_list();

inline  bool            isCorrectVersion()                      { return bVersionArduino; }
inline  bool            isConnect()                             { return bConnect; }
inline  bool            getFree()                               { return bFree; }

SINGLETON_END()

#endif

