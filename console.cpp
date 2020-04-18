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
"    w \t: commande 'n' : stop la monture",
"    P \t: Active/desactive l'affichage de la position (5 fois par seconde)",
"      \t: ('p' arduino)",
"    c \t: Efface la cosole",
"    I \t: La monture attend les coordonnees de reference",
"        via Stellarium Ctrl+1",
"    B \t: Change la couleur du fond",
"    C \t: Spirale de recherche autour de la position actuelle",
"",
"Raccourci Monture",
"-----------------",
"    n \t: stop la monture",
"    g \t: Affiche les positions de la monture et son etat",
"    S \t: Active/Desactive le suivi rotation terre",
"    j \t: Active/Desactive le joystick",
"    M \t: Mode relatif/absolu",
"    m \t: Affiche le mode",
"    r \t: Reset des valeurs courantes (asc. droite , declinaison)",
"    Dn\t: Change la vitesse par",
"    \t  z1, z2, z3, z4",
"    zn\t: Coefficient vitesse joystick",
"      \t  1x 1/5x 1/10x 1/20x",
"    Vn\t: Change la viteese siderale",
"",
"Commande à envoyer avec 'retour chariot'",
"----------------------------------------",
"    xxxx \t: Emulation joystick en asc. droite de xxx",
"    yxxx \t: Emulation joystick en declinaison de xxx",
"",    
"    iaxxx \t: Valeur courante en asc. droite de xxx degres",
"    idxxx \t: Valeur courante en declinaison de xxx degres",
"",    
"    axxx  \t: Deplace la monture de xxx degres en asc. droite",
"    axxxp \t: Deplace la monture de xxx pas    en asc. droite",
"",    
"    dxxx  \t: Deplace la monture de xxx degres en declinaison",
"    dxxxp \t: Deplace la monture de xxx pas    en declinaison",
"",    
"    v     \t: Affiche la vitesse siderale",
"    vxxx  \t: Changement de la vitesse par defaut",
"",    
"    sa    \t: Change sens en ascension droite",
"    sd    \t: Change le sens en declinaison",
"    sA    \t: Change sens en ascension droite du joystick ",
"    sD    \t: Change sens en declinaison du joystick",
"",
"    ss    \t: Change le sens en suivi de rotation",
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
	CONSOLE_MAP_CMD("clear",		cmd_clear )
	CONSOLE_MAP_CMD("exit",			cmd_quit )
	//CONSOLE_MAP_CMD("g",			cmd_null )
	CONSOLE_MAP_CMD("help",			cmd_help )
	//CONSOLE_MAP_CMD("j",			cmd_null )
	//CONSOLE_MAP_CMD("p",			cmd_null )
	//CONSOLE_MAP_CMD("m",			cmd_null )
	//CONSOLE_MAP_CMD("M",			cmd_null )
	//CONSOLE_MAP_CMD("n",			cmd_null )
	CONSOLE_MAP_CMD("quit",			cmd_quit )
	//CONSOLE_MAP_CMD("r",			cmd_null )
	//CONSOLE_MAP_CMD("S",			cmd_null )
	//CONSOLE_MAP_CMD("v",			cmd_null )
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void    Console::callback_cmd(std::string str)
{

    if ( str.length() == 0 )                        return;

    if (!commande( str ) )
    {
        Serial::getInstance().write_string( str.c_str() );
        /*
        string ardui = "DjpmMnrSvad";
        for( int i=0; i<ardui.length(); i++ )
        {
            //logf( (char*)"%c != %c", str[0], ardui[i] );
            if ( str[0] == ardui[i] )
            {
                Serial::getInstance().write_string( str.c_str() );
                break;
            }
        }
        */
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Console::cmd_null()
{
    //Serial::getInstance().write_string( str.c_str() );
    logf( (char*)"Cmd : %s", option.c_str() );
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
void Console::cmd_clear()
{
    PanelConsoleSerial::getInstance().getConsole()->clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Console::commande( string cmd )
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
        return commande( NewCmd );
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
		{
			//pc->affiche( "commande inconnu .... \"" + str + "\"" );
			return false;
	    }
	}
	
	return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


