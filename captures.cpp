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
    current_capture = captures.size() - 1;
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
    logf( (char*)"Captures::rotate_capture_moins(%s)  %d/%d", b? (char*)"true":(char*)"false", current_capture, captures.size() );
    //---------------- determine si la capture courante est full
    int  previous = current_capture;
    bool bFull = false;
    
    if ( previous!=-1 )
    {
        bFull = captures[previous]->getFullScreen() ;
    }
    //------------------------------------------------------------
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
    
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::rotate_capture_plus(bool b)
{
    logf( (char*)"Captures::rotate_capture_plus(%s)  %d/%d", b? (char*)"true":(char*)"false", current_capture, captures.size() );
    //---------------- determine si la capture courante est full
    int  previous = current_capture;
    bool bFull = false;
    
    if ( previous!=-1 )
    {
        bFull = captures[previous]->getFullScreen() ;
    }
    //------------------------------------------------------------

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
    
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::resize_icone( Capture* p, int dx, int y, int dxi, int dyi )
{
    p->resize(dx+10,y+10,dxi-20,dyi-20);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::resize_normal(Capture* p, int dx, int dy)
{
    p->resize(10,10,dx-20,dy-20);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::resize_all()
{
    logf( (char*)"Captures::resize_all" );

    bool bFull = false;
    if ( current_capture != -1 )        bFull = captures[current_capture]->getFullScreen();
    logf( (char*)"  %d   full = %s", current_capture, bFull ? (char*)"true":(char*)"false" );
    
    int dx, dy, dxi, dyi;
    float ratio = (float)width/(float)height;
    
    dxi = width / 8;
    dyi = height  / 6;

    dx = width - dxi;
    dy = height; 

    int DY;
    int y=10;

    int n = captures.size();

    if ( n>1 )      DY = height / (n-1);    
    else            DY = height / (n);    

    for (int i=0; i<n; i++)
    {
        Capture* p = captures[i];
        
        if ( i==current_capture )
        {
            p->show();
            p->onTop();
            p->setIcone(false);
            p->setFullScreen(bFullPreview);
            if ( bFullPreview )             p->fullscreen();
            else
            if ( bIcones )
            {
                resize_icone( p, dx, y, dxi, dyi );
                y += DY;
                p->setIcone(true);
                p->setFullScreen(false);
                if ( bShowIcones )      p->show();
                else                    p->hide();
                p->onTop();
                
            }
            else    resize_normal( p, dx, dy);
            
            logf( (char*)"  %d icone %s", i, captures[i]->getIcone() ? (char*)"true":(char*)"false" );
        }
    } 

    // Affiche les icones
    for (int i=0; i<n; i++)
    {
        Capture* p = captures[i];
        
        if ( i!=current_capture )
        {
            if ( bShowIcones )      p->show();
            else                    p->hide();

            resize_icone( p, dx, y, dxi, dyi );
            p->onTop();
            y += DY;
            p->setIcone(true);
            p->setFullScreen(false);
            
            logf( (char*)"  %d icone %s", i, captures[i]->getIcone() ? (char*)"true":(char*)"false" );
        }        
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
	    logf( (char*)"Touche pgup !!" );
        Capture&  c = *captures[n];

        float e = c.getEchelle() / 0.9;
        c.setEchelle(e);
        
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// pgdown
	case 105:	
	    {
        if ( n == -1 )          return;

	    logf( (char*)"Touche pgdown !!" );
        Capture&  c = *captures[n];

        float e = c.getEchelle() * 0.9;
        c.setEchelle(e);

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

        int n = captures.size();
        if ( n == 0 )           current_capture  = -1;
        else
        {
            current_capture = ++current_capture % n;
        }
    
        showIcones();
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
    
    logf( (char*)"  trouve  -> %s", (char*)p->getBasename().c_str() );

    bool bFull = false;
    if ( current_capture != -1 )            bFull = captures[current_capture]->getFullScreen();
    
    current_capture = i;
    captures[current_capture]->setFullScreen(bFull);

    resize_all();
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
        captures[current_capture]->setIcone(true);
        captures[current_capture]->setFullScreen(false);
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
void Captures::setCurrent(Capture* p)
{
    for(int i=0; i<captures.size(); i++)
    {
        if ( captures[i] == p )     current_capture = i;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::fullscreen()
{
    bFullPreview = !bFullPreview;
    logf( (char*)"Captures::fullscreen  => %s", bFullPreview ? (char*)"true":(char*)"false" );

    if ( bFullPreview )              bShowIcones = false;
    else                            bShowIcones = true;

    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::showIcones()
{
    bShowIcones = true;
    
    resize_all();
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::hideIcones()
{
    bShowIcones = false;
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::switchAffIcones()
{
    bShowIcones = !bShowIcones;
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




