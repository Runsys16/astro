#include "catalog.h"
#include "camera_mgr.h"
#include "panel_stdout.h"
//--------------------------------------------------------------------------------------------------------------------
//#define VER_FICHIER_2
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::Catalog()
{
    sFilename = "/home/rene/.astropilot/vizier/vizier.cat";
    bVizCam = true;

    sName = "";
    charge();
    bSauve = true;
    //Camera_mgr::getInstance().setRefCatalog( 56.0, 24.0 );
    //logf( (char*)"Ajout de la reference %0.2f", 6 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::Catalog(bool b)
{
    sFilename = "/home/rene/.astropilot/vizier/vizier.cat";
    bVizCam = true;
    sName = "";
    bSauve = b;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::Catalog(string f)
{
    Catalog();
    sFilename = f;
    bVizCam = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::Catalog(string f, string n)
{
    Catalog();
    sFilename = f;
    sName = n;
    bVizCam = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::~Catalog()
{
    stars.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::add( StarCatalog* p)
{
    stars.push_back(p);
    if ( bSauve )			sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::efface()
{
    logf( (char*)"Catalog::efface()" );
    log_tab(true);

	logf( (char*)"Supression de %d etoiles", stars.size() );


    int n = stars.size();
    if ( n!= 0)
    {
        for ( int i=0; i<n; i++ )
        {
            delete stars[i];
        }
    }    
    stars.clear();

    log_tab(false);
    logf( (char*)"Catalog::efface()" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::sauve()
{
	if ( !bSauve )					return;
    if ( sFilename == "" )        	return;
    
    std::ofstream fichier;
    fichier.open(sFilename, std::ofstream::out | std::ofstream::trunc);
    if ( !fichier ) 
    {
        logf_thread( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)sFilename.c_str() );
        return;
    }

    for(int i=0; i<stars.size(); i++)
    {
        StarCatalog* p = stars[i];
        
        fichier << "fRA "	<< p->fRA	<< " ";
        fichier << "fDE "	<< p->fDE	<< " ";
        fichier << "fMag "	<< p->fMag	<< " ";
        fichier << "name "	<< p->name	<< " ";
        fichier << "fpmRA "	<< p->fpmRA << " ";
        fichier << "feRA "	<< p->feRA	<< " ";
        fichier << "fpmDE "	<< p->fpmDE << " ";
        fichier << "feDE "	<< p->feDE	<< "\n";
    }

    fichier.close();
        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::sauve( string flnm )
{
    logf_thread( (char*)"Catalog::sauve( %s )", flnm.c_str() );

	sFilename = flnm;
	
    if ( sFilename == "" )
    {
        logf_thread( (char*)"[ERROR]impossble d'ouvrir : ''" );
		return;
    }
	bSauve = true;
	sauve();
	bSauve = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Catalog::charge( string flnm )
{
    logf_thread( (char*)"Catalog::charge( %s )", flnm.c_str() );

	sFilename = flnm;
    if ( sFilename == "" )
    {
        logf_thread( (char*)"[ERROR]impossble d'ouvrir : ''" );
		return false;
    }
	bSauve = true;
    bVizCam = false;

	bool ret = charge();

	bSauve = false;
	
	return ret;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Catalog::charge()
{
    //return;
    logf( (char*)"Catalog::charge() Lecture du ficher filename = '%s'", (char*)sFilename.c_str() );

    stars.clear();
    
    std::ifstream fichier;
    fichier.open(sFilename);
    
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)sFilename.c_str() );
        return false;
    }
    
    char output[255];
    int	 nb = 0;
    while (!fichier.eof()) {
        fichier >> output;
        fichier >> output;
        if (fichier.eof())      break;
        
        string val = string(output);
        //log( (char*)val.c_str() );
        double fRA = stof(val.c_str());

        fichier >> output;
        fichier >> output;
        val = string(output);
        double fDE = stof(val.c_str());

        fichier >> output;
        fichier >> output;
        val = string(output);
        double fMag = stof(val.c_str());

        fichier >> output;
        fichier >> output;
        string name = string(output);
        
		double fpmRA	= 0.0;
		double feRA		= 0.0;
		double fpmDE	= 0.0;
		double feDE		= 0.0;

		if ( bVizCam == false )
		{

			fichier >> output;
			fichier >> output;
			val = string(output);
			fpmRA = stof(val.c_str());

			fichier >> output;
			fichier >> output;
			val = string(output);
			feRA = stof(val.c_str());

			fichier >> output;
			fichier >> output;
			val = string(output);
			fpmDE = stof(val.c_str());

			fichier >> output;
			fichier >> output;
			val = string(output);
			feDE = stof(val.c_str());
		}
				
        StarCatalog* p = new StarCatalog(fRA, fDE, fMag, fpmRA, feRA, fpmDE, feDE, name, nb);
        nb++;
        
        //logf( (char*)"Catalog::charge()  Etoile RA=%0.4f DE=%0.4f Mag=%0.4f %s pmRA%0.2lf", fRA, fDE, fMag, name.c_str(), fpmRA );
        stars.push_back(p);

        if ( name.find("65221487868870784") == 0 )
        {   
            Camera_mgr::getInstance().setRefCatalog( fRA, fDE );
            logf( (char*)"Ajout de la reference %0.2f",fMag );
        }
        else
        {
            //Camera_mgr::getInstance().add_catalogue(p);
        }
               
    }

    fichier.close();
	
	logf( (char*)"Catalog::charge() %d etoiles", nb );

    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::list()
{
    //return;
    logf( (char*)"Catalog::charge() Lecture du ficher '%s'", (char*)sFilename.c_str() );

	for( int i; i<stars.size(); i++ )
	{
	
        logf( (char*)"Catalog::charge()  Etoile RA=%0.4f DE=%0.4f Mag=%0.4f %s", 	stars[i]->getRA(),
        																			stars[i]->getDE(),
        																			stars[i]->getMag(),
        																			stars[i]->getName().c_str()
        																			); 
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::affiche_position()
{
    //return;
    logf( (char*)"Catalog::affiche_position()" );

	for( int i; i<stars.size(); i++ )
	{
		stars[i]->affiche_position();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::affiche_magnitude()
{
    //return;
    logf( (char*)"Catalog::affiche_position()" );

	for( int i; i<stars.size(); i++ )
	{
		stars[i]->affiche_magnitude();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::set_camera_reference()
{
    log( (char*)"Catalog::set_camera_reference()" );

	int n		= stars.size();
	for ( int i; i<n; i++ )
	{
		string& name = stars[i]->getName();
        if ( name.find("65221487868870784") == 0 )
        {   
        	double fra = stars[i]->getRA();
        	double fde = stars[i]->getDE();
            Camera_mgr::getInstance().setRefCatalog( fra, fde );
            logf( (char*)"Ajout de la reference mag=%0.2f", (float)stars[i]->getMag() );
            break;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool cmpsc( StarCatalog* p0, StarCatalog* p1 )
{
	return p0->getMag() < p1->getMag();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::compute_print_all_stars()
{
	log( (char*)"Catalog::compute_print_all_stars()" );

	panelStdOut->change_tab_size();
	panelStdOut->add_tab_size( 200 );
	panelStdOut->add_tab_size( 100 );

	sort( stars.begin(), stars.end(), cmpsc );
    log_tab(true);

    int nb = stars.size();
    for( int i=0; i<nb; i++ )
    {
    	StarCatalog * p = stars[i];
        logf( (char*)"%03d - (%0.4lf, %0.4lf)\tmag= %0.2lf\tpm=(%0.2lf, %0.2lf)", i, p->getRA(), p->getDE(), p->getMag(), p->getPmRA(), p->getPmDE() );
    }

    log_tab(false);
	panelStdOut->restaure_tab_size();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

