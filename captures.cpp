#include "captures.h"



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Captures::Captures()
{
    logf((char*)"----------- Constructeur Captures() -----------" );
    current_capture = -1;

    charge();
    rotate_capture_plus(true);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Captures::isMouseOverCapture(int xm, int ym)
{
    int nb = captures.size();
    for( int n=0; n<nb; n++ )
    {
        if ( captures[n]->isMouseOver(xm, ym) )            return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::change_file( string dirname, string filename )
{
    logf( (char*)"Captures::ajoute() ..." );
    logf( (char*)"    %s",(char*)dirname.c_str() );
    logf( (char*)"    %s",(char*)filename.c_str() );
    captures.push_back( new Capture(dirname, filename) );
    sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::update()
{
    //logf( (char*)"Captures::update() ..." );
    return;
    
    int nb = captures.size();

    for( int n=0; n<nb; n++ )
        captures[n]->update();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::rotate_capture_moins(bool b)
{
    int n = captures.size();
    
    if ( n == 0 )           return;

    if ( b != bIcones )
    {
        bIcones = b;
    }
    else
    {
        current_capture--;
        if ( current_capture == -1 )            current_capture = n-1;
    }
    
       
    WindowsManager& wm = WindowsManager::getInstance();
    
    int dx, dy, dxi, dyi;
    float ratio = (float)width/(float)height;
    
    dxi = width / 8;
    dyi = height  / 6;

    dx = width - dxi;
    dy = height; 

    int j = current_capture;
    int DY;

    if ( bIcones )         n++;

    if ( n>1 )      DY = height / (n-1);    
    else            DY = height / (n);    

    if ( bIcones )         n--;

    int y=10;

    for (int i=0; i<n; i++ )
    {
        Capture* p = captures[j];
        
        if ( j==current_capture )
        {
            if ( !bIcones )
            {
                //int xx = (dx-20)
                p->resize(10,10,dx-20,dy-20);
                p->onTop();
                p->setIcone(false);
            }
            else
            {
                p->resize(dx+10,y+10,dxi-20,dyi-20);
                p->onTop();
                y += DY;
                p->setIcone(true);
            }
        }
        else
        {
            p->resize(dx+10,y+10,dxi-20,dyi-20);
            p->onTop();
            y += DY;
            p->setIcone(true);
        }
        j = ++j % n;
    } 
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::rotate_capture_plus(bool b)
{
    int n = captures.size();
    if ( n == 0 )           return;

    if ( b != bIcones )
    {
        bIcones = b;
    }
    else
    {
        current_capture = ++current_capture % n;
    }
       
    WindowsManager& wm = WindowsManager::getInstance();
    
    
    int dx, dy, dxi, dyi;
    float ratio = (float)width/(float)height;
    
    dxi = width / 8;
    dyi = height  / 6;

    dx = width - dxi;
    dy = height; 

    int j = current_capture;
    int DY;

    if ( bIcones )         n++;

    if ( n>1 )      DY = height / (n-1);    
    else            DY = height / (n);    

    if ( bIcones )         n--;

    int y=10;

    for (int i=0; i<n; i++ )
    {
        Capture* p = captures[j];
        
        if ( j==current_capture )
        {
            if ( !bIcones )
            {
                p->resize(10,10,dx-20,dy-20);
                p->onTop();
                p->setIcone(false);
            }
            else
            {
                p->resize(dx+10,y+10,dxi-20,dyi-20);
                p->onTop();
                p->setIcone(true);
                y += DY;
            }
        }
        else
        {
            p->resize(dx+10,y+10,dxi-20,dyi-20);
            p->onTop();
            y += DY;
            p->setIcone(true);
        }
        j = ++j % n;
    } 
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::glutSpecialFunc(int key, int x, int y)	{

    int n = current_capture;
    if ( n == -1)                   return;
    
    
    switch( key)
    {
	// right
	case 102:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

	    logf( (char*)"Touche right !!" );
        float m = c.getCentX() + 10.0;
        c.setCentX( m );

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );
        
        }
		break;
	// left
	case 100:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float m = c.getCentX() - 10.0;
        c.setCentX( m );

	    logf( (char*)"Touche left !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

	    }
		break;
	// up
	case 101:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float m = c.getCentY() - 10.0;
        c.setCentY( m );

	    logf( (char*)"Touche up !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// down
	case 103:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float m = c.getCentY() + 10.0;
        c.setCentY( m );

        float e = c.getEchelle();

	    logf( (char*)"Touche down !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}	
		break;
	// pgup
	case 104:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float e = c.getEchelle() / 0.9;
        c.setEchelle(e);
        
	    logf( (char*)"Touche pgup !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// pgdown
	case 105:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float e = c.getEchelle() * 0.9;
        c.setEchelle(e);

	    logf( (char*)"Touche pgdown !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

        }
		break;
	// home
	case 106:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        c.setEchelle( 1.0 );
        c.setCentX( 0.0 );
        c.setCentY( 0.0 );

	    logf( (char*)"Touche home !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
    default:
        return;
    }
    
    //Capture&  c = *captures[n];
    //c.update();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::fullscreen()
{
    int n = current_capture;
    if ( n != -1 )
    {
        captures[n]->fullscreen();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::ajoute()
{
    logf( (char*)"Captures::ajoute()" );
    captures.push_back( new Capture() );
    sauve();
    current_capture = captures.size() - 1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::ajoute(string filename)
{
    logf( (char*)"Captures::ajoute()" );
    captures.push_back( new Capture(filename) );
    sauve();
    current_capture = captures.size() - 1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::supprime()
{
    int n = captures.size();
    if ( current_capture != -1 )
    {
        Capture* p = captures[current_capture];
        delete p;
        captures.erase(captures.begin()+current_capture);
        sauve();
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::onTop(Capture* p)
{
    logf( (char*)"Captures::onTop(%s)", (char*)p->getBasename().c_str() );

    int nb = captures.size();
    int i;
    for ( i=0; i<nb; i++ )
    {
        if ( captures[i] == p ) break;
    }
    //--------------------------------------------
    if ( i >= nb )          return;
    logf( (char*)"Captures::onTop  -> %s", (char*)p->getBasename().c_str() );
    
    if ( current_capture != -1 )        rotate_capture_plus(true);
    
    current_capture = i;
    rotate_capture_plus(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::findAllStar()
{
    int n = current_capture;
    if ( n != -1 )
    {
        captures[n]->getPreview()->findAllStars();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::deleteAllStars()
{
    int n = current_capture;
    if ( n != -1 )
    {
        captures[n]->getPreview()->deleteAllStars();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::print_list()
{
    int nb = captures.size();
    for( int i=0; i<nb; i++ )
    {
        string filename = captures[i]->getFilename();
        logf( (char*)"   %d - %s", i, (char*)filename.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::sauve(void)
{
    string filename = "/home/rene/.astropilot/captures.txt";
    logf( (char*)"Sauvegarde des valeurs dans '%s'", (char*)filename.c_str() );
    
    std::ofstream fichier;
    fichier.open(filename, std::ios_base::trunc);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    for(int i=0; i<captures.size(); i++)
    {
        fichier << captures[i]->getFilename() << "\n";
    }

    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::charge(void)
{
    string filename = "/home/rene/.astropilot/captures.txt";
    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );
    
    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    string line;
    int nbLigne = 0;
    
    
    while ( getline (fichier, line) )
    {
        //sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );
        //ajoute(line);
        string filename = line;
        captures.push_back( new Capture(filename) );
        current_capture = captures.size() - 1;
    }    
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::position(double ra, double dc)
{
    if (current_capture == -1)              return;
    
    
    captures[current_capture]->getPreview()->getStars()->position(ra, dc);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




