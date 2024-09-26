#include "catalog.h"
#include "camera_mgr.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::Catalog()
{
    sFilename = "/home/rene/.astropilot/vizier.cat";

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
    sFilename = "/home/rene/.astropilot/vizier.cat";
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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Catalog::Catalog(string f, string n)
{
    Catalog();
    sFilename = f;
    sName = n;
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


    int n = stars.size();
    if ( n!= 0)
    {
        for ( int i=0; i<n; i++ )
        {
            logf( (char*)"efface etoile" );

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
    fichier.open(sFilename);
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)sFilename.c_str() );
    }

    for(int i=0; i<stars.size(); i++)
    {
        StarCatalog* p = stars[i];
        
        fichier << "fRA "<< p->fRA << " ";
        fichier << "fDE "<< p->fDE << " ";
        fichier << "fMag "<< p->fMag << " ";
        fichier << "name "<< p->name << "\n";
    }

    fichier.close();
        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Catalog::charge()
{
    //return;
    logf( (char*)"Catalog::charge() Lecture du ficher '%s'", (char*)sFilename.c_str() );

    stars.clear();
    
    std::ifstream fichier;
    fichier.open(sFilename);
    
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)sFilename.c_str() );
        return;
    }
    
    char output[255];
    while (!fichier.eof()) {
        fichier >> output;
        fichier >> output;
        if (fichier.eof())      break;
        string val = string(output);
        log( (char*)val.c_str() );
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
        
        StarCatalog* p = new StarCatalog(fRA, fDE, fMag, name);
        
        logf( (char*)"Catalog::charge()  Etoile RA=%0.4f DE=%0.4f Mag=%0.4f %s", fRA, fDE, fMag, name.c_str() );
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

