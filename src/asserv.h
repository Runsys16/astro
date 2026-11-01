#ifndef ASSERV_H
#define ASSERV_H 1

#include "main.h"

#include <WindowsManager.h>





class Asservissement
{
private:
	mat3							mTexture2pap;
	vec2							vStarGuide;
	vec2							vRef;
	vec2							ptAD[2];
	vec2							ptDC[2];
	vec3							vAD;
	vec3							vDC;
	
    ivec2							vDeplaceDepuis;
	vec2							vRefSuivi;

	float							fTimeCpt;
	double							dEcart;
	
public:
    Asservissement();
    ~Asservissement();
    
    void                            init();
    void                            load_var( string& name );
    void							commande( int&, int&);
    void							commande( vec3& V );

	void							compute_matrix();
	void							print_ecart();
	void							idleGL(float, vec2&);

inline mat3&						get_matrix()				{ return mTexture2pap; }
inline vec3&						get_vAD()					{ return vAD; }
inline vec3&						get_vDC()					{ return vDC; }
inline vec2&						get_ptAD0()					{ return ptAD[0]; }
inline vec2&						get_ptAD1()					{ return ptAD[1]; }
inline vec2&						get_ptDC0()					{ return ptDC[0]; }
inline vec2&						get_ptDC1()					{ return ptDC[1]; }


inline void							setptAD0( vec2 v )			{ ptAD[0] = v; vAD = vec3(ptAD[1] - ptAD[0], 0.0); }
inline void							setptAD1( vec2 v )			{ ptAD[1] = v; vAD = vec3(ptAD[1] - ptAD[0], 0.0); }

inline void							setptDC0( vec2 v )			{ ptDC[0] = v; vDC = vec3(ptDC[1] - ptDC[0], 0.0); }
inline void							setptDC1( vec2 v )			{ ptDC[1] = v; vDC = vec3(ptDC[1] - ptDC[0], 0.0); }

inline vec2&						getSuivi()					{ return vRefSuivi; }
inline void							setRefSuivi(vec2&v)			{ vRefSuivi = v; }
inline vec2&						getRefSuivi()				{ return vRefSuivi; }
inline ivec2&						getDepuis()					{ return vDeplaceDepuis; }
inline void							setDepuis(ivec2 v)			{ vDeplaceDepuis = v; }
inline void							setDepuis(vec2 v)			{ vDeplaceDepuis = v; }
inline double						getEcart()					{ return dEcart; }

};


#endif
