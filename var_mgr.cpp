#include "var_mgr.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
VarManager::VarManager()
{
    bSauve = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void VarManager::set(const std::string& name, float val)
{
    logf((char*)"set float");
    m_VarDB[name].type = 'f';
    m_VarDB[name].fval = val;
    sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void VarManager::set(const std::string& name, int val)
{
    logf((char*)"set integer");
    m_VarDB[name].type = 'i';
    m_VarDB[name].ival = val;
    sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void VarManager::set(const std::string& name, bool val)
{
    logf((char*)"set boolean");
    m_VarDB[name].type = 'b';
    m_VarDB[name].bval = val;
    sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void VarManager::sauve()
{
    if (!bSauve)        return;

    string filename = "/home/rene/.astropilot/var.ini";
    logf( (char*)"Sauvegarde des variables dans '%s'", (char*)filename.c_str() );
    
    std::ofstream fichier;
    fichier.open(filename);
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
    }

    dbMap& db = getDB();

    VarManager::dbMap::iterator p;
    
    string key;
    
    for(p = db.begin(); p!=db.end(); ++p)
    {
        key = string( p->first );
        int c = (int)p->second.type;
        
        //logf((char*)"sauve() %s  type : %d %d", key.c_str(), (int)c, (int)'f' );
        if ( c == (int)'f' )
        {
            float f = p->second.fval;
            //logf( (char*)"  float : %f", f );
            
            fichier << "float " << key << " = " << f << "\n";
        }
        
        else if ( c == (int)'i' )
        {
            int i = p->second.ival;
            //logf( (char*)"  integer : %d", i );
            
            fichier << "int "<< key << " = " << i << "\n";
        }
        
        else if ( c == (int)'b' )
        {
            bool b = p->second.bval;
            string  val;
            if (b)                      val = "TRUE";
            else                        val = "FALSE";

            //logf( (char*)"  bool : %s", val.c_str() );

            fichier << "bool "<< key << " = " << val << "\n";
         }
         
         
    }

    //logf((char*)" ");

    fichier.close();
        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void VarManager::charge()
{
    bool old_sauve = bSauve;
    bSauve = false;
    
    
    string filename = "/home/rene/.astropilot/var.ini";
    logf( (char*)"Lecture des variables de '%s'", (char*)filename.c_str() );
    
    std::ifstream fichier;
    fichier.open(filename);
    
    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
    }
    
    char output[255];
    while (!fichier.eof()) {
        fichier >> output;
        string type = string(output);
        //cout<< type;// << endl;;
        
        fichier >> output;
        string key = string(output);
        //cout<<" "<<key;// << endl;;
        
        fichier >> output;
        fichier >> output;
        string val = string(output);
        //cout<<" = "<< val << endl;;
        
        if ( type.find("float") != string::npos )
        {
            set( key, stof(val) );
        }
        else if ( type.find("int") != string::npos )
        {
            set( key, (int)stof(val) );
        }
        else if ( type.find("bool") != string::npos )
        {
            if ( val.find("TRUE") != string::npos )         set( key, true );
            else                                            set( key, false );
        }
        
    }

    fichier.close();
    
    bSauve = old_sauve;   
}

