#include "star_compare.h"
//--------------------------------------------------------------------------------------------------------------------
//#define RECHERCHE_VIZIER 1
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCompare::StarCompare()
{
    logf( (char*)"Constructeur StarCompare()" );
	init(NULL, NULL);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCompare::StarCompare( Stars* pS, Catalog* pC )
{
    logf_thread( (char*)"Constructeur StarCompare()" );
	init(pS, pC);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCompare::~StarCompare()
{
    logf_thread( (char*)"Destructeur StarCompare()" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::init( Stars* pS, Catalog* pC )
{
    //logf( (char*)"StarCompare::init()" );
    pStars  = pS;
    pVizier = pC;
    
    magMax = 20.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare la distance de la coordonnée avec la position des etoiles vizier
// Retourne l'index de l'etoile trouvee la plus proche et inferieur à 20px
//
//--------------------------------------------------------------------------------------------------------------------
#define _MIN 15.0
//-----------------------------------------------------------
int StarCompare::findVizier(vec2& vStar)
{
   	int			n		= pVizier->size();
   	int			idx		= -1;
   	double		min		= 9999999.0;
   	
	log_tab(true);
	
	//logf( (char*)"Pour %d etoiles vizier", n );
	//--------------------------
	// Pour tt les etoiles de vizier
   	for( int i=0; i<n; i++ )
   	{
   		//--------------------------
   		// recupere l'etoile vizier
   		vec2 vVizier;
   		pVizier->get(i)->getTex(vVizier);
   		//--------------------------
   		// hors ecran ?  ( panelCamera  => idle() =>  updateVizizePos() )
   		if (vVizier.x == -1.0 )		continue;
   		//--------------------------
   		// calcul de la distance a l'etoile Star
   		//screen2tex( vVizier );
   		vec2 v = vVizier - vStar;
   		double l = v.length();

		#ifdef RECHERCHE_VIZIER
   		logf( (char*)"Etoile[%02d]  (%0.2f, %0.2f) = %0.2f/%0.2f", i, (float)vVizier.x, (float)vVizier.y, (float)l, (float)min );
   		#endif
   		//--------------------------
		// distance inferieur à min
		if ( l < min )		{
			min = l;
			idx = i;
		}   		
   	}
	log_tab(false);

	//--------------------------
	// distance inferieur à 20
   	if ( min > _MIN )		idx = -1;
   	return idx;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare la distance de la coordonnée avec la position des etoiles vizier
// Retourne l'index de l'etoile trouvee la plus proche et inferieur à 20px
//
//--------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------
double StarCompare::getDst(vec2& vStar, int idx )
{

	//--------------------------
	// recupere l'etoile vizier
	vec2 vVizier;
	pVizier->get(idx)->getTex(vVizier);
	//--------------------------
	// calcul de la distance a l'etoile Star
	//screen2tex( vVizier );
	vec2 v = vVizier - vStar;
	double l = v.length();

   	return l;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Remplace le caractere . (point)  par , (virgule)   (fichier CSV)
// 
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::replace(string & str)
{
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '.') {
		    str[i] = ',';
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::compareStar( bool bSave )
{
   	logf( (char*)"StarCompare::compareStar(%s)", BOOL2STR(bSave) );
   	log_tab(true);

    ofstream fichier;
    
    if ( bSave )
    {
		string sFilename = "/home/rene/.astropilot/gnuplot/etoiles-mag.csv";
		fichier.open(sFilename);
		if ( !fichier ) 
		{
			logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)sFilename.c_str() );
			return;
		}
		fichier << "\"luminance\";\"calculées\";\"vizier\"\n";
	}
	
	
   	magMax = -100.0;
   	magMin =  100.0;

   	lumMax = -1000000.0;
   	lumMin =  1000000.0;
   	
   	tStar.clear();
   	tVizi.clear();
	cmpViziStar.clear();

   	int n = pStars->size();
	//--------------------------
	// Pour tt les etoiles star
   	for( int i=0; i<n; i++ )
   	{
   		//--------------------------
   		// recupere l'etoile star
   		vec2 v;
   		pStars->get(i)->getPos(v);
   		if ( v == vec2(0.0,0.0) )		 continue;
   		//--------------------------
   		// comparaison avec vizier idx != -1 != echec
		#ifdef RECHERCHE_VIZIER
   		logf( (char*)"Recherche Etoile[%02d]  (%04.2f, %04.2f)", i, v.x, v.y ); 
   		#endif
   		int idx = findVizier(v);

		double pp = pStars->get(i)->getPonderation();
		if ( pp <   100.0 )		  	continue;
		if ( pp > 35000.0 )		  	continue;
		if ( doublon(pp, i) )		continue;

		float l = 0.0;
		if ( idx != -1 )	   			l = getDst(v, idx);

   		if ( idx != -1 && l<5.0)
   		{
		#ifdef RECHERCHE_VIZIER
	   		logf( (char*)"Etoile[%02d]  (%04.2f, %04.2f) = %04.2f   \tidx = %02d/%02d   \t%.0f %.2f~%.2f  ", 
	   				i, (float)v.x, (float)v.y, l, i, idx, (float)pStars->get(i)->getPonderation(), (float)pStars->get(i)->getMagnitude(), (float)pVizier->get(idx)->getMag() );
		#endif
			//---------------------------------------------
			// Calul le min et le max
			if ( pVizier->get(idx)->getMag() > magMax )	   			magMax = pVizier->get(idx)->getMag();
			if ( pVizier->get(idx)->getMag() < magMin )	   			magMin = pVizier->get(idx)->getMag();
			
			if ( pStars->get(i)->getMagnitude() > magMax )			magMax = pStars->get(i)->getMagnitude();
			if ( pStars->get(i)->getMagnitude() < magMin )			magMin = pStars->get(i)->getMagnitude();
			if ( pStars->get(i)->getPonderation() > lumMax )	   	lumMax = pStars->get(i)->getPonderation();
			if ( pStars->get(i)->getPonderation() < lumMin )	   	lumMin = pStars->get(i)->getPonderation();
			
			//---------------------------------------------
			// Sauvegarde le resutta
			tVizi.push_back( pVizier->get(idx) );
			tStar.push_back( pStars->get(i) );
			cmpViziStar.push_back( ivec2(idx, i) );
			
	   		if ( bSave  )
	   		{
		   		//--------------------------
		   		// Ecriture dans le fichier
				string sout;
				sout = to_string(  pStars->get(i)->getPonderation() );			//replace( sout );
				fichier << sout <<";";
				sout = to_string(  pStars->get(i)->getMagnitude() );				//replace( sout );
				fichier << sout <<";";
				sout = to_string( pVizier->get(idx)->getMag() );				//replace( sout );
				fichier << sout <<"\n";
	   		}
   		}
   		/*
   		//--------------------------
   		// comparaison avec vizier idx = -1 = echec
   		else
   		{
	   		if ( !bSave )
	   		{	   			
	   			logf( (char*)"Etoile[%02d]  (%04.2f, %04.2f) = %04.2f   \tidx = %02d", 
	   				i, (float)v.x, (float)v.y, l, idx );
	   		}
   		}
   		*/
   	}

	if ( bSave )		fichier.close();

	logf( (char*)"magMax = %0.2lf", magMax );
	logf( (char*)"magMin = %0.2lf", magMin );
	logf( (char*)"lumMax = %0.2lf", lumMax );
	logf( (char*)"lumMin = %0.2lf", lumMin );

	compute_moyenne();
	compute_ecart_type();

   	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::compareStar()
{
   	log( (char*)"StarCompare::compareStar()");
	compareStar( false );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::saveCompareStar()
{
   	log( (char*)"StarCompare::saveCompareStar()");
	compareStar( true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::resetDelta()
{
   	log( (char*)"StarCompare::resetDelta()");
	int n = pStars->size();
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
double StarCompare::computeDelta()
{
	if ( magMax == 0.0 )			return 0.0;
   	log( (char*)"StarCompare::computeDelta()");
   	int n = tVizi.size();
   	double tot = 0.0;
   	int err = 0;
   	
	for( int i=0; i<n; i++ )
	{
		double mv = tVizi[i]->getMag();
		double ms = tStar[i]->getMagnitude();
		tot +=  (mv - ms);
	}

	double delta = tot / (double)n;	
	int m = 0;
		
	for( int i=0; i<n; i++ )
	{
		double mv = tVizi[i]->getMag();
		double ms = tStar[i]->getMagnitude() + delta;
		//if ( (mv - ms) < 0.5 )	
		{
			tot +=  (mv - ms);
			m++;
		}
	}
	
	
	return tot / (double)m;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Retourne la correspondance Star
//
//--------------------------------------------------------------------------------------------------------------------
bool StarCompare::doublon(double pond, int idx)
{
	for( int i=0; i<pStars->size(); i++ )
	{
		if ( (pStars->get(i)->getPonderation() == pond) && idx != i )		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Retourne la correspondance Star
//
//--------------------------------------------------------------------------------------------------------------------
int StarCompare::getTwinStar(int ii)
{
	for( int i=0; i<cmpViziStar.size(); i++ )
	{
		if ( cmpViziStar[i].x == ii )		return cmpViziStar[i].y;
	}
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Retourne la correspondance Star
//
//--------------------------------------------------------------------------------------------------------------------
int StarCompare::getTwinVizier(int ii)
{
	for( int i=0; i<cmpViziStar.size(); i++ )
	{
		if ( cmpViziStar[i].y == ii )		return cmpViziStar[i].x;
	}
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Calcul la moyenne des ecarts de magnitude
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::compute_moyenne()
{
	dMoyen = 0.0;
	 
	for( int i=0; i<cmpViziStar.size(); i++ )
	{
		//int iv = cmpViziStar[i].x;
		//int is = cmpViziStar[i].y;
		double mv = tVizi[i]->getMag();
		double ms = tStar[i]->getMagnitude();
		dMoyen += abs(mv - ms );
	}

	dMoyen /= (double)cmpViziStar.size();
}
//--------------------------------------------------------------------------------------------------------------------
//
// Calcul l' ecart type des ecarts de magnitude
//
//--------------------------------------------------------------------------------------------------------------------
void StarCompare::compute_ecart_type()
{
	dEcart = 0.0;
	 
	for( int i=0; i<cmpViziStar.size(); i++ )
	{
		//int iv = cmpViziStar[i].x;
		//int is = cmpViziStar[i].y;
		double mv = tVizi[i]->getMag();
		double ms = tStar[i]->getMagnitude();
		double M = abs(mv-ms) - dMoyen;
		dEcart += M * M;
	}

	dEcart /= (double)cmpViziStar.size();
	dEcart = sqrt( dEcart );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

