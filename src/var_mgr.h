#ifndef VARMANAGER_H
#define VARMANAGER_H 1

#include <assert.h>
#include <map>
#include "Singleton.h"
#include "main.h"

using namespace std;



SINGLETON_BEGIN( VarManager )
public:
	struct sVar
	{
	    unsigned char   type; 
	    union 
	    {
	        float       fval;
	        int         ival;
	        bool        bval;
	        string*     sval;
	     };
	 };

	typedef map<string, sVar> dbMap;

	void set(const string& name, double val);
	void set(const string& name, float val);
	void set(const string& name, int val);
	void set(const string& name, bool val);
	void set(const string& name, const string& val);

	inline float    getf(const string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].fval;}
	inline int      geti(const string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].ival;}
	inline bool     getb(const string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].bval;}
	inline string*  gets(const string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].sval;}
	
	inline dbMap &	getDB()								{ return	m_VarDB; }

    inline void setSauve(bool b)						{ bSauve = b; }
    inline void startSauve()							{ bSauve = true; }
    inline void stopSauve()								{ bSauve = false; }
    inline bool getSauve() 								{ return bSauve; }

    void charge();
    void sauve();
	void Destroy();
	
	bool existe( const string& );
	void erase( const string& );

protected:
    VarManager();
	~VarManager() {Destroy();}
	
	bool        bSauve;

private:
	
	map<string, sVar>	m_VarDB;
	string				sFileVar;

SINGLETON_END()

#endif


