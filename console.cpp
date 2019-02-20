#include "console.h"


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
Console::Console()
{
    initMap();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Console::initMap()
{
	CONSOLE_MAP_CMD("aide",			cmd_help )
	CONSOLE_MAP_CMD("exit",			cmd_quit )
	CONSOLE_MAP_CMD("g",			cmd_null )
	CONSOLE_MAP_CMD("help",			cmd_help )
	CONSOLE_MAP_CMD("j",			cmd_null )
	CONSOLE_MAP_CMD("p",			cmd_null )
	CONSOLE_MAP_CMD("m",			cmd_null )
	CONSOLE_MAP_CMD("M",			cmd_null )
	CONSOLE_MAP_CMD("n",			cmd_null )
	CONSOLE_MAP_CMD("quit",			cmd_quit )
	CONSOLE_MAP_CMD("r",			cmd_null )
	CONSOLE_MAP_CMD("S",			cmd_null )
	CONSOLE_MAP_CMD("v",			cmd_null )
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void    Console::callback_cmd(std::string str)
{

    if ( str.length() == 0 )                        return;

    commande( str );
    
    Serial::getInstance().write_string( str.c_str() );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Console::cmd_null()
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Console::cmd_help()
{
    PanelConsole*   pc = PanelConsoleSerial::getInstance().getConsole();
    
    for( int i=0; i<sizeof(tHelp); i++ )
    {
        if ( tHelp[i].find("END") != string::npos )     break;

        pc->affiche( tHelp[i].c_str() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Console::cmd_quit()
{
    exit(0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Console::commande( string cmd )
{
    PanelConsole*   pc = PanelConsoleSerial::getInstance().getConsole();
	// Split la fonciton si ';'	
	if ( cmd.find(";") != string::npos ) {
		int i = cmd.find(";");
		string NewCmd = cmd.substr(0,i);
		pc->affiche( NewCmd.c_str() );
		commande( NewCmd );
		
		NewCmd = "" + cmd.substr(i+1,cmd.length());
		pc->affiche( NewCmd.c_str() );
		commande( NewCmd );
		return;
	}
		
	//cout<<"Console::commande()   cmd=" << cmd.c_str() <<endl;

	size_t found;
	option = cmd;
	string str = cmd;

	// Supprilme les espaces de debut
	found = str.find_first_not_of(" ");
	while  (found!=0 && found != -1) 	str=str.substr(found);

	// Tronque qu premier espace
	found = str.find_first_of(" ");
	if  (found!=string::npos)		str=str.substr(0,found);
	
	// recher la fonction
	ConsoleFonctionMap::iterator		it;
	it = FonctionMap.find(str);

	if ( it!=FonctionMap.end()) {
		(this->*(it->second))();
	}
	else	{
		if ( str.find_first_of("#") != 0 )
			pc->affiche( "commande inconnu .... \"" + str + "\"" );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


