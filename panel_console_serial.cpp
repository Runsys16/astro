#include "panel_console_serial.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
string tHelp[] = 
{
"        =============================",
"        COMMANDE ET RACCOURCI CLAVIER",
"        =============================",
"",
"Raccourci Serveur",
"-----------------",
"    w : commande 'n' : stop la monture",
"    P : Active/desactive l'affichage de la position (5 fois par seconde)",
"      : ('p' arduino)",
"    c : Efface la cosole",
"    I : La monture attend les coordonnées de référence",
"        via Stellarium Ctrl+1",
"    B : Change la couleur du fond",
"    C : Spirale de recherche autour de la position actuelle",
"",
"Raccourci Monture",
"-----------------",
"    n : stop la monture",
"    g : Affiche les positions de la monture et son etat",
"    S : Active/Désactive le suivi rotation terre",
"    j : Active/Désactive le joystick",
"    M : Mode relatif/absolu",
"    m : Affiche le mode",
"    r : Reset des valeurs courantes (asc. droite , déclinaison)",
"    z1, z2, z3, z4",
"      : Coefficient vitesse joystick",
"        1x 1/5x 1/10x 1/20x",
"",
"Commande à envoyer avec 'retour chariot'",
"----------------------------------------",
"    xxxx : Emulation joystick en asc. droite de xxx",
"    yxxx : Emulation joystick en déclinaison de xxx",
"",    
"    iaxxx : Valeur courante en asc. droite de xxx degrés",
"    idxxx : Valeur courante en déclinaison de xxx degrés",
"",    
"    axxx  : Deplace la monture de xxx degrés en asc. droite",
"    axxxp : Deplace la monture de xxx pas    en asc. droite",
"",    
"    dxxx  : Deplace la monture de xxx degrés en déclinaison",
"    dxxxp : Deplace la monture de xxx pas    en déclinaison",
"",    
"    v     : Affiche la vitesse siderale",
"    vxxx  : Changement de la vitesse par defaut",
"",    
"    sa    : Change sens en ascension droite",
"    sd    : Change le sens en declinaison",
"    sA    : Change sens en ascension droite du joystick ",
"    sD    : Change sens en declinaison du joystick",
"",
"    ss    : Change le sens en suivi de rotation",
"END"
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Callback::Callback()
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void    Callback::callback_cmd(std::string str)
{
    //logf( (char*)"Callback::callback_cm('%s')", str.c_str() );

    if ( str.length() == 0 )                        return;

    if ( str.find("quit") != string::npos  )        exit(0);
    if ( str.find("clear") != string::npos  )
    {
        PanelConsoleSerial::getInstance().getConsole()->clear();
        return;
    }
    if ( str.find("help") != string::npos  )
    {
        PanelConsoleSerial::getInstance().afficheHelp();
        return;
    }
    
    Serial::getInstance().write_string( str.c_str() );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelConsoleSerial::PanelConsoleSerial()
{
    pw = new PanelWindow();
    pc = new PanelConsole( 100, 5);
    cb = new Callback();
    
    pw->setVisible(true);
    pw->add(pc);
    
	pw->setPosAndSize( 10, 10, 500, 700);
	pc->setPosAndSize( 0, 0, 500, 700);
	pc->setPrompt( "arduino> " );

    pc->setCallBackCmd( cb );
    
    pc->setBackground((char*)"background.tga");

    WindowsManager::getInstance().add(pw);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void    PanelConsoleSerial::afficheHelp()
{
    for( int i=0; i<sizeof(tHelp); i++ )
    {
        if ( tHelp[i].find("END") != string::npos )     break;

        pc->affiche( tHelp[i].c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelConsoleSerial::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();

    if ( wm.getFocus() != pc )
    {
        wm.stopKeyboard();
        //logf( (char*)"PAS Traitement PanelConsoleSerial::keyboard()" );
        return false;
    }
    
    wm.startKeyboard();
    WindowsManager::getInstance().keyboardFunc( key, x, y);
    //logf( (char*)"Traitement PanelConsoleSerial::keyboard()" );
    
	switch(key){ 
	
	case 27:
	    {
        logf( (char*)"Echappe" );
	    }
	    break;

    default:
        {
        //logf((char*)"key: %d", key);
        }
        break;
	}
    
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelConsoleSerial::writeln(char* str)
{
    if ( str[0] == '-' )            return;
    pc->affiche( str );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


