#include "MathAstro.h"
#include <sys/time.h>
#include <array>
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define HEU2DEB 1.0
#define IAU2006
//--------------------------------------------------------------------------------------------------------------------
//
//  0 < angle < 360.0
//
//--------------------------------------------------------------------------------------------------------------------
double normalize_deg(double angle)
{
    angle = fmod(angle, 360.0);

    return (angle < 0.0) ? angle + 360.0 : angle;
}
//--------------------------------------------------------------------------------------------------------------------
//
// --- TEMPS, NUTATION ET PRÉCESSION ---
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//   Calul du temps Julien
//
//--------------------------------------------------------------------------------------------------------------------
double to_julian_date(int year, int month, int day, int hour, int minute, double second)
{
    if (month <= 2) { year -= 1; month += 12; }
    int A				= year / 100;
    int B				= 2 - A + (A / 4);
    double day_fraction = (hour + (minute + second / 60.0) / 60.0) / 24.0;
    
    return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + day_fraction + B - 1524.5;
}
//--------------------------------------------------------------------------------------------------------------------
//
//   Calul du temps Julien
//
//--------------------------------------------------------------------------------------------------------------------
double julian_now()
{
	struct timeval	tv;
	struct tm*		tm;

	gettimeofday( &tv, NULL );
	tm = gmtime(&tv.tv_sec);
	
	double sec = (double)tv.tv_usec / 1000000.0 + (double) tm->tm_sec;

    return to_julian_date(	(int)1900+tm->tm_year,
							(int)tm->tm_mon+1,
							(int)tm->tm_mday,
							(int)tm->tm_hour,
							(int)tm->tm_min,
							(double)sec		);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Calcul de la nutation (précision < 0.5")
//
//--------------------------------------------------------------------------------------------------------------------
st_nutation compute_nutation(double jd)
{
    double T	 		= (jd - 2451545.0) / 36525.0;
    double omega		= DEG2RAD( normalize_deg(125.04452 - 1934.136261 * T) );
    double L			= DEG2RAD( normalize_deg(280.4665 + 36000.7698 * T) );
    double Lp			= DEG2RAD( normalize_deg(218.3165 + 481267.8813 * T) );

    double d_psi_arcsec	= -17.20 * sin(omega) - 1.32 * sin(2 * L)
                          - 0.23 * sin(2 * Lp) + 0.21 * sin(2 * omega);
    double d_eps_arcsec	=  9.20 * cos(omega) + 0.57 * cos(2 * L)
                          + 0.10 * cos(2 * Lp) - 0.09 * cos(2 * omega);

    double eps0_arcsec	= 84381.448 - 46.8150 * T - 0.00059 * T * T + 0.001813 * T * T * T;

    return {
        d_psi_arcsec / 3600.0,
        d_eps_arcsec / 3600.0,
        (eps0_arcsec + d_eps_arcsec) / 3600.0
    };
}
//--------------------------------------------------------------------------------------------------------------------
//
// Temps Sidéral Apparent de Greenwich (GAST) en degrés (inclut l'équation des équinoxes)
//
//--------------------------------------------------------------------------------------------------------------------
double greenwich_apparent_sidereal_time_deg(double jd)
{
    double T = (jd - 2451545.0) / 36525.0;
    double gmst = 280.46061837 + 360.98564736629 * (jd - 2451545.0)
                  + 0.000387933 * T * T - (T * T * T) / 38710000.0;
    st_nutation nut = compute_nutation(jd);
    double eq_equinoxes = DEG2RAD( nut.d_psi_deg * cos(nut.eps_deg ));
  
    return normalize_deg(gmst + eq_equinoxes);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Précession rigoureuse (direction = +1 pour J2000->JNow, -1 pour JNow->J2000)
//
//--------------------------------------------------------------------------------------------------------------------
st_eq precess_coordinates(st_eq coord, double jd, bool to_jnow = true)
{
    double T		= (jd - 2451545.0) / 36525.0;
    double T2		= T * T;
    double T3		= T2 * T;

    double zeta		= DEG2RAD( (2306.2181 * T + 0.30188 * T2 + 0.017998 * T3) / 3600.0 );
    double z		= DEG2RAD( (2306.2181 * T + 1.09468 * T2 + 0.018203 * T3) / 3600.0 );
    double theta	= DEG2RAD( (2004.3109 * T - 0.42665 * T2 - 0.041833 * T3) / 3600.0 );

    if (!to_jnow) {
        // Inversion des paramètres de précession
        swap(zeta, z);
        zeta = -zeta;
        z = -z;
        theta = -theta;
    }

    double ra0		= DEG2RAD(  (coord.ad * HEU2DEB) );
    double dec0		= DEG2RAD( coord.dc );

    double A		= cos(dec0) * sin(ra0 + zeta);
    double B		= cos(theta) * cos(dec0) * cos(ra0 + zeta) - sin(theta) * sin(dec0);
    double C		= sin(theta) * cos(dec0) * cos(ra0 + zeta) + cos(theta) * sin(dec0);

    double ra_out  = atan2(A, B) + z;
    double dec_out = asin(clamp(C, -1.0, 1.0));

    return { normalize_deg(RAD2DEG( ra_out )) / HEU2DEB, RAD2DEG( dec_out ) };
}
//--------------------------------------------------------------------------------------------------------------------
//
// Application de la nutation aux coordonnées équatoriales (JNow moyen -> JNow apparent)
//
//--------------------------------------------------------------------------------------------------------------------
st_eq apply_nutation(st_eq mean_jnow, double jd, bool forward = true)
{
    st_nutation nut		= compute_nutation(jd);
    double ra_rad		= DEG2RAD( (mean_jnow.ad * HEU2DEB) );
    double dec_rad		= DEG2RAD( mean_jnow.dc );
    double eps_rad		= DEG2RAD( nut.eps_deg );

    double d_ra_deg		= (cos(eps_rad) + sin(eps_rad) * sin(ra_rad) * tan(dec_rad)) * nut.d_psi_deg
                       - (cos(ra_rad) * tan(dec_rad)) * nut.d_eps_deg;
    double d_dec_deg	= (sin(eps_rad) * cos(ra_rad)) * nut.d_psi_deg
                       + sin(ra_rad) * nut.d_eps_deg;

    double sign = forward ? 1.0 : -1.0;
    
    return {
        normalize_deg(mean_jnow.ad * HEU2DEB + sign * d_ra_deg) / HEU2DEB,
        mean_jnow.dc + sign * d_dec_deg
    };
}
//--------------------------------------------------------------------------------------------------------------------
//
// --- RÉFRACTION ATMOSPHÉRIQUE (DIRECTE & INVERSE) ---
//
//--------------------------------------------------------------------------------------------------------------------
double apply_refraction(double true_alt_deg, double temp_c, double pressure_hpa)
{
    if (true_alt_deg < -1.0)		return true_alt_deg;
    
    double	R_arcmin = DEG2RAD( 1.02 / tan((true_alt_deg + 10.3 / (true_alt_deg + 5.11)) ));
    		R_arcmin *= (pressure_hpa / 1010.0) * (283.0 / (273.0 + temp_c));

    return true_alt_deg + (R_arcmin / 60.0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double remove_refraction(double app_alt_deg, double temp_c, double pressure_hpa)
{
    if (app_alt_deg < -1.0) return app_alt_deg;

    // Formule de Bennett pour passer de la hauteur apparente à la hauteur vraie
    double 	R_arcmin = DEG2RAD( 1.0 / tan((app_alt_deg + 7.31 / (app_alt_deg + 4.4)) ));
    		R_arcmin *= (pressure_hpa / 1010.0) * (283.0 / (273.0 + temp_c));
    		
    return app_alt_deg - (R_arcmin / 60.0);
}
//--------------------------------------------------------------------------------------------------------------------
//
// --- CONVERSIONS PRINCIPALES ---
//
//--------------------------------------------------------------------------------------------------------------------
st_az equatorial_to_horizontal(st_eq j2000, st_obs obs, double jd, bool with_refraction = true)
{
	#ifdef IAU2006
		st_eq app_jnow	= versCoordonneesApparentes( j2000, jd );
	#else
		st_eq mean_jnow = precess_coordinates( j2000, jd, true);
		st_eq app_jnow  = apply_nutation(mean_jnow, jd, true);
    #endif

    double last_deg = normalize_deg( greenwich_apparent_sidereal_time_deg(jd) + obs.lon_deg );
    double H        = DEG2RAD( normalize_deg(last_deg - (app_jnow.ad * HEU2DEB)) );
    double dec      = DEG2RAD( app_jnow.dc );
    double lat      = DEG2RAD( obs.lat_deg );

    double sin_alt	= sin(lat) * sin(dec) + cos(lat) * cos(dec) * cos(H);
    double alt_deg	= RAD2DEG( asin(clamp(sin_alt, -1.0, 1.0)) );

    double y		= sin(H);
    double x		= cos(H) * sin(lat) - tan(dec) * cos(lat);
    double az_deg	= normalize_deg(RAD2DEG( atan2(y, x) ) + 180.0);

    if (with_refraction) {
        alt_deg = apply_refraction(alt_deg, obs.temp_c, obs.pressure_hpa);
    }
    
    return { az_deg, alt_deg };
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
st_eq horizontal_to_equatorial(st_az hor, st_obs obs, double jd, bool has_refraction = true)
{
    double true_alt_deg = has_refraction
        ? remove_refraction(hor.hau , obs.temp_c, obs.pressure_hpa)
        : hor.hau;

    double alt			= DEG2RAD( true_alt_deg );
    // Reconvertir l'azimut Nord->Est en Sud->Ouest pour les formules sphériques
    double A			= DEG2RAD( normalize_deg(hor.azi - 180.0) );
    double lat			= DEG2RAD( obs.lat_deg );

    double sin_dec		= sin(lat) * sin(alt) - cos(lat) * cos(alt) * cos(A);
    double dec_deg		= RAD2DEG( asin(clamp(sin_dec, -1.0, 1.0)) );

    double y			= sin(A);
    double x			= cos(A) * sin(lat) + tan(alt) * cos(lat);
    double ha_deg 		= RAD2DEG( normalize_deg(atan2(y, x) ));

    double last_deg		= normalize_deg(greenwich_apparent_sidereal_time_deg(jd) + obs.lon_deg);
    double ra_app_hours	= normalize_deg(last_deg - ha_deg) / HEU2DEB;

    // Retrait de la nutation puis précession inverse vers J2000.0
    st_eq app_jnow  = { ra_app_hours, dec_deg };
    st_eq mean_jnow = apply_nutation(app_jnow, jd, false);
    
    return precess_coordinates(mean_jnow, jd, false);
}
//********************************************************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************

constexpr double ARCSEC2RAD = (M_PI / 180.0 ) / 3600.0;

using Matrice3_3 = array<array<double, 3>, 3>;



//--------------------------------------------------------------------------------------------------------------------
//
// Multiplication de deux matrices 3x3
//
//--------------------------------------------------------------------------------------------------------------------
Matrice3_3 multiplierMatrices(const Matrice3_3& A, const Matrice3_3& B)
{
    Matrice3_3 C{};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}
//--------------------------------------------------------------------------------------------------------------------
//
// 1. Matrice de précession IAU 2006 (Capitaine et al. 2003, polynômes de degré 5)
//
//--------------------------------------------------------------------------------------------------------------------
Matrice3_3 matricePrecessionIAU2006(double T)
{
    // Angles en secondes d'arc (incluant le décalage de repère GCRS -> J2000)
    double zeta		= (2.650545 + T * (2306.083227 + T * (0.2988499 + T * (0.01801828 + T * (-0.000005971 - 0.0000003173 * T))))) * ARCSEC2RAD;
    double z		= (-2.650545 + T * (2306.077181 + T * (1.0927348 + T * (0.01826837 + T * (-0.000028596 + 0.0000002904 * T))))) * ARCSEC2RAD;
    double theta	= (T * (2004.191903 + T * (-0.4294934 + T * (-0.04182264 + T * (-0.000007089 - 0.0000001274 * T))))) * ARCSEC2RAD;

    double cz 		= cos(zeta), sz = sin(zeta);
    double cZ		= cos(z),    sZ = sin(z);
    double ct		= cos(theta), st = sin(theta);

    // P = Rz(-z) * Ry(theta) * Rz(-zeta)
    return {{
        { cz * ct * cZ - sz * sZ, -sz * ct * cZ - cz * sZ, -st * cZ },
        { cz * ct * sZ + sz * cZ, -sz * ct * sZ + cz * cZ, -st * sZ },
        { cz * st,                -sz * st,                 ct      }
    }};
}
//--------------------------------------------------------------------------------------------------------------------
//
// 2. Matrice de nutation (Principaux termes luni-solaires + obliquité IAU 2006)
//
//--------------------------------------------------------------------------------------------------------------------
Matrice3_3 matriceNutation(double T)
{
    // Obliquité moyenne de l'écliptique IAU 2006 (en radians)
    double epsA = (84381.406 - 46.836769 * T - 0.0001831 * T * T + 0.00200340 * T * T * T) * ARCSEC2RAD;

    // Arguments fondamentaux de Delaunay (en radians)
    double l  = DEG2RAD( fmod(134.96340251 + (1717915923.2178 * T + 31.8792 * T * T) / 3600.0, 360.0) ); // Anomalie moyenne Lune
    double lp = DEG2RAD( fmod(357.52910918 + (129596581.0481 * T - 0.5532 * T * T) / 3600.0, 360.0) );  // Anomalie moyenne Soleil
    double F  = DEG2RAD( fmod(93.27209062  + (1739527262.8478 * T - 12.7512 * T * T) / 3600.0, 360.0) ); // Argument de latitude Lune
    double D  = DEG2RAD( fmod(297.85019547 + (1602961601.2090 * T - 6.3706 * T * T) / 3600.0, 360.0) );  // Élongation moyenne Lune
    double Om = DEG2RAD( fmod(125.04455501 + (-6962890.5431 * T + 7.4722 * T * T) / 3600.0, 360.0) );    // Longitude nœud ascendant Lune

    // Table des 15 termes dominants de nutation (couvre > 99,8 % de l'amplitude totale)
    // Format : {l, l', F, D, Om, dPsi_sin (0.1 mas), dPsi_t, dEps_cos (0.1 mas), dEps_t}
    struct TermeNutation { int l, lp, F, D, Om; double psi, psiT, eps, epsT; };
    constexpr TermeNutation table[] = {
        { 0,  0,  0,  0,  1, -172064.161, -174.666,  92052.331,  9.086 }, // Cycle de 18,6 ans (nœud lunaire)
        { 0,  0,  2, -2,  2,  -13170.906,   -1.675,   5730.336, -3.015 }, // Cycle semi-annuel (Soleil)
        { 0,  0,  2,  0,  2,   -2276.413,   -0.234,    978.459, -0.485 }, // Cycle semi-mensuel (Lune)
        { 0,  0,  0,  0,  2,    2074.554,    0.207,   -897.492,  0.470 },
        { 0,  1,  0,  0,  0,    1475.877,   -3.633,     73.871, -0.184 }, // Cycle annuel (excentricité Terre)
        { 0,  1,  2, -2,  2,    -516.821,    1.226,    224.386, -0.677 },
        { 1,  0,  0,  0,  0,     711.159,    0.073,     -6.750,  0.000 }, // Cycle anomalistique Lune
        { 0,  0,  2,  0,  1,    -387.298,   -0.367,    200.728,  0.018 },
        { 1,  0,  2,  0,  2,    -301.461,   -0.036,    129.025, -0.063 },
        { 0, -1,  2, -2,  2,     215.829,   -0.494,    -95.929,  0.299 },
        { 0,  0,  2, -2,  1,     128.227,    0.137,    -68.982, -0.009 },
        {-1,  0,  2,  0,  2,     123.457,    0.011,    -53.311,  0.032 },
        {-1,  0,  0,  2,  0,     156.994,    0.010,    -12.350,  0.000 },
        { 1,  0,  0,  0,  1,      63.110,    0.063,    -33.228,  0.000 },
        {-1,  0,  0,  0,  1,     -57.976,   -0.063,     31.429,  0.000 }
    };

    double dPsi = 0.0; // Nutation en longitude
    double dEps = 0.0; // Nutation en obliquité

    for (const auto& t : table) {
        double arg = t.l * l + t.lp * lp + t.F * F + t.D * D + t.Om * Om;
        dPsi += (t.psi + t.psiT * T) * sin(arg);
        dEps += (t.eps + t.epsT * T) * cos(arg);
    }

    // Conversion des dixièmes de milliseconde d'arc (0.1 mas) en radians
    dPsi *= 0.0001 * ARCSEC2RAD;
    dEps *= 0.0001 * ARCSEC2RAD;

    double eps = epsA + dEps; // Obliquité vraie de la date
    double ceA = cos(epsA), seA = sin(epsA);
    double ce  = cos(eps),  se  = sin(eps);
    double cp  = cos(dPsi), sp  = sin(dPsi);

    // N = Rx(-eps) * Rz(-dPsi) * Rx(epsA)
    return {{
        {  cp,             -sp * ceA,                     -sp * seA                     },
        {  sp * ce,         cp * ce * ceA + se * seA,      cp * ce * seA - se * ceA     },
        {  sp * se,         cp * se * ceA - ce * seA,      cp * se * seA + ce * ceA     }
    }};
}
//--------------------------------------------------------------------------------------------------------------------
//
// 3. Transformation complète : GCRS (J2000.0) -> Équateur et Équinoxe Vrais de la Date
//
//--------------------------------------------------------------------------------------------------------------------
st_eq versCoordonneesApparentes(st_eq posJ2000, double jdCible)
{
    double T = (jdCible - 2451545.0) / 36525.0;

    Matrice3_3 P	= matricePrecessionIAU2006(T);
    Matrice3_3 N	= matriceNutation(T);
    Matrice3_3 NP	= multiplierMatrices(N, P); // Application séquentielle : Nutation * Précession

    double ra0		= DEG2RAD( posJ2000.ad );
    double dec0		= DEG2RAD( posJ2000.dc );

    double x0		= cos(dec0) * cos(ra0);
    double y0		= cos(dec0) * sin(ra0);
    double z0		= sin(dec0);

    double x		= NP[0][0] * x0 + NP[0][1] * y0 + NP[0][2] * z0;
    double y		= NP[1][0] * x0 + NP[1][1] * y0 + NP[1][2] * z0;
    double z		= NP[2][0] * x0 + NP[2][1] * y0 + NP[2][2] * z0;

    double ra = RAD2DEG( atan2(y, x) );
    if (ra < 0.0) ra += 360.0;

    double dec = RAD2DEG( asin( clamp(z, -1.0, 1.0) ) );

    return {ra, dec};
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------

