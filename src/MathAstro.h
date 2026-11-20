#ifndef MATHASTRO_H
#define MATHASTRO_H  1
//-------------------------------------------------------------
#include <cmath>
//-------------------------------------------------------------
#include "MathlibD.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
// Définition de PI si non défini par le compilateur
#ifndef M_PI
//	#define M_PI		3.14159265358979323846
	#define M_PI		3.141592653589793238462643383279f		// PI
#endif
//-------------------------------------------------------------
// Structure pour stocker les coordonnées horizontales (résultat)
struct st_az {
    double azi;   // En degrés (0° = Nord, 90° = Est, 180° = Sud, 270° = Ouest)
    double hau;	  // En degrés (-90° à +90°)
};
//-------------------------------------------------------------
// Structure pour stocker les coordonnées horizontales (résultat)
struct st_eq {
    double ad;   // Ascenssion droie   en degre
    double dc;   // Declinaison        en degre
};
//-------------------------------------------------------------
struct st_obs {
    double lat_deg;
    double lon_deg;
    double temp_c = 10.0;
    double pressure_hpa = 1010.0;
};
//-------------------------------------------------------------
struct st_nutation {
    double d_psi_deg; // Nutation en longitude (degrés)
    double d_eps_deg; // Nutation en obliquité (degrés)
    double eps_deg;   // Obliquité vraie de l'écliptique (degrés)
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double			normalize_deg( double );
double			to_julian_date( int, int, int, int, int, double );
double			julian_now();
st_nutation		compute_nutation( double);
double			greenwich_apparent_sidereal_time_deg( double );
st_eq			precess_coordinates( st_eq, double, bool );
st_eq			apply_nutation( st_eq, double, bool );
double			apply_refraction( double, double, double );
double			remove_refraction( double, double, double );
st_az			equatorial_to_horizontal( st_eq, st_obs, double, bool );
st_eq			horizontal_to_equatorial( st_az, st_obs, double, bool );

st_eq			versCoordonneesApparentes(st_eq, double );


#endif


