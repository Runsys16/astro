
#include <GL/glut.h>
#include <sys/time.h>
#include <time.h>

#include "timer.h"

#define GETTIME() ((float)glutGet(GLUT_ELAPSED_TIME)/1000)



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float gettime(void)
{
	struct timeval tv;
	struct tm *tm;


	
	gettimeofday( &tv, NULL );
	tm=localtime(&tv.tv_sec);
	float ret = tm->tm_sec;
	ret += tv.tv_usec/1000000.0f;
	return ret;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Timer::Timer() {
	fTimeCompress = 1.0f;
	Start();

	struct timeval tv;
	struct tm *tm;

	gettimeofday( &tv, NULL );
	tm=localtime(&tv.tv_sec);
	fHourTime = tm->tm_hour + ( (float)(tm->tm_min) / 60.0 );
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Timer::Start() {
	fStartTime = GETTIME();
	//fStartTime = gettime();
	fCurrentTime = 0.0f;
	fElapsedTime = 0.0f;
	fReelElapsedTime = 0.0f;
	nFPS = 0;
	frameCount = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Timer::Idle() {

	static GLint TempFPS = 0;	// FPS temporaires						
    static GLfloat fPreviousFPSTime = 0.0f;	

	GLfloat fNewCurrentTime;

//	do
	{
		fNewCurrentTime = GETTIME() - fStartTime;		
		fReelElapsedTime = fNewCurrentTime - fCurrentTime;
	}
//	while(fElapsedTime < (1.0f/60));	// blocage du fps

    ++TempFPS;	// Incrementation du nombre de FPS

	// Si il s'est ecoule plus d'une seconde, on affiche les FPS :
    if( fNewCurrentTime - fPreviousFPSTime > 1.0f ) {
	    fPreviousFPSTime = fNewCurrentTime;
		nFPS = TempFPS;
        TempFPS = 0;
    }

	fCurrentTime = fNewCurrentTime;	// On sauvegarde le temps actuel
	fElapsedTime = fReelElapsedTime * fTimeCompress;
	fHourTime += fReelElapsedTime/3600.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Timer::getGlutTime()
{
	return GETTIME();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

