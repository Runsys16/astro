#ifndef VARMANAGER_H
#define VARMANAGER_H 1

#include "Singleton.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <assert.h>
#include "MathlibD.h"

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

	typedef std::map<std::string, sVar> dbMap;

	void set(const std::string& name, double val);
	void set(const std::string& name, float val);
	void set(const std::string& name, int val);
	void set(const std::string& name, bool val);
	void set(const std::string& name, const std::string& val);

	inline float    getf(const std::string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].fval;}
	inline int      geti(const std::string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].ival;}
	inline bool     getb(const std::string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].bval;}
	inline string*  gets(const std::string& name)		{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].sval;}
	
	inline dbMap &											getDB()			{return	m_VarDB;}

    inline void setSauve()                                  { bSauve = true; }
    inline void stopSauve()                                 { bSauve = false; }

    void charge();
    void sauve();
	void Destroy();
	
	bool existe( const std::string& );
	void erase( const std::string& );

protected:
    VarManager();
	~VarManager() {Destroy();}
	
	bool        bSauve;

private:
	
	std::map<std::string, sVar>	m_VarDB;

SINGLETON_END()

#endif


