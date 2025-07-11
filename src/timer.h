#ifndef TIMER_H
#define TIMER_H

// -------------------------------
// Gestion du temps
// -------------------------------

#include "Singleton.h"

SINGLETON_BEGIN( Timer )
public:
	void Start();
	void Idle();

	inline float		getCurrentTime()		const	{return fCurrentTime;}	// Temps courant
	inline float		getElapsedTime()		const	{return (fElapsedTime);}	// Temps entre 2 frames
	//inline float getElapsedTime()		const {return (fElapsedTime);}	// Temps entre 2 frames
	inline float		getReelElapsedTime()	const	{return (fReelElapsedTime);}	// Temps entre 2 frames
	inline int			getFPSCounter()			const	{return nFPS;}			// FPS

	inline float*		getvTimeCompress()				{return &fTimeCompress;}			// FPS
	inline void			setTimeCompress(float f)		{fTimeCompress = f;}			// FPS
	inline int*			getvFPSCounter()				{return &nFPS;}			// FPS
	inline float		getHourTime()					{return fHourTime;}			// FPS
	inline long			getFrameCount()					{return frameCount;}			// FPS

protected:
	Timer();

private:
	float				fStartTime;		// date du système lors de l'initialisation de la base de temps
	float				fTimeCompress;
	float				fCurrentTime;	// Temps courant remis à jour à chaque frame
	float				fElapsedTime;
	float				fReelElapsedTime;
	int					nFPS;
	float				fHourTime;
	long				frameCount;

SINGLETON_END()



#endif

