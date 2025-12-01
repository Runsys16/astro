#include "star_compare.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCompare::StarCompare()
{
    logf_thread( (char*)"Constructeur StarCompare()" );
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

   		//logf( (char*)"Etoile[%02d]  (%0.2f, %0.2f) = %0.2f", i, (float)vVizier.x, (float)vVizier.y, (float)l );
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
	
   	int n = pStars->size();
	//--------------------------
	// Pour tt les etoiles star
   	for( int i=0; i<n; i++ )
   	{
   		//--------------------------
   		// recupere l'etoile star
   		vec2 v;
   		pStars->get(i)->getPos(v);
   		//--------------------------
   		// comparaison avec vizier idx != -1 != echec
   		int idx = findVizier(v);
		float l = 0.0;
		if ( idx != -1 )	   			l = getDst(v, idx);

   		if ( idx != -1 && l<0.5)
   		{
	   		logf( (char*)"Etoile[%02d]  (%04.2f, %04.2f) = %04.2f   \tidx = %02d/%02d   \t%.0f %.2f~%.2f  ", 
	   				i, (float)v.x, (float)v.y, l, i, idx, (float)pStars->get(i)->getPonderation(), (float)pStars->get(i)->getMagnitude(), (float)pVizier->get(idx)->getMag() );
	   		
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
//--------------------------------------------------------------------------------------------------------------------

